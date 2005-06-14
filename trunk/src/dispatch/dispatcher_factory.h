/* file name  : dispatcher_factory.h
 * authors    : Michael Brailsford
 * created    : Thu Jun 09 23:20:53 CDT 2005
 * copyright  : (c) 2005 Michael Brailsford
 * version    : $Revision$
 */

#ifndef DISPATCHER_FACTORY_H
#define DISPATCHER_FACTORY_H

#include "../aegis.h"

class EventDispatcher;
////** BEGIN GENERATED CODE **////

////**  END GENERATED CODE  **////

/// This builds objects of EventDispatcher and its child classes while hiding the details from the
/// user.
class DispatcherFactory {
	private:
		//nothing
	protected:
		//nothing
	public:
		/// Create an instance of the factory.
		DispatcherFactory();
		/// Destroy an instance of the factory.
		~DispatcherFactory();

		/// Creates an instance of an EventDispatcher child class based on the event type passed in.
		EventDispatcher * build_dispatcher(ev_t event_type);
};
#endif

/*
 * change log
 *
 * $Log$
 */

