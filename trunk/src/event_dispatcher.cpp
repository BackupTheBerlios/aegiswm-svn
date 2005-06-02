/* file name  : eventdispatcher.cpp
 * author     : Michael Brailsford
 * created    : Sun Mar 27 02:57:17 CST 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include "eventdispatcher.h"

using std::map;

//{{{
EventDispatcher::EventDispatcher() : sig_map() {
}
//}}}
//{{{
EventDispatcher::~EventDispatcher() {
}
//}}}

///TODO  Determine how to best register event handlers.
//{{{
void EventDispatcher::registerHandler(Window w, aesig_t * signal) {
	if(sig_map.find(w) != sig_map.end()) {
		sig_map[w] = signal;
	}
}
//}}}
//{{{
void EventDispatcher::dispatch(XEvent * event) {
	Window w = determineWindowId(event);
	if(sig_map.find(w) != sig_map.end()) {
		sig_map[w]->emit(event);
	}
	else {
		log_info("No handlers registered for Window = %i", k);
	}
}
//}}}
//{{{
Window EventDispatcher::determineWindowId(XEvent * event) {
	return event->window;
}
//}}}

/*
 * change log
 *
 * $Log$
 */


