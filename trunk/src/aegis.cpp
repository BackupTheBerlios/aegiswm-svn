/* file name  : aegis.cpp
 * author     : Michael Brailsford
 * created    : Wed Sep 29 23:26:26 CDT 2004
 * copyright  : 2004 Michael Brailsford
 * version    : $Revision$
 */

#include "aegis.h"
#include "client.h"

using std::map;
using std::vector;

//{{{
static char * event_names[LASTEvent] = {
	"None",
	"None",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage",
	"MappingNotify",
};
//}}}

//{{{
Aegis::Aegis() : clients(), atoms() {
	XSetWindowAttributes sattr;

	dpy = XOpenDisplay(getenv("DISPLAY"));
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	
	arrow_curs = XCreateFontCursor(dpy, XC_left_ptr);
	XDefineCursor(dpy, root, arrow_curs);

	//define all our atoms
	internAtoms();

	//loop through all windows already opened in the X display and wrap them in a new Client object.
	reparentExistingWindows();

	//let X know which events we want to be notified of
	sattr.event_mask = SubstructureRedirectMask | SubstructureNotifyMask | 
		               ColormapChangeMask       | ButtonPressMask        | 
		               ButtonReleaseMask        | FocusChangeMask        | 
		               EnterWindowMask          | LeaveWindowMask        | 
		               PropertyChangeMask       | ButtonMotionMask       ;
			   
	XChangeWindowAttributes(dpy, root, CWEventMask, &sattr);
}
//}}}
//{{{
Aegis::~Aegis() {
	ClientMap::reverse_iterator iter, end = clients.rend();

	for(iter = clients.rbegin(); iter != end; iter++) {
		delete (*iter).second;
		(*iter).second = NULL;
	}

	clients.clear();
}
//}}}
//{{{
void Aegis::run() {
	XEvent ev;

	log_debug("Running...\n");
	for(;;) {
		XNextEvent(dpy, &ev);
		log_debug("%s\n", event_names[ev.type]);

		//I don't like the switch, but its easy to write.  It will get to be a pain to 
		//maintain, that is what refactoring is for.  :)
		switch(ev.type) {
			case ButtonPress:
				handleButtonPress(&ev);
				break;
			case ButtonRelease:
				handleButtonRelease(&ev);
				break;
			case ConfigureRequest:
				handleConfigureRequest(&ev);
				break;
			case MapRequest:
				handleMapRequest(&ev);
				break;
			case UnmapNotify:
				handleUnmapNotify(&ev);
				break;
			case EnterNotify:
				handleEnterNotify(&ev);
				break;
			case LeaveNotify:
				handleLeaveNotify(&ev);
				break;
			case MotionNotify:
				if(aestate.button_down)
					handleMotionNotify(&ev);
				break;
		}
	}
}
//}}}
//{{{
void Aegis::internAtoms() {
	atoms[XA_WM_CLASS]              = XInternAtom(dpy, "XA_WM_CLASS", False);
	atoms[XA_WM_HINTS]              = XInternAtom(dpy, "XA_WM_HINTS", False);
	atoms[XA_WM_ICON_NAME]          = XInternAtom(dpy, "XA_WM_ICON_NAME", False);
	atoms[XA_WM_NAME]               = XInternAtom(dpy, "XA_WM_NAME", False);
	atoms[XA_WM_NORMAL_HINTS]       = XInternAtom(dpy, "XA_WM_NORMAL_HINTS", False);
	atoms[XA_WM_TRANSIENT_FOR_HINT] = XInternAtom(dpy, "XA_WM_TRANSIENT_FOR_HINT", False);
	atoms[XA_WM_ZOOM_HINTS]         = XInternAtom(dpy, "XA_WM_ZOOM_HINTS", False);
	atoms[WM_CLIENT_MACHINE]        = XInternAtom(dpy, "WM_CLIENT_MACHINE", False);
	atoms[XA_WM_COMMAND]            = XInternAtom(dpy, "XA_WM_COMMAND", False);
}
//}}}
//{{{
void Aegis::reparentExistingWindows() {
	unsigned int nwins, i;
	Window unused1, tmp, *wins;
	XWindowAttributes attr;

	//Ask X for all the windows that are already mapped in the root window
	XQueryTree(dpy, root, &unused1, &tmp, &wins, &nwins);

	//For each window that is mapped...
	for(i = 0; i < nwins; i++) {
		tmp = wins[i];
		//...get its attributes, and...
		XGetWindowAttributes(dpy, tmp, &attr);
		//...if we are supposed to, wrap it up in a new Client object.
		if (!attr.override_redirect && attr.map_state == IsViewable) {
			log_debug("Creating a new Client for %i\n", (int)tmp);
			clients[tmp] = new Client(dpy, this, tmp);
		}
	}
	XFree(wins);
}
//}}}

//{{{
void Aegis::handleConfigureRequest(XEvent * xev) {
	log_debug("Entering handleConfigureRequest(Xevent * ev)\n");
	XConfigureRequestEvent xcr = xev->xconfigurerequest;
	XWindowChanges wc;
	wc.x          = xcr.x;
	wc.y          = xcr.y;
	wc.width      = xcr.width;
	wc.height     = xcr.height;
	wc.sibling    = xcr.above;
	wc.stack_mode = xcr.detail;

	XConfigureWindow(dpy, xcr.window, xcr.value_mask, &wc);
	log_debug("Leaving handleConfigureRequest()\n");
}
//}}}
//{{{
void Aegis::handleMapRequest(XEvent * xev) {
	log_debug("Entering handleMapRequest(Xevent * ev)\n");
	XMapRequestEvent xmr = xev->xmaprequest;
	Client * c = new Client(dpy, this, xmr.window);
	log_debug("here\n");
	vector<Window> wins = c->getWindowList();
	log_debug("here\n");
	vector<Window>::iterator iter, end = wins.end();
	log_debug("here\n");

	for(iter = wins.begin(); iter != end; iter++)
		clients[(*iter)] = c;

	log_debug("here\n");
	log_debug("Leaving handleMapRequest()\n");
}
//}}}
//{{{
void Aegis::handleUnmapNotify(XEvent * xev) {
	log_debug("Entering handleUnmapNotify(Xevent * ev)\n");
	XUnmapEvent umap = xev->xunmap;
	Client * c = clients[umap.window];
	c->unmap();
	log_debug("Leaving handleUnmapNotify()\n");
}
//}}}
//{{{
void Aegis::handleEnterNotify(XEvent * xev) {
	XEnterWindowEvent xew= (XEnterWindowEvent)xev->xcrossing;
	log_debug("Entering handleEnterNotify(XEvent * xev)\n");
	log_debug("xev->window == %i\n", (int)xew.window);
	log_debug("xev->subwindow == %i\n", (int)xew.subwindow);

	//if(clients[xew.window] != NULL)
		//XSetInputFocus(dpy, xew.window, RevertToNone, CurrentTime);
	log_debug("Leaving handleEnterNotify()\n");
}
//}}}
//{{{
void Aegis::handleLeaveNotify(XEvent * xev) {
	XLeaveWindowEvent xlw = (XLeaveWindowEvent)xev->xcrossing;
	log_debug("Entering handleLeaveNotify(XEvent * xev)\n");
	log_debug("xev->window == %i\n", (int)xlw.window);
	log_debug("xev->subwindow == %i\n", (int)xlw.subwindow);
	log_debug("Leaving handleLeaveNotify()\n");
}
//}}}
//{{{
void Aegis::handleButtonPress(XEvent * xev) {
	XButtonPressedEvent xbp = (XButtonPressedEvent)xev->xbutton;
	//This is unset in handleButtonRelease()
	aestate.button_down = true;
	Client * c = clients[xbp.window];
	log_debug("Entering handleButtonPress(XEvent * xev)\n");
	log_debug("xev->window == %i\n", (int)xbp.window);
	log_debug("xev->button == %i\n", (int)xbp.button);
	log_debug("xev->state  == %i\n", (int)xbp.state);

	if(c) {
		switch(xbp.button) {
			case 1:
				c->raise();
				break;
			case 2:
				break;
			case 3:
				break;
		}
	}
	else {
		//handle button presses on the root window
	}
	log_debug("Leaving handleButtonPress()\n");
}
//}}}
//{{{
void Aegis::handleButtonRelease(XEvent * xev) {
	XButtonReleasedEvent xbp = (XButtonReleasedEvent)xev->xbutton;
	//this is set in handleButtonPress()
	aestate.button_down = false;
}
//}}}
//{{{
void Aegis::handleMotionNotify(XEvent * ev) {
	log_debug("Entering handleMotionNotify(Xevent * ev)\n");
	XMotionEvent xmov = ev->xmotion;
	Client * c = clients[xmov.window];

	compressEvent(xmov.window, ev->type, ev);

	if(c) {
		log_debug("Moving window %i to (%i, %i)\n", (int)xmov.window, xmov.x_root, xmov.y_root);
		c->moveTo(xmov.x_root, xmov.y_root);
	}
	else {
		log_debug("There is no client with window id %i\n", (int)xmov.window);
	}
	log_debug("Leaving handleMotionNotify()\n");
}
//}}}
//{{{
int Aegis::compressEvent(Window win, int event_type, XEvent * ev) {
	log_debug("Entering compressEvent()\n");
	int rv = 0;
	while(XCheckTypedWindowEvent(dpy, win, event_type, ev)) {
		rv++;
	}
	log_debug("Skipped %i %s event%s\n", rv, event_names[event_type], (rv > 1 || rv == 0) ? "s": "");

	log_debug("Leaving compressEvent()\n");
	return rv;
}
//}}}

//{{{
int main(int argc, char ** argv) {
	//set up logging
	openlog("aegiswm", LOG_CONS, LOG_USER);

	Aegis aegis;
	aegis.run();

	closelog();
}
//}}}

/*
 * change log
 *
 * $Log$
 */


