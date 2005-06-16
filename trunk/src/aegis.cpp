/* file name  : aegis.cpp
 * author     : Michael Brailsford
 * created    : Wed Sep 29 23:26:26 CDT 2004
 * copyright  : 2004 Michael Brailsford
 * version    : $Revision$
 */

#include "aegis.h"
#include "client.h"
#include "dispatch/event_dispatcher.h"

using std::pair;

//makes this available to the error handler
Aegis aegis;

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

void test(XEvent * ev) {
	printf("This worked!\n");
}

//{{{
Aegis::Aegis() : clients(), atoms(), aestate() {
	XSetWindowAttributes sattr;

	dpy = XOpenDisplay(getenv("DISPLAY"));

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);

	arrow_curs = XCreateFontCursor(dpy, XC_left_ptr);
	XDefineCursor(dpy, root, arrow_curs);

	//define all our atoms
	internAtoms();

	//create all the EventDispatcher objects
	create_dispatchers();

	registerEventHandler(root, ButtonPress, sigc::ptr_fun(test));

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
	EventDispatcher * dispatcher;

	log_debug("Running...\n");
	for(;;) {
		XNextEvent(dpy, &ev);
		log_debug("%s\n", event_names[ev.type]);

		//Grab the signal for the XEvent we just recieved
		dispatcher = event_registry[ev.type];
		//dispatch the event
		dispatcher->dispatch(&ev);
	}
}
//}}}
//{{{
void Aegis::registerEventHandler(Window w, ev_t event_type, aeslot_t handler) {
	log_info("Entering Aegis::registerEventHandler(w, event_type, handler)");

	if(event_registry.find(event_type) != event_registry.end()) {
		event_registry[event_type]->registerHandler(w, handler);
	}
	else {
		log_err("AegisWM does not have an EventDispatcher object "
				"for the %s event type.  Ignoring register request.", event_names[event_type]);
	}

	log_info("Leaving Aegis::registerEventTypeDispatcher()");
}
//}}}
//{{{
void Aegis::quit() {
	ClientMap::iterator iter, end = clients.end();

	for(iter = clients.begin(); iter != end; iter++) {
		std::pair<Window, Client *> par = (*iter);
		Window win = (par.second)->unparent();
		XMapWindow(dpy, win);
	}
	XCloseDisplay(dpy);
}
//}}}
//{{{
void Aegis::internAtoms() {
	atoms[XA_WM_CLASS]                        = XInternAtom(dpy, "XA_WM_CLASS", false);
	atoms[XA_WM_HINTS]                        = XInternAtom(dpy, "XA_WM_HINTS", false);
	atoms[XA_WM_ICON_NAME]                    = XInternAtom(dpy, "XA_WM_ICON_NAME", false);
	atoms[XA_WM_NAME]                         = XInternAtom(dpy, "XA_WM_NAME", false);
	atoms[XA_WM_NORMAL_HINTS]                 = XInternAtom(dpy, "XA_WM_NORMAL_HINTS", false);
	atoms[XA_WM_TRANSIENT_FOR_HINT]           = XInternAtom(dpy, "XA_WM_TRANSIENT_FOR_HINT", false);
	atoms[XA_WM_ZOOM_HINTS]                   = XInternAtom(dpy, "XA_WM_ZOOM_HINTS", false);
	atoms[WM_CLIENT_MACHINE]                  = XInternAtom(dpy, "WM_CLIENT_MACHINE", false);
	atoms[XA_WM_COMMAND]                      = XInternAtom(dpy, "XA_WM_COMMAND", false);

	//These are the rest of the atoms that we will eventually support  {{{
#if 0
	atoms[UTF8_STRING]                        = XInternAtom(display, "UTF8_STRING", false);
	atoms[WM_PROTOCOLS]                       = XInternAtom(display, "WM_PROTOCOLS", false);
	atoms[WM_DELETE_WINDOW]                   = XInternAtom(display, "WM_DELETE_WINDOW", false);
	atoms[WM_TAKE_FOCUS]                      = XInternAtom(display, "WM_TAKE_FOCUS", false);
	atoms[WM_STATE]                           = XInternAtom(display, "WM_STATE", false);
	atoms[WM_CHANGE_STATE]                    = XInternAtom(display, "WM_CHANGE_STATE", false);
	atoms[_MOTIF_WM_HINTS]                    = XInternAtom(display, "_MOTIF_WM_HINTS", false);
	atoms[_NET_SUPPORTED]                     = XInternAtom(display, "_NET_SUPPORTED", false);
	atoms[_NET_SUPPORTING_WM_CHECK]           = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", false);
	atoms[_NET_CLIENT_LIST]                   = XInternAtom(display, "_NET_CLIENT_LIST", false);
	atoms[_NET_CLIENT_LIST_STACKING]          = XInternAtom(display, "_NET_CLIENT_LIST_STACKING", false);
	atoms[_NET_ACTIVE_WINDOW]                 = XInternAtom(display, "_NET_ACTIVE_WINDOW", false);
	atoms[_NET_DESKTOP_VIEWPORT]              = XInternAtom(display, "_NET_DESKTOP_VIEWPORT", false);
	atoms[_NET_DESKTOP_GEOMETRY]              = XInternAtom(display, "_NET_DESKTOP_GEOMETRY", false);
	atoms[_NET_CURRENT_DESKTOP]               = XInternAtom(display, "_NET_CURRENT_DESKTOP", false);
	atoms[_NET_NUMBER_OF_DESKTOPS]            = XInternAtom(display, "_NET_NUMBER_OF_DESKTOPS", false);
	atoms[_NET_DESKTOP_NAMES]                 = XInternAtom(display, "_NET_DESKTOP_NAMES", false);
	atoms[_NET_WORKAREA]                      = XInternAtom(display, "_NET_WORKAREA", false);
	atoms[_NET_WM_DESKTOP]                    = XInternAtom(display, "_NET_WM_DESKTOP", false);
	atoms[_NET_WM_NAME]                       = XInternAtom(display, "_NET_WM_NAME", false);
	atoms[_NET_WM_VISIBLE_NAME]               = XInternAtom(display, "_NET_WM_VISIBLE_NAME", false);
	atoms[_NET_WM_STRUT]                      = XInternAtom(display, "_NET_WM_STRUT", false);
	atoms[_NET_WM_STRUT_PARTIAL]              = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", false);
	atoms[_NET_WM_PID]                        = XInternAtom(display, "_NET_WM_PID", false);
	atoms[_NET_WM_USER_TIME]                  = XInternAtom(display, "_NET_WM_USER_TIME", false);
	atoms[_NET_WM_STATE]                      = XInternAtom(display, "_NET_WM_STATE", false);
	atoms[_NET_WM_STATE_STICKY]               = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
	atoms[_NET_WM_STATE_SHADED]               = XInternAtom(display, "_NET_WM_STATE_SHADED", false);
	atoms[_NET_WM_STATE_HIDDEN]               = XInternAtom(display, "_NET_WM_STATE_HIDDEN", false);
	atoms[_NET_WM_STATE_MAXIMIZED_VERT]       = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
	atoms[_NET_WM_STATE_MAXIMIZED_HORZ]       = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
	atoms[_NET_WM_STATE_ABOVE]                = XInternAtom(display, "_NET_WM_STATE_ABOVE", false);
	atoms[_NET_WM_STATE_BELOW]                = XInternAtom(display, "_NET_WM_STATE_BELOW", false);
	atoms[_NET_WM_STATE_STAYS_ON_TOP]         = XInternAtom(display, "_NET_WM_STATE_STAYS_ON_TOP", false);
	atoms[_NET_WM_STATE_STAYS_AT_BOTTOM]      = XInternAtom(display, "_NET_WM_STATE_STAYS_AT_BOTTOM", false);
	atoms[_NET_WM_STATE_FULLSCREEN]           = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", false);
	atoms[_NET_WM_STATE_SKIP_TASKBAR]         = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
	atoms[_NET_WM_ALLOWED_ACTIONS]            = XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", false);
	atoms[_NET_WM_ACTION_MOVE]                = XInternAtom(display, "_NET_WM_ACTION_MOVE", false);
	atoms[_NET_WM_ACTION_RESIZE]              = XInternAtom(display, "_NET_WM_ACTION_RESIZE", false);
	atoms[_NET_WM_ACTION_MINIMIZE]            = XInternAtom(display, "_NET_WM_ACTION_MINIMIZE", false);
	atoms[_NET_WM_ACTION_SHADE]               = XInternAtom(display, "_NET_WM_ACTION_SHADE", false);
	atoms[_NET_WM_ACTION_STICK]               = XInternAtom(display, "_NET_WM_ACTION_STICK", false);
	atoms[_NET_WM_ACTION_MAXIMIZE_HORZ]       = XInternAtom(display, "_NET_WM_ACTION_MAXIMIZE_HORZ", false);
	atoms[_NET_WM_ACTION_MAXIMIZE_VERT]       = XInternAtom(display, "_NET_WM_ACTION_MAXIMIZE_VERT", false);
	atoms[_NET_WM_ACTION_FULLSCREEN]          = XInternAtom(display, "_NET_WM_ACTION_FULLSCREEN", false);
	atoms[_NET_WM_ACTION_CHANGE_DESKTOP]      = XInternAtom(display, "_NET_WM_ACTION_CHANGE_DESKTOP", false);
	atoms[_NET_WM_ACTION_CLOSE]               = XInternAtom(display, "_NET_WM_ACTION_CLOSE", false);
	atoms[_NET_WM_WINDOW_TYPE]                = XInternAtom(display, "_NET_WM_WINDOW_TYPE", false);
	atoms[_NET_WM_WINDOW_TYPE_DESKTOP]        = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", false);
	atoms[_NET_WM_WINDOW_TYPE_DOCK]           = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", false);
	atoms[_NET_WM_WINDOW_TYPE_TOOLBAR]        = XInternAtom(display, "_NET_WM_WINDOW_TYPE_TOOLBAR", false);
	atoms[_NET_WM_WINDOW_TYPE_MENU]           = XInternAtom(display, "_NET_WM_WINDOW_TYPE_MENU", false);
	atoms[_NET_WM_WINDOW_TYPE_SPLASH]         = XInternAtom(display, "_NET_WM_WINDOW_TYPE_SPLASH", false);
	atoms[_NET_WM_WINDOW_TYPE_NORMAL]         = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", false);
	atoms[_NET_WM_WINDOW_TYPE_DIALOG]         = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", false);
	atoms[_NET_WM_WINDOW_TYPE_UTILITY]        = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", false);
	atoms[_NET_CLOSE_WINDOW]                  = XInternAtom(display, "_NET_CLOSE_WINDOW", false);
	atoms[_NET_MOVERESIZE_WINDOW]             = XInternAtom(display, "_NET_MOVERESIZE_WINDOW", false);
	atoms[_NET_WM_MOVERESIZE]                 = XInternAtom(display, "_NET_WM_MOVERESIZE", false);
	atoms[_NET_WM_ICON]                       = XInternAtom(display, "_NET_WM_ICON", false);
	atoms[_NET_WM_ICON_IMAGE]                 = XInternAtom(display, "_NET_WM_ICON_IMAGE", false);
	atoms[_NET_WM_ICON_SVG]                   = XInternAtom(display, "_NET_WM_ICON_SVG", false);
	atoms[_AEGIS_NET_WM_STATE_DECOR]          = XInternAtom(display, "_AEGIS_NET_WM_STATE_DECOR", false);
	atoms[_AEGIS_NET_WM_STATE_DECOR_TITLE]    = XInternAtom(display, "_AEGIS_NET_WM_STATE_DECOR_TITLE", false);
	atoms[_AEGIS_NET_WM_STATE_DECOR_BORDER]   = XInternAtom(display, "_AEGIS_NET_WM_STATE_DECOR_BORDER", false);
	atoms[_AEGIS_NET_WM_STATE_DECOR_HANDLES]  = XInternAtom(display, "_AEGIS_NET_WM_STATE_DECOR_HANDLES", false);
	atoms[_AEGIS_NET_MAXIMIZED_RESTORE]       = XInternAtom(display, "_AEGIS_NET_MAXIMIZED_RESTORE", false);
	atoms[_AEGIS_NET_VIRTUAL_POS]             = XInternAtom(display, "_AEGIS_NET_VIRTUAL_POS", false);
	atoms[_AEGIS_NET_WM_DESKTOP_MASK]         = XInternAtom(display, "_AEGIS_NET_WM_DESKTOP_MASK", false);
	atoms[_AEGIS_NET_WM_MERGED_TO]            = XInternAtom(display, "_AEGIS_NET_WM_MERGED_TO", false);
	atoms[_AEGIS_NET_WM_MERGED_TYPE]          = XInternAtom(display, "_AEGIS_NET_WM_MERGED_TYPE", false);
	atoms[_AEGIS_NET_WM_MERGE_ORDER]          = XInternAtom(display, "_AEGIS_NET_WM_MERGE_ORDER", false);
	atoms[_AEGIS_NET_WM_MERGE_ATFRONT]        = XInternAtom(display, "_AEGIS_NET_WM_MERGE_ATFRONT", false);
	atoms[_AEGIS_NET_RESTART]                 = XInternAtom(display, "_AEGIS_NET_RESTART", false);
	atoms[_AEGIS_NET_SHUTDOWN]                = XInternAtom(display, "_AEGIS_NET_SHUTDOWN", false);
	atoms[XdndAware]                          = XInternAtom(display, "XdndAware", false);
	atoms[XdndEnter]                          = XInternAtom(display, "XdndEnter", false);
	atoms[XdndLeave]                          = XInternAtom(display, "XdndLeave", false);
	atoms[_KDE_NET_SYSTEM_TRAY_WINDOWS]       = XInternAtom(display, "_KDE_NET_SYSTEM_TRAY_WINDOWS", false);
	atoms[_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR] = XInternAtom(display, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
	atoms[_XROOTPMAP_ID]                      = XInternAtom(display, "_XROOTPMAP_ID", false);
	atoms[_AEGIS_NET_EVENT_NOTIFY]            = XInternAtom(display, "_AEGIS_NET_EVENT_NOTIFY", false);
	atoms[_AEGIS_NET_DOCKAPP_HOLDER]          = XInternAtom(display, "_AEGIS_NET_DOCKAPP_HOLDER", false);
	atoms[_AEGIS_NET_DOCKAPP_PRIO]            = XInternAtom(display, "_AEGIS_NET_DOCKAPP_PRIO", false);
	atoms[_AEGIS_NET_CFG]                     = XInternAtom(display, "_AEGIS_NET_CFG", false);
#endif
	//}}}
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
void Aegis::create_dispatchers() {
	for(int eid = KeyRelease; eid < LASTEvent; eid++) {
		event_registry[eid] = new EventDispatcher;
	}
}
//}}}

//{{{
int error_handler(Display * dpy, XErrorEvent * ev) {
	char err[300];
	XGetErrorText(dpy, ev->error_code, err, 300);
	log_crit(err);

	return 1;
}
//}}}
//{{{
int main(int argc, char ** argv) {
	//set up logging
	openlog("aegiswm", LOG_CONS, LOG_USER);
	log_info("********** Starting AegisWM **********\n");

	XSetErrorHandler(error_handler);
	aegis.run();

	log_info("********** Shutting Down AegisWM **********\n");
	closelog();
}
//}}}

/*
 * change log
 *
 * $Log$
 */

//{{{
#if 0
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

	clients.print();
	clients[c->window()] = c;
	clients.print();

	log_debug("Leaving handleMapRequest()\n");
}
//}}}
//{{{
void Aegis::handleUnmapNotify(XEvent * xev) {
	log_debug("Entering handleUnmapNotify(Xevent * ev)\n");
	XUnmapEvent umap = xev->xunmap;
	Client * c;
	Window win;

	if(clients.exists(umap.window)) {
		c = clients[umap.window];
	}
	else if(clients.exists(umap.event)) {
		c = clients[umap.event];
	}
	else {
		goto exit;  //goto has its place...
	}

	win = c->window();
	c->unmap();
	delete c;
	c = NULL;
	clients.erase(win);

exit:
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
	Client * c = clients.getClient(xbp.window);
	log_debug("Entering handleButtonPress(XEvent * xev)\n");
	log_debug("xev->window == %i\n", (int)xbp.window);
	log_debug("xev->button == %i\n", (int)xbp.button);
	log_debug("xev->state  == %i\n", (int)xbp.state);
	log_debug("(x, y) == (%i, %i)\n", (int)xbp.x_root, (int)xbp.y_root);
	aestate.ppos.set(xbp.x_root, xbp.y_root);

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
		log_debug("No client clicked, acting on root window click\n");
		switch(xbp.button) {
			case 1:
				break;
			case 2:
				break;
			case 3:
				log_debug("Quitting...");
				quit();
				exit(EXIT_SUCCESS);
				break;
		}
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
	Client * c = clients.getClient(xmov.window);

	compressEvent(xmov.window, ev->type, ev);

	if(c) {
		log_debug("Moving window %i to (%i, %i)\n", (int)xmov.window, xmov.x_root, xmov.y_root);
		c->moveTo(xmov.x_root, xmov.y_root);

		//Update the position of the cursor
		aestate.ppos.x = xmov.x_root;
		aestate.ppos.y = xmov.y_root;
	}
	else {
		log_debug("There is no client with window id %i\n", (int)xmov.window);
		log_debug("There is no client with a subwindow id %i\n", (int)xmov.subwindow);
		clients.print();
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
#endif
//}}}
