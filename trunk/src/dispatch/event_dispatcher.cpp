/* file name  : eventdispatcher.cpp
 * author     : Michael Brailsford
 * created    : Sun Mar 27 02:57:17 CST 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include <map>
#include "event_dispatcher.h"

using std::map;

//{{{
EventDispatcher::EventDispatcher() : sig_map() {
}
//}}}
//{{{
EventDispatcher::~EventDispatcher() {
}
//}}}

//{{{
void EventDispatcher::registerHandler(Window w, aeslot_t handler) {
	sig_map[w].connect(handler);
}
//}}}
//{{{
void EventDispatcher::dispatch(XEvent * event) {
	Window w = determineWindowId(event);

	if(sig_map.find(w) != sig_map.end()) {
		sig_map[w].emit(event);
	}
	else {
		log_info("No handlers registered for Window = %i", (int)w);
	}
}
//}}}
//{{{
Window EventDispatcher::determineWindowId(XEvent * event) {
	return event->xany.window;
}
//}}}

/*
 * change log
 *
 * $Log$
 */


