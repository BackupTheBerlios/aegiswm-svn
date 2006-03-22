/* file name  : eventstate.cpp
 * author     : Michael Brailsford
 * created    : Sun Jun 26 20:35:09 CDT 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include "event_state.h"
#include "aegis.h"
#include "limits.h"

//{{{
EventState::EventState() {
	keycode_button = UINT_MAX;
	mask = UINT_MAX;
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

    log_info("ev->type = %i", ev->type);

    //return true is mask and keycode_button are both still set to default
    //values
    if(mask == UINT_MAX && keycode_button == UINT_MAX) {
        rv = true;
    }
    else {
        switch(ev->type) {
            case KeyPress:
            case KeyRelease:
                //Just use the same structure are XButtonEvents, they are
                //identical to XKeyEvents.
            case ButtonPress:
            case ButtonRelease:
                rv  = mask == UINT_MAX           || ev->xbutton.state == mask;
                rv &= keycode_button == UINT_MAX || ev->xbutton.button == keycode_button;
                break;
            default:
                rv = true;
        }
    }
    log_info("rv = %s\n", rv ? "true": "false");

	return rv;
}
//}}}


/*
 * change log
 *
 * $Log$
 */


