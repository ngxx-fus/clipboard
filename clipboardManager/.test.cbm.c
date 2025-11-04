
/* Test harness for clipboard_xcb_history_fixed.c
 *
 * Compile:
 *   gcc -std=gnu11 -O2 clipboard_xcb_history_fixed.c -o clipboard_xcb_history_fixed -lxcb -lz
 *
 * Run:
 *   ./clipboard_xcb_history_fixed
 *
 * The test will:
 *  - ensure dirs
 *  - read/write nextid
 *  - build filenames
 *  - save a small binary blob and a gzipped text
 *  - append to history
 *  - unlink files
 *  - if X server available: connect, create a window, get some atoms, request targets,
 *    try wait_for_property and handle_poll_clipboard once (best-effort)
 *  - call restoreClipboardContent (best-effort)
 *
 * Note: This is a functional test driver — real behavior of X-related calls depends on running X server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "clipboardManager.h"

int main(int argc, char **argv) {
    int rc = 0;

    printf("==== Clipboard Manager Self-test ====\n");

    /* 1) Ensure dirs */
    if (ensure_dirs() == 0) {
        printf("[OK] ensure_dirs()\n");
    } else {
        fprintf(stderr, "[ERR] ensure_dirs()\n");
        rc = 1;
    }

    /* 2) Read nextid, write nextid */
    unsigned int nid = read_nextid();
    printf("[INFO] read_nextid() -> %u\n", nid);

    unsigned int save_id = (nid + 1) % HISTORY_SIZE;
    write_nextid(save_id);
    unsigned int nid2 = read_nextid();
    if (nid2 == save_id) {
        printf("[OK] write_nextid/read_nextid roundtrip (%u)\n", nid2);
    } else {
        fprintf(stderr, "[ERR] write_nextid/read_nextid expected %u got %u\n", save_id, nid2);
        rc = 1;
    }

    /* 3) Build data filenames */
    char fname[MAX_PATHNAME_SIZE];
    build_data_filename(fname, sizeof(fname), 123, "text", "txt.gz");
    printf("[INFO] build_data_filename (text): %s\n", fname);

    char fname2[MAX_PATHNAME_SIZE];
    build_data_filename(fname2, sizeof(fname2), 7, "image", "png");
    printf("[INFO] build_data_filename (image): %s\n", fname2);

    /* 4) Save bytes (binary) */
    const unsigned char blob[] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A }; /* small PNG signature as dummy */
    if (save_bytes(fname2, blob, sizeof(blob)) == 0) {
        printf("[OK] save_bytes -> %s\n", fname2);
    } else {
        fprintf(stderr, "[ERR] save_bytes -> %s\n", fname2);
        rc = 1;
    }

    /* 5) Save text gz */
    const char txt[] = "Hello clipboard test\nLine2\n";
    if (save_text_gzip(fname, txt, strlen(txt)) == 0) {
        printf("[OK] save_text_gzip -> %s\n", fname);
    } else {
        fprintf(stderr, "[ERR] save_text_gzip -> %s\n", fname);
        rc = 1;
    }

    /* 6) Append history path */
    if (append_history_path(fname) == 0) {
        printf("[OK] append_history_path(%s)\n", fname);
    } else {
        fprintf(stderr, "[ERR] append_history_path(%s)\n", fname);
        rc = 1;
    }

    /* 7) unlink_if_exists */
    /* create a temp file then unlink */
    char tmpfile[MAX_PATHNAME_SIZE];
    snprintf(tmpfile, sizeof(tmpfile), BASE_DIR "/.test_unlink_tmp");
    FILE *f = fopen(tmpfile, "wb");
    if (f) {
        fputs("x", f);
        fclose(f);
        unlink_if_exists(tmpfile);
        if (access(tmpfile, F_OK) != 0) {
            printf("[OK] unlink_if_exists(%s)\n", tmpfile);
        } else {
            fprintf(stderr, "[ERR] unlink_if_exists(%s) still exists\n", tmpfile);
            rc = 1;
        }
    } else {
        fprintf(stderr, "[WARN] could not create tmp file %s to test unlink\n", tmpfile);
    }

    /* 8) Try XCB related tests (best-effort) */
    xcb_connection_t *c = NULL;
    xcb_window_t win = 0;
    int have_xcb = 0;

    c = xcb_connect(NULL, NULL);
    if (c && xcb_connection_has_error(c) == 0) {
        have_xcb = 1;
        printf("[INFO] connected to X server via XCB\n");

        /* create a simple window for property requests */
        win = xcb_generate_id(c);
        xcb_create_window(c,
                          XCB_COPY_FROM_PARENT, /* depth */
                          win,
                          xcb_setup_roots_iterator(xcb_get_setup(c)).data->root,
                          0, 0, 1, 1,
                          0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          xcb_setup_roots_iterator(xcb_get_setup(c)).data->root_visual,
                          0, NULL);
        xcb_flush(c);

        printf("[INFO] created test window id 0x%08x\n", (unsigned)win);

        /* Get some atoms (using wrapper get_atom) */
        xcb_atom_t a_clip = get_atom(c, "CLIPBOARD");
        xcb_atom_t a_utf8 = get_atom(c, "UTF8_STRING");
        xcb_atom_t a_targets = get_atom(c, "TARGETS");
        printf("[INFO] atoms: CLIPBOARD=%u UTF8=%u TARGETS=%u\n", a_clip, a_utf8, a_targets);

        /* Request targets */
        request_targets(c, win);
        printf("[INFO] request_targets() called\n");

        /* Wait for property (short timeout) */
        xcb_get_property_reply_t *reply = wait_for_property(c, win, 200000); /* 200 ms */
        if (reply) {
            printf("[OK] wait_for_property -> got reply (len=%u)\n", reply->value_len);
            free(reply); /* wrappers may expect free */
        } else {
            printf("[INFO] wait_for_property -> no reply (timeout or none)\n");
        }

        /* Try handle_poll_clipboard once (best-effort) */
        unsigned int tid = read_nextid(); /* use current nextid */
        int hres = handle_poll_clipboard(c, win, &tid);
        if (hres > 0) {
            printf("[OK] handle_poll_clipboard reported saved data (nextid now %u)\n", tid);
        } else if (hres == 0) {
            printf("[INFO] handle_poll_clipboard: nothing new\n");
        } else {
            printf("[WARN] handle_poll_clipboard returned error\n");
        }

        /* Try restoreClipboardContent using the history file path */
        if (restoreClipboardContent(c, win, HISTORY_FILE) == 0) {
            printf("[OK] restoreClipboardContent(%s) returned 0\n", HISTORY_FILE);
        } else {
            printf("[INFO] restoreClipboardContent(%s) returned non-zero (maybe no history)\n", HISTORY_FILE);
        }

        /* clean up X window */
        xcb_destroy_window(c, win);
        xcb_flush(c);
        xcb_disconnect(c);
        c = NULL;
    } else {
        fprintf(stderr, "[WARN] cannot connect to X server via XCB, skipping X tests\n");
        if (c) xcb_disconnect(c);
    }

    printf("==== Test finished: %s ====\n", rc == 0 ? "SUCCESS" : "SOME FAILURES");
    return rc;
}
