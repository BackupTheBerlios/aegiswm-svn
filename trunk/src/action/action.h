/* file name  : action.h
 * authors    : Michael Brailsford
 * created    : Sun Oct 31 23:28:22 CST 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef ACTION_H
#define ACTION_H

#include "base_action.h"

class Action {
	protected:
	public:
		Action();
		virtual ~Action();

		/// Do it!
		virtual void run(XEvent * ev);
};
#endif
