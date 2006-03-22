/* file name  : test_action.h
 * authors    : Michael Brailsford
 * created    : Tue Mar 21 22:54:13 CST 2006
 * copyright  : (c) 2006 Michael Brailsford
 */

#ifndef TEST_ACTION_H
#define TEST_ACTION_H

#include "action.h"

class TestAction : Action {
	private:
	protected:
	public:
		TestAction(ev_t triggering_event);
		virtual ~TestAction() {}

        virtual void execute(XEvent * xev);
};
#endif

