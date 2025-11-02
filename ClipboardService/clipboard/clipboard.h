#ifndef __CLIPBOARD_H__
#define __CLIPBOARD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

/* output level constants for xcverb */
#define OSILENT  0
#define OQUIET   1
#define OVERBOSE 2
#define ODEBUG   9

/* xcout() contexts */
#define XCLIB_XCOUT_NONE	0	    /* no context */
#define XCLIB_XCOUT_SENTCONVSEL	1	/* sent a request */
#define XCLIB_XCOUT_INCR	2	    /* in an incr loop */
#define XCLIB_XCOUT_BAD_TARGET	3	/* given target failed */

/* xcin() contexts */
#define XCLIB_XCIN_NONE		0
#define XCLIB_XCIN_SELREQ	1
#define XCLIB_XCIN_INCR		2

/* global verbosity output level, defaults to OSILENT */
extern int xcverb = OSILENT;

/* Table of event names from event numbers */
extern const char *evtstr[LASTEvent];

/* connection to X11 display */
Display *dpy;

struct requestor
{
	Window cwin;
	Atom pty;
	unsigned int context;
	unsigned long sel_pos;
	int finished;
	long chunk_size;
	struct requestor *next;
};

struct requestor *requestors;

struct requestor *get_requestor(Window win);
void del_requestor(struct requestor *requestor);
int clean_requestors();













#endif