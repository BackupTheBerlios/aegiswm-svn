/* Aegis.h

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

#ifndef __Aegis_hh
#define __Aegis_hh

extern "C" {
#include <X11/Xlib.h>

#ifdef    HAVE_SIGNAL_H
#  include <signal.h>
#endif // HAVE_SIGNAL_H

#ifdef    HAVE_ICONV
#  include <iconv.h>
#endif // HAVE_ICONV

#ifdef    THREAD
#  define _MULTI_THREADED
#  ifdef    HAVE_PTHREAD_H
#    include <pthread.h>
#  endif // HAVE_PTHREAD_H
#endif // THREAD

}

#ifdef    HAVE_ICONV
#  define INTERNALCODE "UTF-8"
#endif // HAVE_ICONV

class Aegis;

extern char *aegis_pathenv;

#define FOCUS_HISTORY_SIZE 64

enum {
	ARG_DISPLAY = 0,
	ARG_RCFILE,
	ARG_SCREENMASK,
	ARG_SCRIPTDIR,
	ARG_DOUBLECLICK,
	ARG_CLIENTSIDE,

#ifdef    THREAD
	ARG_THREADS,
	ARG_THREADPRIO,
#endif // THREAD

	ARG_ACTIONFILE,
	ARG_STYLEFILE,
	ARG_MENUFILE,
	ARG_NRDESKTOPS,
	ARG_DESKTOPNAMES,
	ARG_VIRTUALSIZE,
	ARG_MENUSTACKING,
	ARG_DOCKAPPHOLDERSTACKING,
	ARG_TRANSIENTABOVE,
	ARG_FOCUSREVERTTOWINDOW,
	ARG_EXTERNALBG,
	ARG_INFOCOMMAND,
	ARG_WARNINGCOMMAND
};

struct ModifierMap {
	char *name;
	long int modifier;
};

#include "util.h"
#include "screen.h"
#include "timer.h"
#include "net.h"
#include "style.h"
#include "event.h"
#include "cursor.h"

class Aegis {
	public:
		Aegis(char **, char **);
		~Aegis(void);

		WindowObject *findWin(Window, long int);
		void getModifierMappings(void);

		char **options;
		Display *display;
		ResourceHandler *rh;
		EventHandler *eh;
		NetHandler *net;
		Timer *timer;
		AegisCursor *cursor;
		unsigned long double_click, screenmask;
		bool wmerr, running;
		bool quit_signal, restart_signal, unknown_signal;

		map<Window, WindowObject *> window_table;
		list<AegisScreen *> wascreen_list;
		int min_key, max_key;
		list<ModifierMap *> modmaps;

		char config_info[256];

#ifdef    THREAD
		int render_thread_prio;
#endif // THREAD

#ifdef    HAVE_ICONV
		iconv_t utf8conv;
#endif // HAVE_ICONV

#ifdef    SHAPE
		int shape, shape_event;
#endif // SHAPE

#ifdef    XINERAMA
		int xinerama;
		XineramaScreenInfo *xinerama_info;
		int xinerama_info_num;
#endif // XINERAMA

#ifdef    RANDR
		int randr, randr_event;
#endif // RANDR

		bool client_side_rendering;

		bool focusNew(Window, bool = false);
		void addToFocusHistory(Window);
		void removeFromFocusHistory(Window);
		void focusRevertFrom(AegisScreen *, Window);

		Window prefocus;
		list<Window> focus_history;
};

void ae_grab_server(void);
void ae_ungrab_server(void);
bool validate_drawable(Drawable, unsigned int * = NULL, unsigned int * = NULL);
const bool validate_window_mapped(Window);
int xerrorhandler(Display *, XErrorEvent *);
int wmrunningerror(Display *, XErrorEvent *);
void signalhandler(int);
void restart(char *);
void quit(int);
char *expand(char *, AegisWindow *, MenuItem *, const char * = NULL,
		char * = NULL, MenuItem * = NULL);
char *preexpand(char *, bool *);
char *ae_locale_to_utf8(const char *);

extern list<DWindowObject *> __render_list;

extern Tst<char *> _aegis_static_variables;

#ifdef    THREAD
extern unsigned int __render_thread_count;

extern pthread_mutex_t __render_mutex;
extern pthread_cond_t  __render_cond;
extern unsigned int    __render_count;

extern pthread_mutex_t __render_list_mutex;
extern pthread_cond_t  __render_list_cond;

#  define RENDER_LOCK { \
	pthread_mutex_lock(&__render_mutex); \
}
#  define RENDER_RELEASE { \
	pthread_mutex_unlock(&__render_mutex); \
}
#  define RENDER_GET { \
	pthread_mutex_lock(&__render_mutex); \
	while (__render_count) \
	pthread_cond_wait(&__render_cond, &__render_mutex); \
}
#  define RENDER_GET_ONE { \
	pthread_mutex_lock(&__render_mutex); \
	while (__render_count > 1) \
	pthread_cond_wait(&__render_cond, &__render_mutex); \
}
#  define RENDER_BROADCAST { \
	pthread_mutex_unlock(&__render_mutex); \
	pthread_cond_broadcast(&__render_cond); \
}

#  define RENDER_LIST_LOCK { \
	pthread_mutex_lock(&__render_list_mutex); \
}
#  define RENDER_LIST_RELEASE { \
	pthread_mutex_unlock(&__render_list_mutex); \
}
#  define RENDER_LIST_GET { \
	pthread_mutex_lock(&__render_list_mutex); \
	while (__render_list.empty()) \
	pthread_cond_wait(&__render_list_cond, &__render_list_mutex); \
}
#  define RENDER_LIST_SIGNAL { \
	pthread_mutex_unlock(&__render_list_mutex); \
	pthread_cond_signal(&__render_list_cond); \
}

void *render_thread_func(void *);
#else  // !THREAD
#  define RENDER_LOCK
#  define RENDER_RELEASE
#  define RENDER_GET
#  define RENDER_GET_ONE
#  define RENDER_BROADCAST
#  define RENDER_LIST_LOCK
#  define RENDER_LIST_RELEASE
#  define RENDER_LIST_GET
#  define RENDER_LIST_SIGNAL
#endif // THREAD

#endif // __Aegis_hh
