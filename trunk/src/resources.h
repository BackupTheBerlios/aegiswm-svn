/* Resources.h

   Copyright © 2003 David Reveman.

   This file is part of Aegis.

   Aegis is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Aegis is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with Aegis; see the file COPYING. If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#ifndef __Resources_hh
#define __Resources_hh

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H
}

#define DEFAULTRCFILE     AEGIS_PATH "/aegisrc"
#define DEFAULTSCRIPTDIR  AEGIS_PATH "/scripts"
#define DEFAULTACTIONFILE AEGIS_PATH \
	"/actions/sloppyfocusopaque/action.conf"
#define DEFAULTMENUFILE   AEGIS_PATH \
	"/menus/menu.conf"
#define DEFAULTSTYLEFILE  AEGIS_PATH \
	"/styles/freedesktop/style.conf"

class ResourceHandler;

#include "aegis.h"
#include "aegis_regex.h"
#include "style.h"

class ResourceHandler {
	public:
		ResourceHandler(Aegis *, char **);

		void loadConfig(Aegis *);
		void loadConfig(WaScreen *);

		XrmDatabase database;
		char **options;

	private:
		Aegis *aegis;
		Display *display;
};

#endif // __Resources_hh
