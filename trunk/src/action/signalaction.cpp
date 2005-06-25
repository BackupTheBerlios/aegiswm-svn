/* file name  : signalaction.cpp
 * author     : Michael Brailsford
 * created    : Thu May 06 20:14:27 CDT 2004
 * copyright  : 2004 Michael Brailsford
 */

#include "signalaction.h"

//{{{
SignalAction::SignalAction(aeslot_t slot) : Action(LASTEvent), my_sig() {
	my_sig.connect(slot);
}
//}}}
//{{{
SignalAction::~SignalAction() {
}
//}}}
