/* file name  : clientmap.cpp
 * author     : Michael Brailsford
 * created    : Sun Oct 31 09:24:40 CST 2004
 * copyright  : 2004 Michael Brailsford
 * version    : $Revision$
 */

#include "clientmap.h"
#include "client.h"

using std::map;

//{{{
ClientMap::~ClientMap() {
	iterator iter, end = this->end();
	for(iter = begin(); iter != end; iter++) {
		delete (*iter).second;
		(*iter).second = NULL;
	}

	clear();
}
//}}}
//{{{
Client *& ClientMap::operator[](Window key) {
	if(this->find(key) == this->end())
		return map<Window, Client *>::operator[](key) = NULL;
	else
		return map<Window, Client *>::operator[](key);
}
//}}}


/*
 * change log
 *
 * $Log$
 */


