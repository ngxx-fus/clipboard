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

#ifndef __CLIPBOARD_MANAGER_H__
#define __CLIPBOARD_MANAGER_H__

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

#include "../helper/general.h"

#if 1 /// Auto define config if not!
    #ifndef POLL_MS
    #define POLL_MS 200
    #endif

    #ifndef BASE_DIR
    #define BASE_DIR "/tmp/clipboard"
    #endif

    #ifndef HISTORY_FILE
    #define HISTORY_FILE BASE_DIR "/history"
    #endif

    #ifndef IMG_DIR
    #define IMG_DIR BASE_DIR "/imgs"
    #endif

    #ifndef NEXTID_FILE
    #define NEXTID_FILE BASE_DIR "/.nextid"
    #endif

    #ifndef HISTORY_SIZE
    #define HISTORY_SIZE 1000 /* adjustable */
    #endif

    #ifndef ID_WIDTH
    #define ID_WIDTH 4
    #endif

    #ifndef MAX_FILENAME_SIZE
    #define MAX_FILENAME_SIZE 30
    #endif

    #ifndef MAX_PATHNAME_SIZE
    #define MAX_PATHNAME_SIZE 64
    #endif
#endif

#define PROP_NAME "CB_XCB_PROP_1"

/// @brief Atom representing the X11 clipboard selection.
extern xcb_atom_t atom_clipboard;

/// @brief Atom representing UTF-8 string format.
extern xcb_atom_t atom_utf8;

/// @brief Atom representing list of supported target formats.
extern xcb_atom_t atom_targets;

/// @brief Atom representing PNG image MIME type ("image/png").
extern xcb_atom_t atom_png;

/// @brief Atom representing JPEG image MIME type ("image/jpeg").
extern xcb_atom_t atom_jpeg;

/// @brief Atom representing internal property used for clipboard data exchange.
extern xcb_atom_t atom_prop;

/// @brief Ensure required directories for clipboard data exist.
/// @return 0 on success, -1 on failure.
int ensure_dirs(void);

/// @brief Read the next clipboard item ID from persistent storage.
/// @return The next ID value (non-negative integer).
unsigned int read_nextid(void);

/// @brief Write the next clipboard item ID to persistent storage.
/// @param v The ID value to write.
void write_nextid(unsigned int v);

/// @brief Append a clipboard history entry (file path) to the history file.
/// @param path Path to append (string).
/// @return 0 on success, -1 on failure.
int append_history_path(const char *path);

/// @brief Build a clipboard data filename (e.g., /tmp/clipboard/historydata_<id>_<datatype>.<ext>).
/// @param buf Output buffer to hold the resulting path.
/// @param buflen Size of the output buffer.
/// @param id Clipboard item ID.
/// @param datatype Data type string (e.g., "text", "image").
/// @param ext File extension (e.g., "png", "txt").
void build_data_filename(char *buf, size_t buflen, unsigned int id, const char *datatype, const char *ext);

/// @brief Remove file at path if it exists.
/// @param path Path to target file.
void unlink_if_exists(const char *path);

/// @brief Save binary data to file.
/// @param path Destination file path.
/// @param data Pointer to binary data.
/// @param len Length of data in bytes.
/// @return 0 on success, -1 on failure.
int save_bytes(const char *path, const void *data, size_t len);

/// @brief Save text data compressed with gzip (.txt.gz).
/// @param path Destination file path.
/// @param data Pointer to text data.
/// @param len Length of text data in bytes.
/// @return 0 on success, -1 on failure.
int save_text_gzip(const char *path, const void *data, size_t len);

/// @brief Get or intern an X11 atom by name.
/// @param c XCB connection.
/// @param name Atom name string.
/// @return The corresponding xcb_atom_t value.
xcb_atom_t get_atom(xcb_connection_t *c, const char *name);

/// @brief Request available TARGETS from clipboard selection.
/// @param c XCB connection.
/// @param win Window used for property requests.
void request_targets(xcb_connection_t *c, xcb_window_t win);

/// @brief Wait for a SelectionNotify event up to a timeout and get property reply.
/// @param c XCB connection.
/// @param win Target window.
/// @param timeout_us Timeout in microseconds.
/// @return Pointer to xcb_get_property_reply_t on success, NULL on timeout or failure.
xcb_get_property_reply_t *wait_for_property(xcb_connection_t *c, xcb_window_t win, int timeout_us);

/// @brief Poll clipboard for new data and save snapshot if available.
/// @param c XCB connection.
/// @param win Window used for selection requests.
/// @param nextid Pointer to next ID counter.
/// @return 1 if data saved, 0 if nothing new, -1 on error.
int handle_poll_clipboard(xcb_connection_t *c, xcb_window_t win, unsigned int *nextid);

/// @brief Restore clipboard content from a history file.
/// @param c XCB connection.
/// @param win Target window.
/// @param historyPath Path to clipboard history file.
/// @return 0 on success, -1 on failure.
int restoreClipboardContent(xcb_connection_t *c, xcb_window_t win, const char *historyPath);

#endif /// __CLIPBOARD_MANAGER_H__