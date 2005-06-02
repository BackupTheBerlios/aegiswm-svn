/* file name  : event_dispatcher.h
 * authors    : Michael Brailsford
 * created    : Sun Mar 06 02:04:59 CST 2005
 * copyright  : (c) 2005 Michael Brailsford
 * version    : $Revision$
 */

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <map>
#include <X11/X.h>
#include <sigc++/sigc++.h>

/// Instances of this class will dispatch events to specific Client of event handlers.  Objects of this
/// class are signalled when a given XEvent type has occurred.  Then, based on the window id,
/// objects of this class will signal the appropriate Clients to handle the event.  Objects of this
/// class maintain a mapping of sigc::signals.  The lifespan of an EventDispatcher is:
/// <ol>
/// <li>Creation during AegisWM initialization</li>
/// <li>When clients are created in AegisWM, they register signals with the appropriate
/// EventDispatcher for the XEvent.  This registration is done by connecting the
/// EventDispatcher::dispatch() member function with the signal for that event type.</li>
/// <li>When AegisWM recieves an XEvent, the main Aegis object determines which EventDispatcher is
/// appropriate for the XEvent type.</li>
/// <li>Aegis calls EventDispatcher::dispatch() by emitting the appropriate signal.</li>
/// <li>EventDispatcher::dispatch() determines which window is to recieve the event.</li>
/// <li>EventDispatcher::dispatch() calls the appropriate EventHandler by calling
/// sigc::signal::emit() on the appropriate signal.</li>
/// </ol>
//{{{
class EventDispatcher : public sigc::trackable {
	private:
		/// Maps the window number to the signal that a Client has registered.
		std::map<Window, aesig_t *> sig_map;

	protected:
		//none
	public:
		EventDispatcher();
		~EventDispatcher();

		/// This registers a signal to the key passed in.
		/// @param w The window ID of the window for which this signal is registered.
		/// @param signal The signal that is associated with the Window ID parameter.
		void registerHandler(Window w, aesig_t * signal);

		/// This signals the event handler for the key, k, passed in.
		/// @param event The event that triggered this event to be sent to this EventDispatcher.
		void dispatch(XEvent * event);

		/// This determines the correct window ID based on the event type.  This method should be
		/// overridden by sublasses to provide the correct window id for the XEvent type.
		/// @param event The XEvent for which we will determine the window ID.
		/// @return The window to which this event applies.
		Window determineWindowId(XEvent * event);
};
//}}}
#endif

/*
 * change log
 *
 * $Log$
 */

