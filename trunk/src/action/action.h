/* file name  : action.h
 * authors    : Michael Brailsford
 * created    : Sun Oct 31 23:28:22 CST 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef ACTION_H
#define ACTION_H

#include "aegis.h"

/// This class represents some type of action that can be triggered by an event
/// in Aegis.  It provides the main abstraction for executing commands via fork/exec,
/// a scripting language, or some builtin action.  This is an abstract class, it should
/// be subclassed to provide some type of Action functionality.  The execute(XEvent *)
/// method should be provided to perform the action.
class Action {
	private:
		//none

	protected:
		/// This is the event type of the event that will trigger this action.
		ev_t trigger;

	public:
		/// This is the default constructor for an Action.
		Action(ev_t triggering_event) {
			trigger = triggering_event;
		}
		
		/// Typical destructor.
		virtual ~Action() {}

		/// Perform the action specified.  All Actions will probably not need
		/// the XEvent param, but including it here, makes an Action eligble
		/// to be added as the function pointer in a sigc::signal that is
		/// suitable for evsubsys.
		virtual void execute(XEvent * ev) = 0;

		/// Returns the event id of the event that triggers this action.
		inline ev_t getTriggerEvent() { return trigger; }

		/// Sets the event id of the event that triggers this action.
		inline void setTriggerEvent(ev_t event) { trigger = event; }
};
#endif
