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

	this->dpy = dpy;
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
			CWOverrideRedirect | /*CWDontPropagate |*/ CWBackPixel | CWBorderPixel | CWEventMask, 
			&pattr);
	XMapRaised(dpy, wid);
}
//}}}

//{{{
Client::Client(Display * dpy, Aegis * aegis, Window window) : state() {
	syslog(LOG_DEBUG, "Creating a new Client\n");
	//set initial window state
	state.x = state.y = 0;
	state.w = 200;
	state.h = 200;
	state.name = NULL;
	XSetWindowAttributes pattr;
	syslog(LOG_DEBUG, "here\n");

	this->dpy = dpy;
	wm = aegis;
	adopted = window;

	//set some window attributes
	pattr.do_not_propagate_mask = ButtonPressMask|ButtonReleaseMask|ButtonMotionMask;
	pattr.override_redirect=False;
	pattr.event_mask = ButtonMotionMask | SubstructureRedirectMask | 
		SubstructureNotifyMask          | ButtonPressMask          | 
		ButtonReleaseMask               | ExposureMask             | 
		EnterWindowMask                 | LeaveWindowMask;

	//get the window's name
	getXWindowName();

	//Create windows
	///determine the requested size and location fo the window that we are adopting
	///and set the dimensions of window accordingly.
	getAdoptedWindowSize();
	
	//make an outer frame big enough for the titlebar and the window we will reparent.
	id = XCreateWindow(dpy, aegis->rootWindow(), state.x, state.y, state.w, state.h + 15, 0,
			CopyFromParent, CopyFromParent, DefaultVisual(dpy, aegis->getScreen()),
			CWOverrideRedirect | CWDontPropagate | CWBackPixel | CWBorderPixel | CWEventMask, 
			&pattr);

	///make the titlebar.
	title = new Titlebar(dpy, wm, id, 0, 0, state.w, 15);

	//reparent client into toplevel window
	XReparentWindow(dpy, adopted, id, 0, 16);
	
	//focus the client windows
	XSelectInput(dpy, id, ButtonReleaseMask | ButtonPressMask   | 
                          FocusChangeMask   | EnterWindowMask   | 
                          LeaveWindowMask);

	//Map them at the top of the stacking order
	XMapRaised(dpy, adopted);
	XMapRaised(dpy, id);
	syslog(LOG_DEBUG, "Done creating new Client\n");
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
void Client::moveTo(int x, int y) {
	XMoveWindow(dpy, id, x, y);
}
//}}}
//{{{
void Client::unmap() {
	log_info("Unmapping the %s windows", state.name);
	XUnmapSubwindows(dpy, id);
	delete this;
	log_info("Done unmapping %s", state.name);
}
//}}}
//{{{
void Client::raise() {
	log_info("Raising %s window", state.name);
	XRaiseWindow(dpy, id);
	log_info("Done raising %s window", state.name);
}
//}}}
