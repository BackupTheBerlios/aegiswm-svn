/* file name  : eventdispatcher.cpp
 * author     : Michael Brailsford
 * created    : Sun Mar 27 02:57:17 CST 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include <map>
#include "event_dispatcher.h"
#include "event_state.h"

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
void EventDispatcher::registerHandler(Window w, aeslot_t handler, EventState * ev_state) {
	log_info("Entering EventDispatcher::registerHandler(Window %i, handler, ev_state)", (int)w);
	///@todo  Provide a way to unregister handlers
	if(ev_state == NULL) {
		sig_map[w][new EventState].connect(handler);
	}
	else {
		sig_map[w][ev_state].connect(handler);
	}
	log_info("Leaving EventDispatcher::registerHandler(Window %i, handler, ev_state)", (int)w);
}
//}}}
//{{{
void EventDispatcher::dispatch(XEvent * event) {
	Window w = determineWindowId(event);
	ev_state_sigmap::iterator iter, end;
	bool signalled = false;

	if(sig_map.find(w) != sig_map.end()) {
		ev_state_sigmap state_signal_map = sig_map[w];
		end = state_signal_map.end();

		for(iter = state_signal_map.begin(); iter != end; iter++) {
			///@todo Fix the EventState::operator==(XEvent*) to actually compare the equality of an
			//XEvent and an EventState object.  This might require beefing up EventState objects.
			if((*iter->first) == event) {
				iter->second.emit(event);
				signalled = true;
			}
		}
		
		if(!signalled) {
			log_info("No handlers registered for this Window = %i and Event %i", (int)w, event->type);
		}
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


