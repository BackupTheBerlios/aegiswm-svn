/* file name  : base_action.h
 * authors    : Michael Brailsford
 * created    : Sun Oct 31 23:18:24 CST 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef BASEACTION_H
#define BASEACTION_H

#include <X11/X.h>
#include <X11/Xlib.h>

//forward declaration
class Aegis;

/// BaseActions are the backbone of AegisWM.  They allow the flexiblilty to perform any sort of
/// action, from executing scripts to system commands to C/C++ code.
class BaseAction {
	protected:
		/// This is the display on which the window manager is running.
		Display * dpy;
		/// This is the Aegis object that gives us access to rest of the WM if needed.
		Aegis * wm;

	public:
		/// This actually performs the action.
		/// @param ev The XEvent the precipitated this Action.
		virtual void run(XEvent * ev) = 0;
};
#endif
