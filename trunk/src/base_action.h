/* file name  : base_action.h
 * authors    : Michael Brailsford
 * created    : Sun Oct 31 23:18:24 CST 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef BASEACTION_H
#define BASEACTION_H

#include <X11/X.h>

//forward declaration
class Aegis;

class BaseAction {
	protected:
		/// This is the display on which the window manager is running.
		Display * dpy;
		/// This is the Aegis object that gives us access to rest of the WM if needed.
		Aegis * wm;

	public:
		virtual int run(XEvent * ev) = 0;
};
#endif
