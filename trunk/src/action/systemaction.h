/* file name  : systemaction.h
 * authors    : Michael Brailsford
 * created    : Thu Apr 29 19:25:12 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef SYSTEMACTION_H
#define SYSTEMACTION_H

#include "action.h"
#include <string>

class SystemAction : public Action {
	protected:
		/// This is the command to execute.
		std::string cmd;

	public:
		/// This creates a SystemAction which will invoke 'cmd' with fork/exec.
		///
		/// @param trigger The event that will trigger this action.
		/// @param cmd The command to invoke.
		SystemAction(ev_t trigger, std::string cmd);
		~SystemAction();

		/// Execute this action's command in a subshell.
		virtual void execute(XEvent * ev);
};
#endif
