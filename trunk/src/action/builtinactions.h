/* file name  : builtinactions.h
 * authors    : Michael Brailsford
 * created    : Thu Apr 29 20:19:02 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef BUILTINACTIONS_H
#define BUILTINACTIONS_H

#include "aegis.h"
#include "builtinids.h"
#include <actions/action.h>
#include <actions/systemaction.h>
#include <actions/signalaction.h>

/// This encapsulates all the builtin actions for Aegis.  It is essentially a
/// map of Action pointers. It is highly coupled with the Aegis class.  This
/// is how it has to be since builtin actions take action on the window manager
/// itself or on behalf of the window manager.
class BuiltinActions {
	protected:
		/// Our lifeline to the Aegis object.
		Aegis * aegis;

		/// This is the map of Actions that compose the Aegis builtin actions.
		/// Things like viewport moving, quitting, restarting belong here.  If
		/// you want to add a builtin, you need to add an entry to the builtin_t
		/// enum, then add a corresponding action to the map.
		std::map<builtin_t, Action *> builtins;

		/// This method creates all the run state Actions.  These are the
		/// actions to quit, restart, and whatever else might cause Aegis to
		/// alter its running state.
		void makeRunStateActions();

        /// This method creates all the builtins to deal with window mapping
        /// an drawing.
        void makeWindowMappingActions();

	public:
		/// Create all the Aegis builtin actions, and store them in a way that
		/// is efficient to retrieve them when needed.  It should be noted that
		/// there is nothing fancy about builtin actions.  This class just moves
		/// the nastiness from the Aegis class, where it would simply clutter
		/// and confuse.  The builtin actions are just that, builtin, static,
		/// hardcoded.  Every effort has been made to make it simple to add
		/// builtins easily, and access them easily.  Just don't expect any 
		/// fancy code.  This is all brute force...  
		BuiltinActions(Aegis * aegis);

		/// Destroy the builtin actions.
		virtual ~BuiltinActions();

		/// Retrieve the builtin action for the id passed in, and bind it to the
		/// event type passed in.
		///
		/// @param event The event type to which we bind the builtin action.
		/// @param id The id of the builtin that we want.
		/// @return The Action that was specified by 'id'.
		inline Action * getBuiltin(ev_t event, builtin_t id) {
			if(id >= AEGIS_FIRST_BUILTIN && id < AEGIS_LAST_BUILTIN) {
				Action * act =  builtins[id];
				act->setTriggerEvent(event);
				return act;
			}
			else
				return NULL;
		};
};
#endif
