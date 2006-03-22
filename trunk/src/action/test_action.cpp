/* file name  : testaction.cpp
 * author     : Michael Brailsford
 * created    : Tue Mar 21 22:57:37 CST 2006
 * copyright  : 2006 Michael Brailsford
 */

#include "test_action.h"

//{{{
TestAction::TestAction(ev_t triggering_event) : Action(triggering_event) {
}
//}}}

//{{{
void TestAction::execute(XEvent * ev) {
	printf("Test (ev.type = %i)\n", ev->type);
}
//}}}

