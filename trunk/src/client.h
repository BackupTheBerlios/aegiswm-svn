/* file name  : client.h
 * authors    : Michael Brailsford
 * created    : Tue Oct 05 20:41:46 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 * version    : $Revision$
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <vector>

//forward declaration
class Aegis;

/// This struct encapsulates the state of a client window.
/// @todo WindowState needs to be made more comprehensive.  Right now it suffices, but we want
/// it to encapsulate <b>all</b> the state of a window.
//{{{
struct WindowState {
	char * name; ///< This is the Window's name.
	int x; ///< The x-coordinate of the window.
	int y; ///< The y-coordinate of the window.
	int h; ///< The height of the window.
	int w; ///< The width of the window.

	/// Set initial state to all zeros.
	//{{{
	WindowState() {
		x = y = h = w = 0;
	}
	//}}}

	/// Set initial state to the values passed in.
	//{{{
	WindowState(int x, int y, int h, int w) {
		this->x = x;
		this->y = y;
		this->h = h;
		this->w = w;
	}
	//}}}
	
	/// Destroy this WindowState's resources.
	//{{{
	WindowState::~WindowState() {
	}
	//}}}
};
//}}}

/// This is a Titlebar object.  I assume it will eventually be moved to a file of its own, but for
/// now it remains here.  The Titlebar will be mapped when its parent is mapped.
/// @todo Move this class to its own file
//{{{
class Titlebar {
	protected:
		/// This is the ubiquitous X Display, needed for all Xlib function calls.
		Display * dpy;

		/// This is our link to the rest of Aegis.
		Aegis * aegis;

		/// This is the Window ID of the Titlebar.
		Window wid;

		/// This is the state of the Titlebar window.
		WindowState state;

	public:

		/// Create a new Titlebar object.
		///
		/// @param dpy  This si the X Display.
		/// @param pwin This is the parent window for the Titlebar.
		/// @param x    This is the x coordinate of the Titlebar, relative to pwin's origin.  
		///               It defaults to 0.
		/// @param y    This is the y coordinate of the Titlebar, relative to pwin's origin.  
		///               It defaults to 0.
		/// @param w    This is the width of the Titlebar.
		/// @param h    This is the height of the Titlebar.
		Titlebar(Display * dpy, Aegis * aegis, Window pwin, int x, int y, int w, int h);
		
		
		/// Get the Window ID for the Titlebar.
		inline Window getWindow() { return wid; }

		/// Get the current state of the Titlebar.
		inline WindowState & getState() { return state; }
};
//}}}

/// A Client window is the parent of all windows in Aegis.  It handles all construction, destruction
/// and management of window resources.  Its constructor expects the display and the window that this
/// client will reparent.
//{{{
class Client {
	private:
		/// Determines the name of the X Window.  This is called when the Client is first created.
		/// It should not need to be called after that.  This will set the WindowState::name field,
		/// overwriting any value that was previously there.
		void getXWindowName();

		/// Get the size and location that a window has requested when it was created.  This is only
		/// called once in the Client() constructor.
		void getAdoptedWindowSize();

	protected:
		/// This is X Display on which AegisWM is being run.
		Display * dpy;

		/// This is the link to the window manager.  Useful for accessing certain data structures
		/// and functionality.
		Aegis * wm;

		/// This is the state of this window.  State includes (x, y) position, and hieght and width
		/// of the window.  See struct WindowState.
		WindowState state;

		/// This is the window ID for this Client object.
		Window id;

		/// This is the window that we reparent.
		Window adopted;

		/// This is the Titlebar for the Client.
		Titlebar * title;

	public:
		/// This creates a new Aegis Client window.  Creating a client window in Aegis is a two step
		/// process.  The first step sets initial window state.  The second state creates all
		/// subwindows, these include any and all window decorations such as titlebars, resize bars
		/// at the bottom of a window, etc...
		/// 
		/// This constructor is called when a window has been created, but not yet mapped.  When
		/// this constructor exits, the window will be mapped (visible) on the screen.  Other
		/// characteristics of when this constructor is called are:
		///  - An application has requested a window be created on the display we manage.
		///  - The window has not yet been mapped.
		///  - The application has requested that its window have certain dimensions.
		///
		/// After this constructor exits the following will be true:
		///  - The client window will be reparented into a parent window that Aegis will manage.
		///  - The client window and all parent windows will be visible on the screen unless the
		///    application requests the initial state be something that precludes it.
		Client(Display * dpy, Aegis * aegis, Window window);

		/// Destroy this Client and all its resources.
		~Client();

		/// This returns the Window ID of the window that we adopted when this Client object was
		/// created.
		inline Window getAdoptedWindowID() { return adopted; }

		/// This returns a list (ok, really a vector) of all the windows that make up this Client
		/// object.
		std::vector<Window> getWindowList();

		/// Move a Client object.
		void moveTo(int x, int y);

		/// Raise this client to the top of te stacking order and set input focus.
		void raise();
		
		/// Unmap this client and all its subwindows.
		void unmap();

		/// Returns the top level window for this Client object.
		inline Window getTopLevel() { return id; }

		/// Returns the name of the Window.
		inline char * getName() { return state.name; }
};
//}}}
#endif

/*
 * change log
 *
 * $Log$
 */
