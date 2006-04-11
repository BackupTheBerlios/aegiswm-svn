/* file name  : builtinaction.cpp
 * author     : Michael Brailsford
 * created    : Tue Apr 11 00:17:31 CDT 2006
 * copyright  : 2006 Michael Brailsford
 */

#include "builtinaction.h"

//{{{
BuiltinAction::BuiltinAction(ev_t triggering_event, Aegis * wm) : Action(triggering_event) {
    aegis = wm;
}
//}}}
//{{{
BuiltinAction::~BuiltinAction() {
    aegis = NULL;
}
//}}}
