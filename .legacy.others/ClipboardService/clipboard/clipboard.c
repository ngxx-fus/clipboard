#include "clipboard.h"

/* global verbosity output level, defaults to OSILENT */
int xcverb = OSILENT;

/* Table of event names from event numbers */
const char *evtstr[LASTEvent] = {
    "ProtocolError", "ProtocolReply", "KeyPress", "KeyRelease",
    "ButtonPress", "ButtonRelease", "MotionNotify", "EnterNotify",
    "LeaveNotify", "FocusIn", "FocusOut", "KeymapNotify", "Expose",
    "GraphicsExpose", "NoExpose", "VisibilityNotify", "CreateNotify",
    "DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
    "ReparentNotify", "ConfigureNotify", "ConfigureRequest",
    "GravityNotify", "ResizeRequest", "CirculateNotify",
    "CirculateRequest", "PropertyNotify", "SelectionClear",
    "SelectionRequest", "SelectionNotify", "ColormapNotify",
    "ClientMessage", "MappingNotify", "GenericEvent", };


struct requestor *get_requestor(Window win)
{
	struct requestor *requestor;

	if (requestors) {
	    for (requestor = requestors; requestor != NULL; requestor = requestor->next) {
	        if (requestor->cwin == win) {
		    if (xcverb >= OVERBOSE) {
                fprintf(stderr,
                    "    = Reusing requestor for %s\n",
                    xcnamestr(dpy, win) );
                }

	            return requestor;
	        }
	    }
	}

	if (xcverb >= OVERBOSE) {
	    fprintf(stderr, "    + Creating new requestor for %s\n",
		    xcnamestr(dpy, win) );
	}

	requestor = (struct requestor *)calloc(1, sizeof(struct requestor));
	if (!requestor) {
	    errmalloc();
	} else {
	    requestor->context = XCLIB_XCIN_NONE;
	}

	if (!requestors) {
	    requestors = requestor;
	} else {
	    requestor->next = requestors;
	    requestors = requestor;
	}

	return requestor;
}

void del_requestor(struct requestor *requestor)
{
	struct requestor *reqitr;

	if (!requestor) {
	    return;
	}

	if (xcverb >= OVERBOSE) {
	    fprintf(stderr,
		    "    - Deleting requestor for %s\n",
		    xcnamestr(dpy, requestor->cwin) );
	}

	if (requestors == requestor) {
	    requestors = requestors->next;
	} else {
	    for (reqitr = requestors; reqitr != NULL; reqitr = reqitr->next) {
	        if (reqitr->next == requestor) {
	            reqitr->next = reqitr->next->next;
	            break;
	        }
	    }
	}

	free(requestor);
}

int clean_requestors() {
    /* Remove any requestors for which the X window has disappeared */
    if (xcverb >= ODEBUG) {
	fprintf(stderr, "xclip: debug: checking for requestors whose window has closed\n");
    }
    struct requestor *r = requestors;
    Window win;
    XWindowAttributes dummy;
    while (r) {
	win = r->cwin;

	// check if window exists by seeing if XGetWindowAttributes works.
	// note: this triggers X's BadWindow error and runs xchandler().
	if ( !XGetWindowAttributes(dpy, win, &dummy) ) {
	    if (xcverb >= OVERBOSE) {
		fprintf(stderr, "    ! Found obsolete requestor 0x%lx\n", win);
	    }
	    del_requestor(r);
	}
	r = r -> next;
    }
    return 0;
}