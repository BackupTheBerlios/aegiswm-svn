/* file name  : exposeeventaction.cpp
 * author     : Michael Brailsford
 * created    : Tue Apr 11 00:28:16 CDT 2006
 * copyright  : 2006 Michael Brailsford
 */

#include "window_mgmt_builtins.h"

//ExposeEventAction
//{{{
ExposeEventAction::ExposeEventAction(ev_t triggering_event, Aegis * wm) : BuiltinAction(triggering_event, wm) {
}
//}}}
//{{{
void ExposeEventAction::execute(XEvent * ev) {
	XExposeEvent xev = ev->xexpose;

	//(Re)Draw the client window decorations
	XClearWindow(aegis->getDisplay(), xev.window);
}
//}}}

//MapRequestAction
//{{{
MapRequestAction::MapRequestAction(ev_t triggering_event, Aegis * wm) : BuiltinAction(triggering_event, wm) {
}
//}}}
//{{{
void MapRequestAction::execute(XEvent * ev) {
	Window w = ev->xmaprequest.window;
    Client * client;
	log_info("In MapRequestAction::execute()");

    //Create a new client window
    client = makeNewClient(w);

    if( client == NULL ) {
        log_warning("Could not create client window.");
    }
}
//}}}

//////////////////// Generated classes go here ////////////////////
