/* file name  : systemaction.cpp
 * author     : Michael Brailsford
 * created    : Thu Apr 29 19:39:05 CDT 2004
 * copyright  : 2004 Michael Brailsford
 */

#include "systemaction.h"
#include <sys/types.h>
#include <unistd.h>

//{{{
SystemAction::SystemAction(ev_t trigger, std::string command) : Action(trigger), cmd(command) {
}
//}}}
//{{{
SystemAction::~SystemAction() {
}
//}}}
//{{{
void SystemAction::execute(XEvent * ev) {
	if(cmd.length() > 0) {
		pid_t pid = fork();

		if(!pid) {
			///TODO:  Change the /bin/sh to the shell specified in the $SHELL environment variable
			execlp("/bin/sh", "sh", "-c", cmd.c_str(), NULL);

			//we should never get here
			exit(EXIT_FAILURE);
		}
	}
}
//}}}
