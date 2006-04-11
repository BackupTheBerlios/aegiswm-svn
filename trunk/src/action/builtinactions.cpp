/* file name  : builtinactions.cpp
 * author     : Michael Brailsford
 * created    : Fri May 07 00:04:38 CDT 2004
 * copyright  : 2004 Michael Brailsford
 */

#include "builtinactions.h"

using namespace sigc;
using std::map;

//{{{
BuiltinActions::BuiltinActions(Aegis * aegis) : builtins() {
	this->aegis = aegis;

	makeRunStateActions();
    makeWindowMappingActions();

	//add additional methods that make builtins below here, keep the specific
	//code of making the actions out of the constructor.  If everyone just added
	//the builtins in the constructor it would be one big unreadable blob of
	//crap.  Put the code to add an action in a method and call that method from
	//here, or die!
}
//}}}
//{{{
BuiltinActions::~BuiltinActions() {
	map<builtin_t, Action *>::iterator iter, end = builtins.end();

	//loop over all the Actions and delete them...
	for(iter = builtins.begin(); iter != end; iter++)
		delete (*iter).second, (*iter).second = NULL;

	builtins.clear();
}
//}}}

//{{{
void BuiltinActions::makeRunStateActions() {
	//Add the quit action
	builtins[AEGIS_QUIT] = new SignalAction(sigc::mem_fun(*aegis, &Aegis::quit));
}
//}}}

//{{{
void BuiltinActions::makeWindowMappingActions() {
	builtins[AEGIS_EXPOSE_EVENT]      = new ExposeEventAction(Expose, aegis);
	builtins[AEGIS_MAP_REQUEST_EVENT] = new MapRequestAction(MapRequest, aegis);
}
//}}}
