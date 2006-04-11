/* file name  : window_mgmt_builtins.h
 * authors    : Michael Brailsford
 * created    : Tue Apr 11 00:21:28 CDT 2006
 * copyright  : (c) 2006 Michael Brailsford
 */

/// This file contains several Builtin Actions which implement window
/// management features of Aegis.  There are several classes defined in this
/// file.

#ifndef WINDOW_MGMT_BUILTINS_H
#define WINDOW_MGMT_BUILTINS_H

#include "builtinaction.h"
#include "../aegis.h"

class ExposeEventAction : public BuiltinAction {
    private:
        //none
    protected:
        //none
    public:
        ExposeEventAction(ev_t triggering_event, Aegis * wm);
        virtual ~ExposeEventAction();

        virtual void execute(XEvent * xev);
};

class MapRequestAction : public BuiltinAction {
    private:
        //none
    protected:
        //none
    public:
        MapRequestAction(ev_t triggering_event, Aegis * wm);
        virtual ~MapRequestAction();

        virtual void execute(XEvent * xev);
};

//////////////////// Generated classes go here ////////////////////

#endif

