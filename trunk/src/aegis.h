/* file name  : aegis.h
 * authors    : Michael Brailsford
 * created    : Wed Sep 29 23:25:57 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef AEGIS_H
#define AEGIS_H

#include <syslog.h>
#include <stdarg.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include "clientmap.h"
#include <sigc++/sigc++.h>

//Forward declarations
class EventDispatcher;
class Action;

/// @mainpage
/// This is the documentation for AegisWM.

/// This is an event type.
typedef int ev_t;

/// This is the type of event signals in AegisWM.
typedef sigc::signal<void, XEvent *> aesig_t;

/// This is a map of sigc::signals, which will have connected to them the EventDispatcher::dispatch()
/// member function.  So when the signal is emitted it causes the EventDispatcher::dispatch() method
/// to be called.
typedef std::map<ev_t, EventDispatcher *> dispatch_map_t;

/// This is the type of a handler that can be registered as an event handler.  A sigc::slot<void,
/// XEvent> can be created via the sigc::mem_fun(object, object_method) or
/// sigc::ptr_fun(function_ptr).
typedef sigc::slot<void, XEvent *> aeslot_t;

///// This is a map of the core AegisWM actions.  There are the actions which are the most fundamental
///// for a window manager.  They deal with Atoms, and are generally so intimately coupled with the
///// Aegis object itself that it is a strain to make them full fledged Action objects.
//typedef std::map<ev_t, aeslot_t> core_actions_t;

/// This struct represents a single (x,y) point.
//{{{
struct point {
	int x;  ///< The x coordinate of the point
	int y;  ///< The y coordinate of the point

	/// Create a point with coordinates (x, y).
	//{{{
	point(int x = 0, int y = 0) {
		this->x = x;
		this->y = y;
	}
	//}}}

	/// Set the point to be (x, y).
	//{{{
	inline void set(int x, int y) {
		this->x = x;
		this->y = y;
	}
	//}}}
};
//}}}

/// This struct tracks the internal state of Aegis.  It tracks things such as if we can move a
/// window.  This allows us to enforce a state machine view of certain events.  For example, we
/// cannot move a window unless a button is currently pressed, we move that window until the button
/// is unpressed.  That fits well into a state transition model.
//{{{
struct AegisState {
	/// Is a button currently pressed?
	bool button_down;
	/// This is pointer position as of the last XButtonPressEvent.
	point ppos; 
};
//}}}

//// Simple logging stuff {{{
#ifdef __GNUC__
//This stuff will need to be replaced for any preprocessor other than the GNU preproccessor.  The
//__VA_ARGS__ is a GNU cpp extension, as is the ##.  The ## gets rid of any extra ',' if there are
//no va_args.  i.e.  without the '##', then 'log_info("foobar")', would be expanded to
//'log_info("foobar",)'.  With the ## then trailing ',' will be deleted if there are no va_args.

/// Macro to log with the info severity.
#define log_info(fmt, ...)    syslog(LOG_INFO, fmt,##__VA_ARGS__)
/// Macro to log with the debug severity.
#define log_debug(fmt, ...)   syslog(LOG_DEBUG, fmt,##__VA_ARGS__)
/// Macro to log with the notice severity.
#define log_notice(fmt, ...)  syslog(LOG_NOTICE, fmt,##__VA_ARGS__)
/// Macro to log with the warning severity.
#define log_warning(fmt, ...) syslog(LOG_WARNING, fmt,##__VA_ARGS__)
/// Macro to log with the err severity.
#define log_err(fmt, ...)     syslog(LOG_ERR, fmt,##__VA_ARGS__)
/// Macro to log with the crit severity.
#define log_crit(fmt, ...)    syslog(LOG_CRIT, fmt,##__VA_ARGS__)
/// Macro to log with the alert severity.
#define log_alert(fmt, ...)   syslog(LOG_ALERT, fmt,##__VA_ARGS__)
/// Macro to log with the emerg severity.
#define log_emerg(fmt, ...)   syslog(LOG_EMERG, fmt,##__VA_ARGS__)
#else
/// Function to log with the info severity.
//{{{
void log_info(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
    vsyslog(LOG_INFO, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the debug severity.
//{{{
void log_debug(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
    vsyslog(LOG_DEBUG, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the notice severity.
//{{{
void log_notice(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_NOTICE, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the warning severity.
//{{{
void log_warning(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_WARNING, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the err severity.
//{{{
void log_err(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_ERR, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the crit severity.
//{{{
void log_crit(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_CRIT, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the alert severity.
//{{{
void log_alert(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_ALERT, fmt, args);
	va_end(args);
}
//}}}
/// Function to log with the emerg severity.
//{{{
void log_emerg(char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsyslog(LOG_EMERG, fmt, args);
	va_end(args);
}
//}}}
#endif
//}}}

//forward declaration
class Client;

/// This is the enum of all Atoms that we care about.
//{{{
enum AtomIDs {
	XA_WM_CLASS = 0,
	XA_WM_HINTS,
	XA_WM_ICON_NAME,
	XA_WM_NAME,
	XA_WM_NORMAL_HINTS,
	XA_WM_TRANSIENT_FOR_HINT,
	XA_WM_ZOOM_HINTS,
	WM_CLIENT_MACHINE,
	XA_WM_COMMAND,
};
//}}}


/// This is the main Aegis class.
//{{{
class Aegis {
	protected:
		/// Our Display.
		Display * dpy;
		/// Our screen.
		int scr;            
		/// The root window ID.
		Window root;  
		/// The ubiquitous arrow pointer.
		Cursor arrow_curs;      
		/// This is our list of clients.
		ClientMap clients;       

		/// This is the map of XEvent types to signals.
		dispatch_map_t event_registry;

		/// This contains all the Atoms we have interned (In the future would should probably
		/// subclass std::map<> so we can specify how to intern an Atom if one is requested that is
		/// not in the map).
		std::map<AtomIDs, Atom> atoms;

		/// This discards all superflous events on the window specified, and with the given
		/// event_type.  It returns the number of events of type event_type it skipped.
		int compressEvent(Window win, int event_type, XEvent * ev);

		AegisState aestate;
	public:
		/// Construct the Aegis window manager.
		Aegis();

		/// Destroy the Aegis window manager and all its resources.
		~Aegis();

		/// Run the event loop
		void run();

		/// Unparent all client windows and close down.
		void quit();

		/// Intern the Atoms we care about.
		void internAtoms();

		/// Loops over all windows in X and wraps them in a Client object.  This is called once,
		/// when Aegis is initiated.  It makes sure that any window which is already mapped in the X
		/// server is reparented into a Client widget if the override_redirect flag is not set.
		void reparentExistingWindows();


		/// Returns the Window ID of the root window.
		inline Window rootWindow() { return root; }

		/// Get a pointer to the AegisState struct.
		inline AegisState * getState() { return &aestate; }

		/// Returns the Screen.
		inline int getScreen() { return scr; }

        /// Returns the Display.
        inline Display * getDisplay() { return dpy; }

		/// This registers an event handler to an EventDispatcher in Aegis.  Calls
		/// to this method will look  something like:   
		/// <div><code>aegis->registerEventHandler(window_id, KeyPress,
		/// sigc::mem_fun(client_object, handle_key_press)<code></div>
		/// @param w		  This is the window ID of the window for which we are
		///                   registering.
		/// @param event_type This is the XEvent type for which are registering the
		///                   dispatcher.
		/// @param handler    This is an aeslot_t created with sigc::mem_fun() or
		///                   sigc::ptr_fun().
		void registerEventHandler(Window w, ev_t event_type, aeslot_t handler);

		/// This registers an event handler to an EventDispatcher in Aegis.  Calls
		/// to this method will look something like:   
		/// <div><code>aegis->registerEventHandler(window_id, KeyPress, action_obj)<code></div>
		/// @param w		  This is the window ID of the window for which we are
		///                   registering.
		/// @param handler    This is an Action object that will have its Action::execute() method
		///                   called to handle the XEvent.
		void registerEventHandler(Window w, Action * handler);

		/// This registers all of the core event dispatchers.
		void create_dispatchers();

		/// Sets up the default set of handlers.
		void setupDefaultHandlers();

		/// This handles an XMapRequest event.
		/// @param ev This is XEvent union with all the details about the MapRequest.
		void handleMapRequestEvent(XEvent * ev);

		/// This handles (re)drawing all of our decorations when a window is exposed.
		/// @param ev This is XEvent union with all the details about the ExposeEvent.
		void handleExposeEvent(XEvent * ev);

        /// This creates a new Client object and adds it to the internal data
        /// structures required to properly manage the new Client window.
        Client * makeNewClient(Window w);
};
//}}}
#endif
