/* file name  : builtinaction.h
 * authors    : Michael Brailsford
 * created    : Tue Apr 11 00:12:48 CDT 2006
 * copyright  : (c) 2006 Michael Brailsford
 */

#ifndef BUILTINACTION_H
#define BUILTINACTION_H

#include "action.h"
#include "builtinids.h"

/// This class represents one builtin action.  All builtin AegisWM actions
/// should be subclasses of this class.
class BuiltinAction : public Action {
	private:
        //none
	protected:
        Aegis * aegis;
	public:
        //This creates a new builtin action
		BuiltinAction(ev_t triggering_event, Aegis * wm);
		virtual ~BuiltinAction();
};
#endif

