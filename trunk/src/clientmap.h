/* file name  : clientmap.h
 * authors    : Michael Brailsford
 * created    : Sun Oct 31 09:23:25 CST 2004
 * copyright  : (c) 2004 Michael Brailsford
 * version    : $Revision$
 */

#ifndef CLIENTMAP_H
#define CLIENTMAP_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <map>

//forward declaration
class Client;

/// This maps from Window ID to Client object.
class ClientMap : public std::map<Window, Client *> {
	public:
		/// Destroys the ClientMap and all its resources.
		virtual ~ClientMap();

		/// This works just like ClientMap::operator[] except that is does not insert anything into
		/// the map if the key does not exist.
		virtual Client * getClient(Window key);

		/// Returns true if the key exists in the map.
		virtual bool exists(Window key);

		void print();
};
#endif

/*
 * change log
 *
 * $Log$
 */

