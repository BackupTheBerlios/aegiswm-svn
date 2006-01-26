/* file name  : client.cpp
 * author     : Michael Brailsford
 * created    : Tue Oct 05 21:19:36 CDT 2004
 * copyright  : 2004 Michael Brailsford
 * version    : $Revision$
 */

#include "client.h"
#include "aegis.h"

using std::vector;

/// @todo Move Titlebar to its own file
//{{{
Titlebar::Titlebar(Display * dpy, Aegis * aegis, Window pwin, int x, int y, int w, int h) {
	XSetWindowAttributes pattr;

	this->dpy   = dpy;
	this->aegis = aegis;

	//pattr.do_not_propagate_mask = ButtonMotionMask;
	pattr.override_redirect=False;
	pattr.event_mask = ButtonMotionMask | SubstructureRedirectMask | 
		SubstructureNotifyMask          | ButtonPressMask          | 
		ButtonReleaseMask               | ExposureMask             | 
		EnterWindowMask                 | LeaveWindowMask;

	wid = XCreateWindow(dpy, pwin, x, y, w, h, 0,
			DefaultDepth(dpy, aegis->getScreen()),
			CopyFromParent, 
			DefaultVisual(dpy, aegis->getScreen()),
			CWOverrideRedirect | CWDontPropagate | CWBackPixel | CWBorderPixel | CWEventMask, 
			&pattr);
	XMapRaised(dpy, wid);
}
//}}}

//{{{
Client::Client(Display * dpy, Aegis * aegis, Window window) : state() {
	log_debug("Creating a new Client\n");
	//set initial window state
	state.x = state.y = 0;
	state.w = 200;
	state.h = 200;
	state.name = NULL;
	XSetWindowAttributes pattr;
	log_debug("here\n");

	this->dpy = dpy;
	wm = aegis;
	adopted = window;

	//set some window attributes
	//pattr.do_not_propagate_mask = ButtonPressMask|ButtonReleaseMask|ButtonMotionMask;
	pattr.override_redirect=False;
	pattr.event_mask = ButtonMotionMask       | SubstructureRedirectMask | 
					   SubstructureNotifyMask | ButtonPressMask          | 
					   ButtonReleaseMask      | ExposureMask             | 
					   EnterWindowMask        | LeaveWindowMask;

	//get the window's name
	getXWindowName();

	//Create windows
	///determine the requested size and location fo the window that we are adopting
	///and set the dimensions of window accordingly.
	getAdoptedWindowSize();
	
	//make an outer frame big enough for the titlebar and the window we will reparent.
	id = XCreateWindow(dpy, aegis->rootWindow(), state.x, state.y, state.w, state.h + 15, 0,
			CopyFromParent, CopyFromParent, DefaultVisual(dpy, aegis->getScreen()),
			CWOverrideRedirect | /*CWDontPropagate |*/ CWBackPixel | CWBorderPixel | CWEventMask, 
			&pattr);

	///make the titlebar.
	title = new Titlebar(dpy, wm, id, 0, 0, state.w, 15);

	//reparent client into toplevel window
	XReparentWindow(dpy, adopted, id, 0, 16);
	XAddToSaveSet(dpy, adopted);
	
    //Add the default event handlers for new clients
    setupDefaultEventHandlers();

	//focus the client windows
	//XSelectInput(dpy, id, ButtonReleaseMask | ButtonPressMask   | 
                          //FocusChangeMask   | EnterWindowMask   | 
                          //LeaveWindowMask);
	XSelectInput(dpy, window, SubstructureNotifyMask|SubstructureRedirectMask);

	//Map them at the top of the stacking order
	XMapRaised(dpy, adopted);
	XMapRaised(dpy, id);
	log_debug("Done creating new Client\n");
}
//}}}
//{{{
Client::~Client() {
	delete title;
	title = NULL;
	dpy = NULL;
	wm = NULL;
}
//}}}
//{{{
void Client::setupDefaultEventHandlers() {
	wm->registerEventHandler(id, MotionNotify, sigc::mem_fun(this, &Client::move ));
	wm->registerEventHandler(id, UnmapNotify , sigc::mem_fun(this, &Client::unmap));
}
//}}}

//{{{
void Client::getXWindowName() {
	if(state.name) XFree(state.name);

	XFetchName(dpy, adopted, &state.name);

	//if(state.name == NULL) {
		//XStoreName(dpy, adopted, "AegisWM Client Window");
		//XFetchName(dpy, adopted, &state.name);
	//}
}
//}}}
//{{{
void Client::getAdoptedWindowSize() {
	XWindowAttributes wattr;

	XGetWindowAttributes(dpy, adopted, &wattr);
	
	state.x = wattr.x;
	state.y = wattr.y;
	state.w = wattr.width;
	state.h = wattr.height;
}
//}}}
//{{{
vector<Window> Client::getWindowList() {
	vector<Window> rv;
	rv.push_back(id);
	rv.push_back(adopted);
	rv.push_back(title->getWindow());

	return rv;
}
//}}}
//{{{
void Client::move(XEvent * xev) {
	XMotionEvent xmov = xev->xmotion;
	log_debug("Entering Client::move(XEvent *)");
    int x = xmov.x;
    int y = xmov.y;
	int cursor_root_x = xmov.x_root;
    int cursor_root_y = xmov.y_root;
	int dx, dy;
	AegisState * wmstate = wm->getState();

	//get the current x and y coordinates of the top left corner of the window relative to the root
	//window
	//curx = state.x;
	//cury = state.y;
	//get the number of pixels to move.  This is the difference between the x,y passed in and the
	//previous known position of the cursor
	dx = x - state.x;
	dy = y - state.y;

	log_info("x      = %i, y      = %i" , x           , y);
	log_info("x_root = %i, y_root = %i" , cursor_root_x , cursor_root_y);
	//log_info("curx   = %i, cury   = %i" , curx        , cury);
	log_info("dx     = %i, dy     = %i" , dx          , dy);

	//move the window
	XMoveWindow(dpy, id, dx, dy);

	//update our window state
	state.x += dx;;
	state.y += dy;;

	log_debug("Leaving Client::move()");
}
//}}}
//{{{
void Client::unmap(XEvent * xev) {
	log_info("Unmapping the %s windows", state.name);
	//XUnmapSubwindows(dpy, id);
	XUnmapWindow(dpy, id);
	log_info("Done unmapping %s", state.name);
}
//}}}
//{{{
void Client::raise() {
	log_info("Raising %s window", state.name);
	XRaiseWindow(dpy, id);
	XSetInputFocus(dpy, id, RevertToNone, CurrentTime);
	log_info("Done raising %s window", state.name);
}
//}}}
//{{{
Window Client::unparent() {
	log_info("Unparenting %i window", (int)adopted);
	XReparentWindow(dpy, adopted, wm->rootWindow(), state.x, state.y);
	return adopted;
}
//}}}
//foo
