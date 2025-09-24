/*
 * clipboard_xcb_history_fixed.c
 *
 * Requirements implemented:
 * 1) Poll clipboard every ~200 ms (POLL_MS)
 * 2) Maintain history file: /tmp/clipboard/history (list of paths)
 * 3) Each history item saved as /tmp/clipboard/historydata_<id>_<datatype>.<ext>
 *      - <id> is zero-padded 4 digits 0000..HISTORY_SIZE-1 (ring buffer)
 *      - <datatype> is "text" or "image"
 *      - <ext> is the file extension (txt.gz for text, png/jpg for images)
 * 4) Text entries are stored in compressed gz (binary) to save space
 *
 * Build:
 *   gcc -std=gnu11 -O2 clipboard_xcb_history_fixed.c -o clipboard_xcb_history_fixed -lxcb -lz
 *
 * Run:
 *   ./clipboard_xcb_history_fixed
 *
 * Notes:
 * - This program uses XCB to request CLIPBOARD contents. It requests TARGETS
 *   first to detect available formats. It prefers UTF8_STRING (text), then
 *   image/png and image/jpeg. When a format is available it requests that
 *   format and stores the returned bytes.
 * - The program stores a persistent next-id counter in /tmp/clipboard/.nextid
 *   so that ids continue across restarts. Ids wrap in [0, HISTORY_SIZE-1].
 * - /tmp/clipboard/history contains one path per line, newest appended.
 * - When overwriting an existing id, the old file is unlinked.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <zlib.h> /* link -lz */

#include "../global.h"
#include "../log/log.h"

// #define POLL_MS                     200
// #define BASE_DIR                    "/tmp/clipboard"
// #define HISTORY_FILE BASE_DIR       "/history"
// #define IMG_DIR BASE_DIR            "/imgs"
// #define NEXTID_FILE BASE_DIR        "/.nextid"
// #define HISTORY_SIZE                1000 /* adjustable */
// #define ID_WIDTH                    4
// #define MAX_FILENAME_SIZE           30
// #define MAX_PATHNAME_SIZE           64

#define PROP_NAME "CB_XCB_PROP_1"

extern xcb_atom_t atom_clipboard;
extern xcb_atom_t atom_utf8;
extern xcb_atom_t atom_targets;
extern xcb_atom_t atom_png;
extern xcb_atom_t atom_jpeg;
extern xcb_atom_t atom_prop;

/* Utility: ensure directories exist */
int ensure_dirs(void);
/* Read/Write next id (persistent) */
unsigned int read_nextid(void);
void write_nextid(unsigned int v);
/* Append path to history file (one per line). Newest appended. */
int append_history_path(const char *path);
/* Build filename: /tmp/clipboard/historydata_<id>_<datatype>.<ext> or in IMG_DIR for images */
void build_data_filename(char *buf, size_t buflen, unsigned int id, const char *datatype, const char *ext);
/* If file exists at path, unlink it */
void unlink_if_exists(const char *path);
/* Save bytes to file (binary) */
int save_bytes(const char *path, const void *data, size_t len);
/* Save text compressed with gzip (.txt.gz) */
int save_text_gzip(const char *path, const void *data, size_t len);
/* Get atom by name */
xcb_atom_t get_atom(xcb_connection_t *c, const char *name);
/* Request TARGETS and gather available atoms; return flags */
void request_targets(xcb_connection_t *c, xcb_window_t win);
/* Wait up to timeout_us for selection notify events and return reply for property atom if available */
xcb_get_property_reply_t *wait_for_property(xcb_connection_t *c, xcb_window_t win, int timeout_us);
/* Poll and handle one clipboard snapshot. Returns 1 if something saved, 0 otherwise */
int handle_poll_clipboard(xcb_connection_t *c, xcb_window_t win, unsigned int *nextid);

