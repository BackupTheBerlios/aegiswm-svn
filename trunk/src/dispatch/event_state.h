/* file name  : event_state.h
 * authors    : Michael Brailsford
 * created    : Sun Jun 26 20:21:46 CDT 2005
 * copyright  : (c) 2005 Michael Brailsford
 * version    : $Revision$
 */

#ifndef EVENT_STATE_H
#define EVENT_STATE_H

#include <X11/Xlib.h>

/// This is to determine the "state" of an event.  That is the keycodes, key masks, and/or buttons
/// of the event, It may also encapsulate any other things that may be pertinent to determine
/// whether or not an XEvent matches a specific condition.  This is design to deal with only the key
/// and button events.  If other event states are needed then this class should be made pure
/// virtual, and concrete classes made for each type of XEvent that needs its own EventState.
class EventState {
	protected:
	public:
		unsigned int keycode_button;
		unsigned int mask;

		/// Create an empty EventState.
		EventState();

		/// This creates an EventState object for KeyPress/Release or ButtonPress/Release events.
		/// @param korb This is the keycode or button number.
		/// @param mask This is the button mask/masks for the event (such as alt, control, etc..).
		EventState(unsigned int korb, unsigned int mask);

		~EventState();

		/// This determines if the XEvent passed in matches this EventState.
		bool operator==(XEvent * ev);
};

/// This is a function object that provides the STL all it needs to use an EventState pointer as the
/// key in an std::map.  Less than comparison is rather arbitrary for EventStates, so we only need
/// to provide the following properties:
///   For cases when a is less than b:
///     a < b = true
///     b < a = false
///
///   For cases when a is greater than b:
///     a < b = false
///     b < a = true
///
///   For cases when a is equal to b:
///     a < b = false
///     b < a = false
//{{{
struct EventStateLess : public std::binary_function<EventState *, EventState *, bool> {
	bool operator()(EventState * first, EventState * second) {
		if(first->keycode_button == second->keycode_button)
			return first->mask < second->mask;
		else
			return first->keycode_button < second->keycode_button;
	}
};
//}}}

#endif

/*
 * change log
 *
 * $Log$
 */

