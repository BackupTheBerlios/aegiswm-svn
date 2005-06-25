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
EventDispatcher::EventDispatcher(ev_t event_type) : sig_map() {
	this->event_type = event_type;
}
//}}}
//{{{
EventDispatcher::~EventDispatcher() {
}
//}}}

//{{{
void EventDispatcher::registerHandler(Window w, aeslot_t handler) {
	///@todo  Provide a way to unregister handlers
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


