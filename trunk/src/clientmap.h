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

		/// Change the behavior of std::map<Window, Client *>::operator[](Window) to add a NULL to
		/// the map if a key is passed in that does not already exist in the map, otherwise, it
		/// returns a pointer to the Client object that the key maps to.
		virtual Client *& operator[](Window key);
};
#endif

/*
 * change log
 *
 * $Log$
 */

