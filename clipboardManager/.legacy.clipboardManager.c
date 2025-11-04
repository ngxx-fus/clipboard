#include "clipboardManager.h"

xcb_atom_t atom_clipboard = XCB_ATOM_NONE;
xcb_atom_t atom_utf8 = XCB_ATOM_NONE;
xcb_atom_t atom_targets = XCB_ATOM_NONE;
xcb_atom_t atom_png = XCB_ATOM_NONE;
xcb_atom_t atom_jpeg = XCB_ATOM_NONE;
xcb_atom_t atom_prop = XCB_ATOM_NONE;

/* Utility: ensure directories exist */
int ensure_dirs(void) {
    struct stat st;
    if (stat(BASE_DIR, &st) != 0) {
        if (mkdir(BASE_DIR, 0755) != 0 && errno != EEXIST) {
            perror("mkdir base");
            return -1;
        }
    }
    if (stat(IMG_DIR, &st) != 0) {
        if (mkdir(IMG_DIR, 0755) != 0 && errno != EEXIST) {
            perror("mkdir imgs");
            return -1;
        }
    }
    return 0;
}

/* Read/Write next id (persistent) */
unsigned int read_nextid(void) {
    __entry("read_nextid()");
    FILE *f = fopen(NEXTID_FILE, "r");
    if (!f) return 0;
    unsigned int v = 0;
    if (fscanf(f, "%u", &v) != 1) v = 0;
    fclose(f);
    if (v >= HISTORY_SIZE) v = v % HISTORY_SIZE;
    __exit("read_nextid(): Done %d", v);
    return v;
}
void write_nextid(unsigned int v) {
    __entry("write_nextid(%d)", v);
    FILE *f = fopen(NEXTID_FILE, "w");
    if (!f) return;
    fprintf(f, "%u\n", (v) % HISTORY_SIZE);
    fclose(f);
    __exit("write_nextid()");
}

/* Append path to history file (one per line). Newest appended. */
int append_history_path(const char *path) {
    FILE *f = fopen(HISTORY_FILE, "a");
    if (!f) {
        perror("fopen history append");
        return -1;
    }
    fprintf(f, "%s\n", path);
    fclose(f);
    return 0;
}

/* Build filename: /tmp/clipboard/historydata_<id>_<datatype>.<ext> or in IMG_DIR for images */
void build_data_filename(char *buf, size_t buflen, unsigned int id, const char *datatype, const char *ext) {
    char idbuf[16];
    snprintf(idbuf, sizeof(idbuf), "%0*u", ID_WIDTH, id);
    if (strcmp(datatype, "image") == 0) {
        snprintf(buf, buflen, IMG_DIR "/historydata_%s_%s.%s", idbuf, datatype, ext);
    } else {
        snprintf(buf, buflen, BASE_DIR "/historydata_%s_%s.%s", idbuf, datatype, ext);
    }
    __log("[build_data_filename] Done: %s", buf);
}

/* If file exists at path, unlink it */
void unlink_if_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) unlink(path);
}

/* Save bytes to file (binary) */
int save_bytes(const char *path, const void *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t w = fwrite(data, 1, len, f);
    fclose(f);
    return (w == len) ? 0 : -1;
}

/* Save text compressed with gzip (.txt.gz) */
int save_text_gzip(const char *path, const void *data, size_t len) {
    gzFile gz = gzopen(path, "wb");
    if (!gz) return -1;
    int written = gzwrite(gz, data, (unsigned)len);
    gzclose(gz);
    return (written == (int)len) ? 0 : -1;
}

/* Get atom by name */
xcb_atom_t get_atom(xcb_connection_t *c, const char *name) {
    xcb_intern_atom_cookie_t ck = xcb_intern_atom(c, 0, strlen(name), name);
    xcb_intern_atom_reply_t *r = xcb_intern_atom_reply(c, ck, NULL);
    if (!r) return XCB_ATOM_NONE;
    xcb_atom_t a = r->atom;
    free(r);
    return a;
}

/* Request TARGETS and gather available atoms; return flags */
void request_targets(xcb_connection_t *c, xcb_window_t win) {
    xcb_convert_selection(c, win, atom_clipboard, atom_targets, atom_prop, XCB_CURRENT_TIME);
    xcb_flush(c);
}

/* Wait up to timeout_us for selection notify events and return reply for property atom if available */
xcb_get_property_reply_t *wait_for_property(xcb_connection_t *c, xcb_window_t win, int timeout_us) {
    __entry2("wait_for_property()");
    int waited = 0;
    while (waited < timeout_us) {
        xcb_generic_event_t *ev = xcb_poll_for_event(c);
        if (!ev) {
            usleep(5000);
            waited += 5000;
            continue;
        }
        if ((ev->response_type & ~0x80) == XCB_SELECTION_NOTIFY) {
            xcb_selection_notify_event_t *sn = (xcb_selection_notify_event_t*)ev;
            if (sn->property == atom_prop) {
                xcb_get_property_cookie_t pk = xcb_get_property(c, 0, win, atom_prop, XCB_ATOM_ANY, 0, UINT32_MAX);
                xcb_get_property_reply_t *rep = xcb_get_property_reply(c, pk, NULL);
                free(ev);
                __exit2("wait_for_property() : %p", rep);
                return rep;
            }
        }
        free(ev);
    }
    __exit2("wait_for_property() : NULL");
    return NULL;
}

/* Poll and handle one clipboard snapshot. Returns 1 if something saved, 0 otherwise */
int handle_poll_clipboard(xcb_connection_t *c, xcb_window_t win, unsigned int *nextid) {
    __entry2("handle_poll_clipboard()");
    static uLong last_crc = 0;
    static size_t last_len = 0;
    static xcb_atom_t last_target = XCB_ATOM_NONE;
    
    /* ask for TARGETS */
    request_targets(c, win);
    /* wait up to POLL_MS microseconds for TARGETS reply (~POLL_MS ms) */
    xcb_get_property_reply_t *targets_reply = wait_for_property(c, win, POLL_MS * 1000);
    int saved = 0;
    if (targets_reply) {
        int n = xcb_get_property_value_length(targets_reply) / sizeof(xcb_atom_t);
        xcb_atom_t *atoms = (xcb_atom_t*)xcb_get_property_value(targets_reply);
        int has_utf8 = 0, has_png = 0, has_jpeg = 0;
        for (int i = 0; i < n; ++i) {
            xcb_atom_t a = atoms[i];
            if (a == atom_utf8) has_utf8 = 1;
            if (a == atom_png) has_png = 1;
            if (a == atom_jpeg) has_jpeg = 1;
        }
        free(targets_reply);

        /* Prefer text */
        if (has_utf8) {
            xcb_atom_t target = atom_utf8;
            /* request UTF8_STRING */
            xcb_convert_selection(c, win, atom_clipboard, target, atom_prop, XCB_CURRENT_TIME);
            xcb_flush(c);
            xcb_get_property_reply_t *rep = wait_for_property(c, win, POLL_MS * 1000);
            if (rep) {
                int len = xcb_get_property_value_length(rep);
                if (len > 0) {
                    const void *data = xcb_get_property_value(rep);
                    uLong crc = crc32(0L, (const Bytef *)data, (uInt)len);
                    if ((size_t)len == last_len && crc == last_crc && target == last_target) {
                        free(rep);
                        return 0;
                    }
                    /* save compressed text */
                    char fname[MAX_FILENAME_SIZE];
                    char tmpath[MAX_PATHNAME_SIZE];
                    unsigned int id = *nextid;
                    build_data_filename(fname, sizeof(fname), id, "text", "txt.gz");
                    /* if file exists for this id, remove it first */
                    unlink_if_exists(fname);
                    if (save_text_gzip(fname, data, (size_t)len) == 0) {
                        /* append to history */
                        snprintf(tmpath, sizeof(tmpath), "%s", fname);
                        append_history_path(tmpath);
                        printf("[LOG] Saved TEXT (%d bytes) to %s\n", len, tmpath);
                        fflush(stdout);
                        saved = 1;
                        last_crc = crc;
                        last_len = (size_t)len;
                        last_target = target;
                    }
                    /* advance id */
                    *nextid = (*nextid + 1) % HISTORY_SIZE;
                    write_nextid(*nextid);
                }
                free(rep);
            }
        } else if (has_png || has_jpeg) {
            xcb_atom_t target = has_png ? atom_png : atom_jpeg;
            const char *ext = has_png ? "png" : "jpg";
            xcb_convert_selection(c, win, atom_clipboard, target, atom_prop, XCB_CURRENT_TIME);
            xcb_flush(c);
            xcb_get_property_reply_t *rep = wait_for_property(c, win, POLL_MS * 1000);
            if (rep) {
                int len = xcb_get_property_value_length(rep);
                if (len > 0) {
                    const void *data = xcb_get_property_value(rep);
                    uLong crc = crc32(0L, (const Bytef *)data, (uInt)len);
                    if ((size_t)len == last_len && crc == last_crc && target == last_target) {
                        free(rep);
                        return 0;
                    }
                    char fname[512];
                    char tmpath[512];
                    unsigned int id = *nextid;
                    build_data_filename(fname, sizeof(fname), id, "image", ext);
                    unlink_if_exists(fname);
                    if (save_bytes(fname, data, (size_t)len) == 0) {
                        snprintf(tmpath, sizeof(tmpath), "%s", fname);
                        append_history_path(tmpath);
                        printf("[LOG] Saved IMAGE (%s, %d bytes) to %s\n", ext, len, tmpath);
                        fflush(stdout);
                        saved = 1;
                        last_crc = crc;
                        last_len = (size_t)len;
                        last_target = target;
                    }
                    *nextid = (*nextid + 1) % HISTORY_SIZE;
                    write_nextid(*nextid);
                }
                free(rep);
            }
        }
    }
    __exit2("handle_poll_clipboard()");
    return saved;
}


int restoreClipboardContent(xcb_connection_t *c, xcb_window_t win, const char *historyPath){
    __entry("restoreClipboardContent()");
    if (!historyPath) {
        __err("restoreClipboardContent: NULL path");
        __exit("restoreClipboardContent() : -1");
        return -1;
    }

    const char *ext = strrchr(historyPath, '.');
    if (!ext) {
        __err("restoreClipboardContent: no extension in path %s", historyPath);
        __exit("restoreClipboardContent() : -1");
        return -1;
    }

    if (strcmp(ext, ".gz") == 0) {
        // restore TEXT
        gzFile gz = gzopen(historyPath, "rb");
        if (!gz) {
            __err("gzopen failed for %s", historyPath);
        __exit("restoreClipboardContent() : -1");
            return -1;
        }

        // đọc hết file (ở đây cho fixed-size, bạn có thể stat() để biết kích thước trước)
        size_t cap = 1024 * 1024;
        char *buf = malloc(cap);
        if (!buf) {
            gzclose(gz);
            __err("malloc failed");
        __exit("restoreClipboardContent() : -1");
            return -1;
        }
        int n = gzread(gz, buf, cap - 1);
        if (n < 0) {
            int errnum;
            const char *errmsg = gzerror(gz, &errnum);
            __err("gzread error: %s", errmsg);
            free(buf);
            gzclose(gz);
            __exit("restoreClipboardContent() : -1");
            return -1;
        }
        buf[n] = '\0';
        gzclose(gz);

        xcb_change_property(c, XCB_PROP_MODE_REPLACE, win,
                            atom_clipboard, atom_utf8, 8,
                            n, buf);
        xcb_flush(c);

        __log("Restored TEXT to clipboard from %s", historyPath);

        free(buf);
        __exit("restoreClipboardContent() : 0");
        return 0;
    }

    if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0) {
        // restore IMAGE
        FILE *f = fopen(historyPath, "rb");
        if (!f) {
            __err("fopen failed for %s", historyPath);
            __exit("restoreClipboardContent() : -1");
            return -1;
        }
        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (len <= 0) {
            fclose(f);
            __err("file length invalid %s", historyPath);
            __exit("restoreClipboardContent() : -1");
            return -1;
        }
        void *buf = malloc(len);
        if (!buf) {
            fclose(f);
            __err("malloc failed");
            __exit("restoreClipboardContent() : -1");
            return -1;
        }
        fread(buf, 1, len, f);
        fclose(f);

        xcb_atom_t mime = (strcmp(ext, ".png") == 0) ? atom_png : atom_jpeg;
        xcb_change_property(c, XCB_PROP_MODE_REPLACE, win,
                            atom_clipboard, mime, 8,
                            len, buf);
        xcb_flush(c);

        __log("Restored IMAGE (%s) to clipboard from %s", ext+1, historyPath);

        free(buf);
        __exit("restoreClipboardContent() : 0");
        return 0;
    }

    __err("restoreClipboardContent: unsupported extension %s", ext);
    __exit("restoreClipboardContent() : -1");
    return -1;
}
