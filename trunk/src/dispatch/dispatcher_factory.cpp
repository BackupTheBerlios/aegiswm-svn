/* file name  : dispatcherfactory.cpp
 * author     : Michael Brailsford
 * created    : Sun Jun 12 20:20:40 CDT 2005
 * copyright  : 2005 Michael Brailsford
 * version    : $Revision$
 */

#include "dispatcher_factory.h"
#include "event_dispatcher.h"
////** BEGIN GENERATED INCLUDES **////

////**  END GENERATED INCLUDES  **////

//{{{
DispatcherFactory::DispatcherFactory() {
}
//}}}
//{{{
DispatcherFactory::~DispatcherFactory() {
}
//}}}
//{{{
EventDispatcher * DispatcherFactory::build_dispatcher(ev_t event_type) {
	EventDispatcher * rv;
	switch(event_type) {
		////** BEGIN GENERATED FACTORY CODE **////
		////**  END GENERATED FACTORY CODE  **////
		default:
			rv = new EventDispatcher(event_type);
			break;
	}

	return rv;
}
//}}}

/*
 * change log
 *
 * $Log$
 */


