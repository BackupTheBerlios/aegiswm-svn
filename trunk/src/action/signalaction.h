/* file name  : signalaction.h
 * authors    : Michael Brailsford
 * created    : Thu May 06 20:08:25 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef SIGNALACTION_H
#define SIGNALACTION_H

#include "action.h"
#include <sigc++/sigc++.h>

/// This action occurs has a member which is a sigc::signal that is emitted when
/// execute() is called.
class SignalAction : public Action {
	protected:
		/// The signal that we will emit when execute() is called.
		sigc::signal<void, XEvent *> my_sig;

	public:
		/// Create a SignalAction which is an action that will emit a signal
		/// when it is executed.
		SignalAction(aeslot_t slot);

		/// Destroy this SignalAction.
		~SignalAction();

		/// Perform the action.
		///
		/// @param ev The XEvent that triggered the actoin.
		inline void execute(XEvent * ev) {
			my_sig.emit(ev);
		}
};
#endif
