/* file name  : clientmap.cpp
 * author     : Michael Brailsford
 * created    : Sun Oct 31 09:24:40 CST 2004
 * copyright  : 2004 Michael Brailsford
 * version    : $Revision$
 */

#include "clientmap.h"
#include "client.h"
#include "aegis.h"

using std::map;
using std::pair;

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
Client * ClientMap::getClient(Window key) {
	Client * rv = NULL;
	if(this->find(key) != this->end())
		rv = map<Window, Client *>::operator[](key);

	return rv;
}
//}}}

//{{{
void ClientMap::print() {
	ClientMap::iterator iter, end = this->end();
	log_info("ClientMap = {");
	for(iter = this->begin(); iter != end; iter++) {
		pair<Window, Client *> par = (*iter);
		log_info("\t %i => 0x%X,", (int)par.first, (int)par.second);
	}
	log_info("}");
}
//}}}


/*
 * change log
 *
 * $Log$
 */


