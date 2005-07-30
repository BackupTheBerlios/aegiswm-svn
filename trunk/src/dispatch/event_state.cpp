/* file name  : eventstate.cpp
 * author     : Michael Brailsford
 * created    : Sun Jun 26 20:35:09 CDT 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include "event_state.h"

//{{{
EventState::EventState() {
	keycode_button = 0;
	mask = 0;
}
//}}}
//{{{
EventState::EventState(unsigned int korb, unsigned int mask) {
	keycode_button = korb;
	this->mask = mask;
}
//}}}
//{{{
EventState::~EventState() {
}
//}}}

//{{{
bool EventState::operator==(XEvent * ev) {
	bool rv = false;

	switch(ev->type) {
		case KeyPress:
		case KeyRelease:
			rv = ev->xkey.state == mask;
			rv &= ev->xkey.keycode == keycode_button;
			break;
		case ButtonPress:
		case ButtonRelease:
			rv = ev->xbutton.state == mask;
			rv &= ev->xbutton.button == keycode_button;
			break;
		default:
			rv = true;
	}

	return rv;
}
//}}}


/*
 * change log
 *
 * $Log$
 */


