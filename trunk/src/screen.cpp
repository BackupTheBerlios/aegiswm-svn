/* Screen.cc

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

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>

#ifdef    RANDR
#  include <X11/extensions/Xrandr.h>
#endif // RANDR

#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#ifdef    HAVE_UNISTD_H
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif // HAVE_UNISTD_H

#ifdef    STDC_HEADERS
#  include <stdlib.h>
#endif // STDC_HEADERS

#ifdef    HAVE_SIGNAL_H
#  include <signal.h>
#endif // HAVE_SIGNAL_H

#ifdef    HAVE_ERRNO_H
#  include <errno.h>
#endif // HAVE_ERRNO_H

}

#include "screen.h"
#include "style.h"
#include "window.h"

AegisScreen::AegisScreen(Display *d, int scrn_number, Aegis *aegis) :
	RootWindowObject(NULL, 0, RootType, NULL, "root") {
		Window ro, pa, *children;
		int eventmask, dummy;
		unsigned int nchild, i;
		XSetWindowAttributes attrib_set;

		ws = this;
		display = d;
		screen_number = scrn_number;
		id = RootWindow(display, screen_number);
		visual = DefaultVisual(display, screen_number);
		colormap = DefaultColormap(display, screen_number);

		XGetGeometry(display, id, &ro, &dummy, &dummy, &width, &height,
				&i, &screen_depth);

		vdpi = ((double) height * 25.4) /
			(double) DisplayHeightMM(display, screen_number);
		hdpi = ((double) width * 25.4) /
			(double) DisplayWidthMM(display, screen_number);

		aegis = aegis;
		net = aegis->net;
		rh = aegis->rh;
		focused = true;
		shutdown = dont_propagate_cfg_update = false;
		bg_surface = NULL;

		eventmask = StructureNotifyMask | SubstructureRedirectMask |
			PropertyChangeMask | ColormapChangeMask | KeyPressMask |
			KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
			EnterWindowMask | LeaveWindowMask | FocusChangeMask |
			StructureNotifyMask;

		i = snprintf(displaystring, 256, "DISPLAY=%s", DisplayString(display));
		if (i < 256) snprintf(displaystring + i - 1, 256 - i, "%d", screen_number);

		XSetErrorHandler((XErrorHandler) wmrunningerror);
		XSelectInput(display, id, eventmask);
		XSync(display, false);
		XSetErrorHandler((XErrorHandler) xerrorhandler);
		if (aegis->wmerr) {
			cerr << "aegis: warning: another window manager is running on " <<
				displaystring + 8 << endl;
			aegis->wmerr = true;
			return;
		}

		v_x = v_y = 0;

#ifdef    RANDR
		if (aegis->randr) XRRSelectInput(display, id, RRScreenChangeNotifyMask);
#endif // RANDR

		rh->loadConfig(this);

		/* NYI: support ARGB visuals
		   Visual *argb_visual = findARGBVisual();
		   if (argb_visual) {
		   visual = argb_visual;
		   screen_depth = 32;
		   colormap = XCreateColormap (display, id, argb_visual, AllocNone);
		   }
		   */

		XGCValues gcv;
		gcv.foreground = WhitePixel(display, screen_number);
		white_gc = XCreateGC(display, id, GCForeground, &gcv);
		gcv.foreground = BlackPixel(display, screen_number);
		black_gc = XCreateGC(display, id, GCForeground, &gcv);
		gcv.foreground = WhitePixel(display, screen_number)
			^ BlackPixel(display, screen_number);
		gcv.function = GXxor;
		gcv.subwindow_mode = IncludeInferiors;
		xor_gc = XCreateGC(display, id,
				GCForeground | GCFunction | GCSubwindowMode, &gcv);

		current_desktop = new Desktop(0, width, height);
		desktop_list.push_back(current_desktop);

		for (i = 1; i < config.desktops; i++)
			desktop_list.push_back(new Desktop(i, width, height));

		net->setWorkarea(this);

		aegis->window_table.insert(make_pair(id, this));

		attrib_set.override_redirect = true;
		wm_check = XCreateWindow(display, id, 0, 0, 1, 1, 0,
				CopyFromParent, InputOnly, CopyFromParent,
				CWOverrideRedirect, &attrib_set);

		windowlist_menu = NULL;

		v_xmax = (config.virtual_x - 1) * width;
		v_ymax = (config.virtual_y - 1) * height;

		readActionLists();
		readStyles();

		west = new ScreenEdge(this, "west", 0, 0, 2, height);
		east = new ScreenEdge(this, "east", width - 2, 0, 2, height);
		north = new ScreenEdge(this, "north", 0, 0, width, 2);
		south = new ScreenEdge(this, "south", 0, height - 2, width, 2);

		resetActionList(new AegisStringMap(WindowIDName, "root"));

		net->setDesktopGeometry(this);
		net->setNumberOfDesktops(this);
		net->getCurrentDesktop(this);
		net->setCurrentDesktop(this);
		net->getDesktopViewPort(this);
		net->setDesktopViewPort(this);

		net->getXRootPMapId(this);

		resetStyle();
		pushRenderEvent();
		readMenus();

		XWindowAttributes attr;
		XQueryTree(display, id, &ro, &pa, &children, &nchild);
		for (i = 0; i < nchild; ++i) {
			XWMHints *wm_hints = NULL;
			int status = 0;
			if (children[i] == None) continue;
			ae_grab_server();
			if (validate_drawable(children[i])) {
				status = XGetWindowAttributes(display, children[i], &attr);
				wm_hints = XGetWMHints(display, children[i]);
			} else {
				ae_ungrab_server();
				continue;
			}
			ae_ungrab_server();

			if (wm_hints) {
				if ((wm_hints->flags & IconWindowHint) &&
						(wm_hints->icon_window != children[i])) {
					for (unsigned int j = 0; j < nchild; j++) {
						if (children[j] == wm_hints->icon_window) {
							children[j] = None;
							break;
						}
					}
				}
				XFree(wm_hints);
			}
		}

		for (i = 0; i < nchild; ++i) {
			XWMHints *wm_hints = NULL;
			int state = -1;
			int status = 0;
			if (children[i] == None) continue;
			ae_grab_server();
			if (validate_drawable(children[i])) {
				status = XGetWindowAttributes(display, children[i], &attr);
				wm_hints = XGetWMHints(display, children[i]);
			} else {
				ae_ungrab_server();
				continue;
			}
			ae_ungrab_server();

			if (wm_hints) {
				if (wm_hints->flags & StateHint)
					state = wm_hints->initial_state;
				XFree(wm_hints);
			}

			if (status && attr.screen && (! attr.override_redirect) &&
					(attr.map_state != IsUnmapped)) {
				if (net->isSystrayWindow(children[i])) {
					if (! (aegis->findWin(children[i], SystrayType))) {
						XSelectInput(display, children[i], StructureNotifyMask);
						SystrayWindow *stw = new SystrayWindow(children[i], this);
						aegis->window_table.insert(make_pair(children[i], stw));
						systray_window_list.push_back(children[i]);
						net->setSystrayWindows(this);
					}
				} else {
					if (state == WithdrawnState) {
						EventDetail ed;
						ed.x = ed.y = ed.detail = ed.x11mod = ed.wamod = 0;
						ed.type = DockappAddRequest;
						Dockapp *d = new Dockapp(this, children[i]);
						aegis->eh->evAct(NULL, d->id, &ed);
						if (! d->dh) addDockapp(d, NULL);
					} else if ((aegis->window_table.find(children[i])) ==
							aegis->window_table.end()) {
						AegisWindow *newwin = new AegisWindow(children[i], this);
						if ((newwin = (AegisWindow *)
									aegis->findWin(children[i], WindowType))) {
							net->setState(newwin, newwin->state);
							net->getMergedState(newwin);
							list<MReq *>::iterator it = mreqs.begin();
							while (it != mreqs.end())
								if ((*it)->mid == children[i]) {
									newwin->merge((*it)->win, (*it)->type);
									it = mreqs.erase(it);
								} else
									it++;
						}
					}
				}
			}
		}
		XFree(children);
		LISTDEL(mreqs);
		net->getClientListStacking(this);
		ws->restackWindows();
		net->setClientList(this);
		net->getActiveWindow(this);

		net->setSupportedWMCheck(this, wm_check);
		net->setSupported(this);
	}

AegisScreen::~AegisScreen(void) {
	AegisFrameWindow *wf;
	shutdown = true;
	XSelectInput(display, id, NoEventMask);
	net->deleteSupported(this);
	XDestroyWindow(display, wm_check);

	LISTDELITEMS(docks);

	AegisWindow **delstack = new AegisWindow*[wawindow_list.size()];
	int stackp = 0;

	list<Window>::reverse_iterator it = aab_stacking_list.rbegin();
	for (; it != aab_stacking_list.rend(); ++it) {
		wf = (AegisFrameWindow *) aegis->findWin(*it, WindowFrameType);
		if (wf) delstack[stackp++] = wf->aegis;
	}
	it = stacking_list.rbegin();
	for (; it != stacking_list.rend(); ++it) {
		wf = (AegisFrameWindow *) aegis->findWin(*it, WindowFrameType);
		if (wf) delstack[stackp++] = wf->aegis;
	}
	it = aot_stacking_list.rbegin();
	for (; it != aot_stacking_list.rend(); ++it) {
		wf = (AegisFrameWindow *) aegis->findWin(*it, WindowFrameType);
		if (wf) delstack[stackp++] = wf->aegis;
	}

	for (int i = 0; i < stackp; i++)
		delete delstack[i];

	delete [] delstack;

	LISTCLEAR(wawindow_list);
	LISTCLEAR(wawindow_list_map_order);

	LISTDEL(strut_list);

	delete west;
	delete east;
	delete north;
	delete south;

	net->setXRootPMapId(this, None);

	aegis->window_table.erase(id);
}

Visual *AegisScreen::findARGBVisual(void) {
	XVisualInfo *xvi;
	XVisualInfo templ;
	int nvi;
	int i;
	XRenderPictFormat *format;
	Visual *visual;

	templ.screen = screen_number;
	templ.depth = 32;
	xvi = XGetVisualInfo (display,
			VisualScreenMask |
			VisualDepthMask,
			&templ,
			&nvi);
	if (!xvi)
		return 0;
	visual = 0;
	for (i = 0; i < nvi; i++) {
		format = XRenderFindVisualFormat (display, xvi[i].visual);
		if (format->type == PictTypeDirect && format->direct.alphaMask) {
			visual = xvi[i].visual;
			break;
		}
	}

	XFree (xvi);

	return visual;
}


void AegisScreen::propagateActionlistUpdate(ActionList *actionlist) {
	if (dont_propagate_cfg_update) return;

	map<Window, WindowObject *>::iterator it = aegis->window_table.begin();
	for (; it != aegis->window_table.end(); it++) {
		if (((*it).second)->ws == this &&
				((*it).second)->type == WindowType) {
			AegisWindow *aegis = (AegisWindow *) (*it).second;
			ActionList *al = aegis->actionlists[
				STATE_FROM_MASK_AND_LIST(window_state_mask, aegis->actionlists)];
			if (al == actionlist)
				aegis->updateGrabs();
		}
	}
}

void AegisScreen::propagateStyleUpdate(Style *style) {
	if (dont_propagate_cfg_update) return;

	map<Window, WindowObject *>::iterator it = aegis->window_table.begin();
	for (; it != aegis->window_table.end(); it++) {
		if (((*it).second)->ws == this &&
				((*it).second)->type & ANY_DECOR_WINDOW_TYPE &&
				((DWindowObject *) (*it).second)->style == style)
			((DWindowObject *) (*it).second)->styleUpdate(true, true);
	}
}

void AegisScreen::forceRenderOfWindows(int typemask) {
	map<Window, WindowObject *>::iterator it = aegis->window_table.begin();
	for (; it != aegis->window_table.end(); it++) {
		if (((*it).second)->ws == this &&
				((*it).second)->type & ANY_DECOR_WINDOW_TYPE &&
				((*it).second)->type & typemask) {
			((DWindowObject *) (*it).second)->force_texture = true;
			((DWindowObject *) (*it).second)->pushRenderEvent();
		}
	}
}

void AegisScreen::clearAllCacheAndRedraw(void) {
	Tst<RenderGroup *>::iterator it = rendergroups.begin();
	for (; it != rendergroups.end(); it++) {
		(*it)->clearCache();
		if ((*it)->is_a_style) {
			((Style *) *it)->serial++;
			propagateStyleUpdate((Style *) *it);
		}
	}
}

void AegisScreen::reload(void) {

	RENDER_GET;

	ws->aegis->prefocus = None;
	XSetInputFocus(display, None, RevertToNone, CurrentTime);

	Tst<char *>::iterator cit = constants.begin();
	for (; cit != constants.end(); cit++)
		delete [] *cit;
	constants.clean();

	dont_propagate_cfg_update = true;
	readActionLists();
	readStyles();
	readMenus();
	dont_propagate_cfg_update = false;

	map<Window, WindowObject *>::iterator it = aegis->window_table.begin();
	for (; it != aegis->window_table.end(); it++) {
		if (((*it).second)->ws == this &&
				((*it).second)->type & ANY_ACTION_WINDOW_TYPE)
			((AWindowObject *) (*it).second)->resetActionList();
	}

	resetStyle();
	commonStyleUpdate();
	pushRenderEvent();

	list<DockappHandler *>::iterator dock_it = docks.begin();
	for (; dock_it != docks.end(); dock_it++) {
		(*dock_it)->resetStyle();
		(*dock_it)->commonStyleUpdate();
		(*dock_it)->update();
	}
	list<AegisWindow *>::iterator win_it = wawindow_list.begin();
	for (; win_it != wawindow_list.end(); win_it++) {
		(*win_it)->frame->resetStyle();
		(*win_it)->frame->commonStyleUpdate();
		(*win_it)->windowStateCheck(true);
		(*win_it)->updateAllAttributes();
	}

	aegis->focusRevertFrom(this, None);

	RENDER_RELEASE;

}

void AegisScreen::raiseTransientWindows(Window win, list<Window> *stacklist) {
	AegisFrameWindow *wf = (AegisFrameWindow *)
		aegis->findWin(win, WindowFrameType);
	if (wf) {
		AegisWindow *ww = wf->aegis;
		if (! ww->transients.empty()) {
			list<Window>::iterator tit = ww->transients.begin();
			for (; tit != ww->transients.end();) {
				AegisWindow *wt = (AegisWindow *)
					aegis->findWin(*tit, WindowType);
				if (wt) {
					stacking_list.remove(wt->frame->id);
					aab_stacking_list.remove(wt->frame->id);
					aot_stacking_list.remove(wt->frame->id);
					stacklist->push_front(wt->frame->id);
					tit++;
				} else
					tit = ww->transients.erase(tit);
			}
		}
	}
}

void AegisScreen::raiseWindow(Window win, bool restack) {
	list<Window>::iterator it = aot_stacking_list.begin();
	for (; it != aot_stacking_list.end(); ++it) {
		if (*it == win) {
			aot_stacking_list.erase(it);
			aot_stacking_list.push_front(win);
			raiseTransientWindows(*it, &aot_stacking_list);
			if (restack) restackWindows();
			return;
		}
	}
	it = stacking_list.begin();
	for (; it != stacking_list.end(); ++it) {
		if (*it == win) {
			stacking_list.erase(it);
			stacking_list.push_front(win);
			raiseTransientWindows(*it, &stacking_list);
			if (restack) restackWindows();
			return;
		}
	}
	it = aab_stacking_list.begin();
	for (; it != aab_stacking_list.end(); ++it) {
		if (*it == win) {
			aab_stacking_list.erase(it);
			aab_stacking_list.push_front(win);
			raiseTransientWindows(*it, &aab_stacking_list);
			if (restack) restackWindows();
			return;
		}
	}
}

void AegisScreen::lowerWindow(Window win, bool restack) {
	list<Window>::iterator it = aot_stacking_list.begin();
	for (; it != aot_stacking_list.end(); ++it) {
		if (*it == win) {
			aot_stacking_list.erase(it);
			aot_stacking_list.push_back(win);
			if (restack) restackWindows();
			return;
		}
	}
	it = stacking_list.begin();
	for (; it != stacking_list.end(); ++it) {
		if (*it == win) {
			stacking_list.erase(it);
			stacking_list.push_back(win);
			if (restack) restackWindows();
			return;
		}
	}
	it = aab_stacking_list.begin();
	for (; it != aab_stacking_list.end(); ++it) {
		if (*it == win) {
			aab_stacking_list.erase(it);
			aab_stacking_list.push_back(win);
			if (restack) restackWindows();
			return;
		}
	}
}

void AegisScreen::restackWindows(void) {
	int i = 0;
	Window *stack = new Window[4 + aot_stacking_list.size() +
		stacking_list.size() +
		aab_stacking_list.size()];

	stack[i++] = west->id;
	stack[i++] = east->id;
	stack[i++] = north->id;
	stack[i++] = south->id;

	list<Window>::iterator it = aot_stacking_list.begin();
	for (; it != aot_stacking_list.end(); ++it) stack[i++] = *it;
	it = stacking_list.begin();
	for (; it != stacking_list.end(); ++it) stack[i++] = *it;
	it = aab_stacking_list.begin();
	for (; it != aab_stacking_list.end(); ++it) stack[i++] = *it;

	XRestackWindows(display, stack, i);

	delete [] stack;
}

ActionList *AegisScreen::getActionListNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<ActionList *>::iterator it = actionlists.find(name);
	if (it != actionlists.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown actionlist=%s", name);

	return NULL;
}

Style *AegisScreen::getStyleNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<Style *>::iterator it = styles.find(name);
	if (it != styles.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown style=%s", name);

	return NULL;
}

RenderGroup *AegisScreen::getRenderGroupNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<RenderGroup *>::iterator it = rendergroups.find(name);
	if (it != rendergroups.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown group=%s", name);

	return NULL;
}

RenderOpPath *AegisScreen::getPathNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<RenderOpPath *>::iterator it = paths.find(name);
	if (it != paths.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown path=%s", name);

	return NULL;
}

RenderPattern *AegisScreen::getPatternNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<RenderPattern *>::iterator it = patterns.find(name);
	if (it != patterns.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown pattern=%s", name);

	return NULL;
}

RenderOpText *AegisScreen::getTextNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<RenderOpText *>::iterator it = texts.find(name);
	if (it != texts.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown text-object=%s", name);

	return NULL;
}

Menu *AegisScreen::getMenuNamed(char *name, bool warn) {
	if (! name) return NULL;

	Tst<Menu *>::iterator it = menus.find(name);
	if (it != menus.end()) return *it;

	if (warn)
		showAegisrningMessage(__FUNCTION__, "unknown menu=%s", name);

	return NULL;
}

void AegisScreen::updateWorkarea(void) {
	int old_x = current_desktop->workarea.x,
		old_y = current_desktop->workarea.y;
	unsigned int old_width = current_desktop->workarea.width,
				 old_height = current_desktop->workarea.height;

	current_desktop->workarea.x = current_desktop->workarea.y = 0;
	current_desktop->workarea.width = width;
	current_desktop->workarea.height = height;

	list<WMstrut *>::iterator it = strut_list.begin();
	for (; it != strut_list.end(); ++it) {
		WindowObject *wo = aegis->findWin((*it)->window,
				WindowType | DockHandlerType);
		if (wo) {
			if (wo->type == WindowType) {
				if (! (((AegisWindow *) wo)->desktop_mask &
							(1L << current_desktop->number)))
					continue;
			} else if (wo->type == DockHandlerType) {
				if (! (((DockappHandler *) wo)->desktop_mask &
							(1L << current_desktop->number)))
					continue;
			}
		} else
			continue;

		if ((signed int) (*it)->left > current_desktop->workarea.x)
			current_desktop->workarea.x = (*it)->left;
		if ((signed int) (*it)->top > current_desktop->workarea.y)
			current_desktop->workarea.y = (*it)->top;
		if ((width - (*it)->right) < current_desktop->workarea.width)
			current_desktop->workarea.width = width - (*it)->right;
		if ((height - (*it)->bottom) < current_desktop->workarea.height)
			current_desktop->workarea.height = height - (*it)->bottom;
	}
	current_desktop->workarea.width = current_desktop->workarea.width -
		current_desktop->workarea.x;
	current_desktop->workarea.height = current_desktop->workarea.height -
		current_desktop->workarea.y;

	int res_x, res_y, res_w, res_h;
	if (old_x != current_desktop->workarea.x ||
			old_y != current_desktop->workarea.y ||
			old_width != current_desktop->workarea.width ||
			old_height != current_desktop->workarea.height) {
		net->setWorkarea(this);

		list<AegisWindow *>::iterator ae_it = wawindow_list.begin();
		for (; ae_it != wawindow_list.end(); ++ae_it) {
			if (! ((*ae_it)->desktop_mask &
						(1L << current_desktop->number))) break;
			if ((*ae_it)->wstate & StateMaximizedMask) {
				(*ae_it)->wstate &= ~StateMaximizedMask;
				res_x = (*ae_it)->restore_max.x;
				res_y = (*ae_it)->restore_max.y;
				res_w = (*ae_it)->restore_max.width;
				res_h = (*ae_it)->restore_max.height;
				(*ae_it)->_maximize((*ae_it)->restore_max.misc0,
									(*ae_it)->restore_max.misc1);
				(*ae_it)->restore_max.x = res_x;
				(*ae_it)->restore_max.y = res_y;
				(*ae_it)->restore_max.width = res_w;
				(*ae_it)->restore_max.height = res_h;
			}
		}
	}
}

void AegisScreen::getWorkareaSize(int *x, int *y,
		unsigned int *w, unsigned int *h) {
	*x = current_desktop->workarea.x;
	*y = current_desktop->workarea.y;
	*w = current_desktop->workarea.width;
	*h = current_desktop->workarea.height;

#ifdef    XINERAMA
	Window win;
	int px, py, i;
	unsigned int ui;

	if (aegis->xinerama && aegis->xinerama_info) {
		XQueryPointer(display, id, &win, &win, &px, &py, &i, &i, &ui);
		for (i = 0; i < aegis->xinerama_info_num; ++i) {
			if (px > aegis->xinerama_info[i].x_org &&
					px < (aegis->xinerama_info[i].x_org +
						aegis->xinerama_info[i].width) &&
					py > aegis->xinerama_info[i].y_org &&
					py < (aegis->xinerama_info[i].y_org +
						aegis->xinerama_info[i].height)) {
				int diff = aegis->xinerama_info[i].x_org - *x;
				unsigned int xt = aegis->xinerama_info[i].width;
				if (diff > 0) {
					*w -= diff;
					*x = aegis->xinerama_info[i].x_org;
				} else xt += diff;
				if (*w > xt) *w = xt;

				diff = aegis->xinerama_info[i].y_org - *y;
				xt = aegis->xinerama_info[i].height;
				if (diff > 0) {
					*h -= diff;
					*y = aegis->xinerama_info[i].y_org;
				} else xt += diff;
				if (*h > xt) *h = xt;
				break;
			}
		}
	}
#endif // XINERAMA

}

void AegisScreen::moveViewportTo(int x, int y) {
	if ((int) v_x == x && (int) v_y == y) return;
	if (x > v_xmax) x = v_xmax;
	else if (x < 0) x = 0;
	if (y > v_ymax) y = v_ymax;
	else if (y < 0) y = 0;

	int x_move = - (x - v_x);
	int y_move = - (y - v_y);
	current_desktop->v_x = v_x = x;
	current_desktop->v_y = v_y = y;

	list<AegisWindow *>::iterator it = wawindow_list.begin();
	for (; it != wawindow_list.end(); ++it) {
		if (! ((*it)->wstate & StateStickyMask)) {
			int old_x = (*it)->attrib.x;
			int old_y = (*it)->attrib.y;
			(*it)->attrib.x = (*it)->attrib.x + x_move;
			(*it)->attrib.y = (*it)->attrib.y + y_move;

			if ((((*it)->attrib.x + (*it)->attrib.width) > 0 &&
						(*it)->attrib.x < (int) width) &&
					(((*it)->attrib.y + (*it)->attrib.height) > 0 &&
					 (*it)->attrib.y < (int) height))
				(*it)->redrawWindow(true);
			else {
				if (((old_x + (*it)->attrib.width) > 0 &&
							old_x < (int) width) &&
						((old_y + (*it)->attrib.height) > 0 &&
						 old_y < (int) height))
					(*it)->redrawWindow();
				else {
					(*it)->dontsend = true;
					(*it)->redrawWindow();
					(*it)->dontsend = false;
					net->setVirtualPos(*it);
				}
			}
		}
	}

	Tst<Menu *>::iterator it2 = menus.begin();
	for (; it2 != menus.end(); it2++)
		if ((*it2)->mapped && (! (*it2)->rootitem_id))
			(*it2)->move((*it2)->x + x_move, (*it2)->y + y_move);

	net->setDesktopViewPort(this);
}

void AegisScreen::moveViewport(int direction) {
	int vd;

	switch (direction) {
		case WestDirection:
			if (v_x > 0) {
				if (((int) v_x - (int) width) < 0) vd = v_x;
				else vd = width;
				XWarpPointer(display, None, None, 0, 0, 0, 0, vd - 6, 0);
				moveViewportTo(v_x - vd, v_y);
			}
			break;
		case EastDirection:
			if (v_x < v_xmax) {
				if ((v_x + (int) width) > v_xmax) vd = v_xmax - v_x;
				else vd = (int) width;
				XWarpPointer(display, None, None, 0, 0, 0, 0, 6 - vd, 0);
				moveViewportTo(v_x + vd, v_y);
			}
			break;
		case NorthDirection:
			if (v_y > 0) {
				if ((v_y - (int) height) < 0) vd = v_y;
				else vd = (int) height;
				XWarpPointer(display, None, None, 0, 0, 0, 0, 0, vd - 6);
				moveViewportTo(v_x, v_y - vd);
			}
			break;
		case SouthDirection:
			if (v_y < v_ymax) {
				if ((v_y + (int) height) > v_ymax) vd = v_ymax - v_y;
				else vd = (int) height;
				XWarpPointer(display, None, None, 0, 0, 0, 0, 0, 6 - vd);
				moveViewportTo(v_x, v_y + vd);
			}
	}
}

void AegisScreen::viewportFixedMove(char *s) {
	int x, y, mask;
	unsigned int w = 0, h = 0;

	if (! s) return;

	mask = XParseGeometry(s, &x, &y, &w, &h);
	if (mask & XNegative) x = v_xmax + x;
	if (mask & YNegative) y = v_ymax + y;
	moveViewportTo(x, y);
}

void AegisScreen::viewportRelativeMove(char *s) {
	int x, y, mask;
	unsigned int w = 0, h = 0;

	if (! s) return;

	mask = XParseGeometry(s, &x, &y, &w, &h);
	moveViewportTo(v_x + x, v_y + y);
}

void AegisScreen::startViewportMove(void) {
	XEvent event;
	int px, py, i;
	list<XEvent *> *maprequest_list;
	Window w;
	unsigned int ui;
	list<AegisWindow *>::iterator it;

	if (aegis->eh->move_resize != EndMoveResizeType) return;
	aegis->eh->move_resize = MoveOpaqueType;

	XQueryPointer(display, id, &w, &w, &px, &py, &i, &i, &ui);

	maprequest_list = new list<XEvent *>;
	XGrabPointer(display, id, true, ButtonReleaseMask | ButtonPressMask |
			PointerMotionMask | EnterWindowMask | LeaveWindowMask,
			GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	XGrabKeyboard(display, id, true, GrabModeAsync, GrabModeAsync,
			CurrentTime);
	for (;;) {
		it = wawindow_list.begin();
		for (; it != wawindow_list.end(); ++it) {
			(*it)->dontsend = true;
		}
		aegis->eh->eventLoop(&aegis->eh->menu_viewport_move_return_mask,
				&event);
		switch (event.type) {
			case MotionNotify: {
								   while (XCheckTypedWindowEvent(display, event.xmotion.window,
											   MotionNotify, &event));
								   int x = v_x - (event.xmotion.x_root - px);
								   int y = v_y - (event.xmotion.y_root - py);

								   if (x > v_xmax) x = v_xmax;
								   else if (x < 0) x = 0;
								   if (y > v_ymax) y = v_ymax;
								   else if (y < 0) y = 0;

								   int x_move = - (x - v_x);
								   int y_move = - (y - v_y);
								   current_desktop->v_x = v_x = x;
								   current_desktop->v_y = v_y = y;

								   list<AegisWindow *>::iterator it = wawindow_list.begin();
								   for (; it != wawindow_list.end(); ++it) {
									   if (! ((*it)->wstate & StateStickyMask)) {
										   int old_x = (*it)->attrib.x;
										   int old_y = (*it)->attrib.y;
										   (*it)->attrib.x = (*it)->attrib.x + x_move;
										   (*it)->attrib.y = (*it)->attrib.y + y_move;

										   if ((((*it)->frame->attrib.x +
														   (*it)->frame->attrib.width) > 0 &&
													   (*it)->frame->attrib.x < (int) width) &&
												   (((*it)->frame->attrib.y +
													 (*it)->frame->attrib.height) > 0 &&
													(*it)->frame->attrib.y < (int) height))
											   (*it)->redrawWindow();
										   else {
											   if (((old_x + (*it)->frame->attrib.width) > 0 &&
														   old_x < (int) width) &&
													   ((old_y + (*it)->frame->attrib.height) > 0 &&
														old_y < (int) height))
												   (*it)->redrawWindow();
										   }
									   }
								   }

								   Tst<Menu *>::iterator it2 = menus.begin();
								   for (; it2 != menus.end(); it2++)
									   if ((*it2)->mapped && (! (*it2)->rootitem_id))
										   (*it2)->move((*it2)->x + x_move, (*it2)->y + y_move);

								   px = event.xmotion.x_root;
								   py = event.xmotion.y_root;
							   } break;
			case LeaveNotify:
			case EnterNotify:
							   break;
			case MapRequest:
							   maprequest_list->push_front(&event); break;
			case ButtonPress:
			case ButtonRelease:
							   event.xbutton.window = id;
			case KeyPress:
			case KeyRelease:
							   if (event.type == KeyPress || event.type == KeyRelease)
								   event.xkey.window = id;
							   aegis->eh->handleEvent(&event);
							   if (aegis->eh->move_resize != EndMoveResizeType) break;
							   while (! maprequest_list->empty()) {
								   XPutBackEvent(display, maprequest_list->front());
								   maprequest_list->pop_front();
							   }
							   delete maprequest_list;
							   it = wawindow_list.begin();
							   for (; it != wawindow_list.end(); ++it) {
								   (*it)->dontsend = false;
								   net->setVirtualPos(*it);
								   if ((((*it)->frame->attrib.x +
												   (*it)->frame->attrib.width) > 0 &&
											   (*it)->frame->attrib.x < (int) width) &&
										   (((*it)->frame->attrib.y +
											 (*it)->frame->attrib.height) > 0 &&
											(*it)->frame->attrib.y < (int) height)) {
									   (*it)->sendConfig();
								   }
							   }
							   XUngrabKeyboard(display, CurrentTime);
							   XUngrabPointer(display, CurrentTime);
							   net->setDesktopViewPort(this);
							   return;
		}
	}
}

void AegisScreen::taskSwitcher(void) {
	if (aegis->eh->move_resize != EndMoveResizeType) return;
	if (wawindow_list.empty()) return;
	if (! windowlist_menu) return;

	int workx, worky;
	unsigned int workw, workh;
	getWorkareaSize(&workx, &worky, &workw, &workh);

	windowlist_menu->map(workx + (workw / 2 - windowlist_menu->width / 2),
			worky + (workh / 2 - windowlist_menu->height / 2));
	windowlist_menu->focusFirst();
}

void AegisScreen::previousTask(void) {
	if (aegis->eh->move_resize != EndMoveResizeType) return;
	if (wawindow_list.size() < 2) return;

	list<AegisWindow *>::iterator it = wawindow_list.begin();
	it++;
	(*it)->raise();
	aegis->focusNew((*it)->id, true);
}

void AegisScreen::nextTask(void) {
	if (aegis->eh->move_resize != EndMoveResizeType) return;
	if (wawindow_list.size() < 2) return;

	wawindow_list.back()->raise();
	aegis->focusNew(wawindow_list.back()->id, true);
}

void AegisScreen::pointerFixedAegisrp(char *s) {
	int x, y, mask, i, o_x, o_y;
	unsigned int ui, w, h;
	Window dw;

	mask = XParseGeometry(s, &x, &y, &w, &h);
	if (mask & XNegative) x = width + x;
	if (mask & YNegative) y = height + y;
	XQueryPointer(display, id, &dw, &dw, &o_x, &o_y, &i, &i, &ui);
	x = x - o_x;
	y = y - o_y;
	XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
}

void AegisScreen::pointerRelativeAegisrp(char *s) {
	int x, y, mask;
	unsigned int w, h;

	mask = XParseGeometry(s, &x, &y, &w, &h);
	XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
}

void AegisScreen::goToDesktop(unsigned int number) {
	list<Desktop *>::iterator dit = desktop_list.begin();
	for (; dit != desktop_list.end(); dit++)
		if ((unsigned int) (*dit)->number == number) break;

	if (dit != desktop_list.end() && *dit != current_desktop) {

		ws->aegis->prefocus = None;
		XSetInputFocus(display, None, RevertToNone, CurrentTime);

		(*dit)->workarea.x = current_desktop->workarea.x;
		(*dit)->workarea.y = current_desktop->workarea.y;
		(*dit)->workarea.width = current_desktop->workarea.width;
		(*dit)->workarea.height = current_desktop->workarea.height;
		current_desktop->v_x = v_x;
		current_desktop->v_y = v_y;
		current_desktop = (*dit);


		list<AegisWindow *>::iterator it = wawindow_list.begin();
		for (; it != wawindow_list.end(); ++it) {
			if ((*it)->desktop_mask & (1L << current_desktop->number)) {
				(*it)->show();
				net->setDesktop(*it);
			}
			else
				(*it)->hide();
		}

		list<DockappHandler *>::iterator dock_it = docks.begin();
		for (; dock_it != docks.end(); ++dock_it) {
			if ((*dock_it)->desktop_mask &
					(1L << current_desktop->number)) {
				if ((*dock_it)->hidden) {
					XMapWindow(display, (*dock_it)->id);
					(*dock_it)->hidden = false;
				}
			} else if (! (*dock_it)->hidden &&
					! (*dock_it)->dockapp_list.empty()) {
				XUnmapWindow(display, (*dock_it)->id);
				(*dock_it)->hidden = true;
			}
		}
		updateWorkarea();
		moveViewportTo(current_desktop->v_x, current_desktop->v_y);
		net->setCurrentDesktop(this);
		aegis->focusRevertFrom(this, None);
	} else
		if (dit == desktop_list.end())
			showAegisrningMessage(__FUNCTION__, "bad desktop id `%d', "
					"desktop %d doesn't exist", number, number);
}

void AegisScreen::nextDesktop(void) {
	if (current_desktop->number + 1 == config.desktops)
		goToDesktop(0);
	else
		goToDesktop(current_desktop->number + 1);
}

void AegisScreen::previousDesktop(void) {
	if (current_desktop->number == 0)
		goToDesktop(config.desktops - 1);
	else
		goToDesktop(current_desktop->number - 1);
}

void AegisScreen::findClosestWindow(int direction) {
	AegisWindow *closest = NULL;
	Window w;
	int i, origin_x, origin_y;
	unsigned int ui;
	int closest_diff_x = INT_MAX, closest_diff_y = INT_MAX;
	XQueryPointer(display, id, &w, &w, &origin_x, &origin_y, &i, &i, &ui);
	list<AegisWindow *>::iterator it = wawindow_list.begin();
	for (; it != wawindow_list.end(); it++) {
		if ((! (*it)->mapped) || (*it)->hidden ||
				(! ((*it)->wstate & StateTasklistMask)))
			continue;
		int middle_x = (*it)->frame->attrib.x + (*it)->frame->attrib.width / 2;
		int middle_y = (*it)->frame->attrib.y +
			(*it)->frame->attrib.height / 2;
		switch (direction) {
			case NorthDirection:
			case SouthDirection: {
									 int diff_y;
									 if (direction == NorthDirection) diff_y = origin_y - middle_y;
									 else diff_y = middle_y - origin_y;
									 if (diff_y <= 0) continue;
									 if (diff_y < closest_diff_y) {
										 closest = *it;
										 closest_diff_y = diff_y;
									 }
								 } break;
			case WestDirection:
			case EastDirection: {
									int diff_x;
									if (direction == WestDirection) diff_x = origin_x - middle_x;
									else diff_x = middle_x - origin_x;
									if (diff_x <= 0) continue;
									if (diff_x < closest_diff_x) {
										closest = *it;
										closest_diff_x = diff_x;
									}
								} break;
		}
	}

	if (closest) {
		if (closest->attrib.x >= (int) width ||
				closest->attrib.y >= (int) height ||
				(closest->attrib.x + (int) closest->attrib.width) <= 0 ||
				(closest->attrib.y + (int) closest->attrib.height) <= 0) {
			int newvx, newvy, x, y;
			x = ws->v_x + closest->attrib.x;
			y = ws->v_y + closest->attrib.y;
			newvx = (x / width) * width;
			newvy = (y / height) * height;
			moveViewportTo(newvx, newvy);
		}
		XWarpPointer(display, None, closest->frame->id, 0, 0, 0, 0,
				closest->frame->attrib.width / 2,
				closest->frame->attrib.height / 2);
	}
}

void AegisScreen::addDockapp(Dockapp *dockapp, char *handlername) {
	XSetWindowAttributes attrib_set;
	DockappHandler *handler = NULL;

	attrib_set.event_mask = PropertyChangeMask | StructureNotifyMask |
		FocusChangeMask | EnterWindowMask | LeaveWindowMask;
	attrib_set.do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask |
		ButtonMotionMask;

	if (! dockapp->prio_set)
		net->getDockappPrio(dockapp);

	if (! handlername)
		handlername = aegis->net->getDockappHandler(dockapp);

	if (! handlername)
		handlername = AE_STRDUP("default");

	list<DockappHandler *>::iterator it = docks.begin();
	for (; it != docks.end(); it++)
		if (! strcmp((*it)->name, handlername)) {
			handler = *it;
			break;
		}

	if (! handler) {
		handler = new DockappHandler(this, handlername);
		handler->commonStyleUpdate();
		docks.push_back(handler);
	}

	if (dockapp->dh) {
		if (dockapp->dh == handler) return;
		dockapp->dh->dockapp_list.remove(dockapp);
		dockapp->dh->update();
	}
	dockapp->dh = handler;

	XSelectInput(display, dockapp->id, NoEventMask);
	XReparentWindow(ws->display, dockapp->id, handler->id, 0, 0);
	XChangeSaveSet(display, dockapp->id, SetModeInsert);
	XMapRaised(ws->display, dockapp->id);
	XFlush(display);

	XChangeWindowAttributes(display, dockapp->id,
			CWEventMask | CWDontPropagate, &attrib_set);

	list<Dockapp *>::iterator dit = handler->dockapp_list.begin();
	for (; dit != handler->dockapp_list.end() &&
			(*dit)->prio <= dockapp->prio; dit++);
	handler->dockapp_list.insert(dit, dockapp);
	ws->net->setDockappHandler(dockapp);
	ws->net->setDockappPrio(dockapp);
	handler->update();
}

#ifdef    RANDR
void AegisScreen::rrUpdate(void) {
	vdpi = ((double) height * 25.4) /
		(double) DisplayHeightMM(display, screen_number);
	hdpi = ((double) width * 25.4) /
		(double) DisplayWidthMM(display, screen_number);

	v_xmax = (config.virtual_x - 1) * width;
	v_ymax = (config.virtual_y - 1) * height;

	XMoveResizeWindow(display, west->id, 0, 0, 2, height);
	XMoveResizeWindow(display, east->id, width - 2, 0, 2, height);
	XMoveResizeWindow(display, north->id, 0, 0, width, 2);
	XMoveResizeWindow(display, south->id, 0, height - 2, width, 2);

	list<DockappHandler *>::iterator dit = docks.begin();
	for (; dit != docks.end(); ++dit)
		(*dit)->update();

	updateWorkarea();

	clearAllCacheAndRedraw();

	net->setDesktopGeometry(this);
}
#endif // RANDR

void AegisScreen::smartName(AegisWindow *ww) {
	int match = 0;
	list<AegisWindow *>::iterator it = wawindow_list_map_order.begin();
	for (; it != wawindow_list_map_order.end(); it++) {
		if (*it == ww) continue;
		if ((*it)->deleted) continue;
		int i = 0;
		for (;i < ww->realnamelen && i < (*it)->realnamelen; i++)
			if ((*it)->name[i] != ww->name[i])
				break;
		if (i == ww->realnamelen && i == (*it)->realnamelen) {
			char *newn = new char[(*it)->realnamelen + 6];
			(*it)->name[(*it)->realnamelen] = '\0';
			if ((*it)->realnamelen == 0)
				sprintf(newn, "<%d>", match + 1);
			else
				sprintf(newn, "%s <%d>", (*it)->name, match + 1);
			delete [] (*it)->name;
			(*it)->name = newn;
			(*it)->drawDecor();
			net->setVisibleName(*it);
			match++;
			if (match >= 998) return;
		}
	}
	if (match) {
		char *newn = new char[ww->realnamelen + 6];
		if (ww->realnamelen == 0)
			sprintf(newn, "<%d>", match + 1);
		else
			sprintf(newn, "%s <%d>", ww->name, match + 1);
		delete [] ww->name;
		ww->name = newn;
	}
}

void AegisScreen::smartNameRemove(AegisWindow *ww) {
	int match = 1;
	bool second = false;
	AegisWindow *fw = NULL;
	list<AegisWindow *>::iterator it = wawindow_list_map_order.begin();
	for (; it != wawindow_list_map_order.end(); it++) {
		if (*it == ww) continue;
		if ((*it)->deleted) continue;
		int i = 0;
		for (;i < ww->realnamelen && i < (*it)->realnamelen; i++)
			if ((*it)->name[i] != ww->name[i])
				break;
		if (i == ww->realnamelen && i == (*it)->realnamelen) {
			if (second || fw) {
				second = true;
				char *newn = new char[(*it)->realnamelen + 6];
				(*it)->name[(*it)->realnamelen] = '\0';
				if ((*it)->realnamelen == 0)
					sprintf(newn, "<%d>", match + 1);
				else
					sprintf(newn, "%s <%d>", (*it)->name, match + 1);
				delete [] (*it)->name;
				(*it)->name = newn;
				(*it)->drawDecor();
				net->setVisibleName(*it);
				match++;
				if (match >= 998) return;
			} else
				fw = *it;
		}
	}
	if (fw) {
		char *newn = new char[fw->realnamelen + 6];
		fw->name[fw->realnamelen] = '\0';
		if (second) {
			if (ww->realnamelen == 0)
				sprintf(newn, "<1>");
			else
				sprintf(newn, "%s <1>", fw->name);
		}
		else
			sprintf(newn, "%s", fw->name);
		delete [] fw->name;
		fw->name = newn;
		fw->drawDecor();
		net->setVisibleName(fw);
	}
}

void AegisScreen::readActionLists(void) {
	Parser *parser;

	Tst<ActionList *>::iterator it = actionlists.begin();
	for (; it != actionlists.end(); it++)
		(*it)->unref();
	actionlists.clean();

	ActionList *a = new ActionList(this, "__baseactionlist");
	actionlists.insert(a->name, a);

	LISTDEL(window_actionlists);
	LISTDEL(screenedge_actionlists);
	LISTDEL(root_actionlists);
	LISTDEL(menu_actionlists);
	LISTDEL(dockappholder_actionlists);

	parser = new Parser(this);
	parser->pushElementHandler(new CfgElementHandler(parser));

	parser->parseFile(config.action_file, false);

	delete parser;
}

void AegisScreen::readStyles(void) {
	Parser *parser;

	Tst<Style *>::iterator sit = styles.begin();
	for (; sit != styles.end(); sit++)
		(*sit)->unref();
	styles.clean();

	Tst<RenderGroup *>::iterator rit = rendergroups.begin();
	for (; rit != rendergroups.end(); rit++)
		(*rit)->unref();
	rendergroups.clean();

	Tst<RenderOpPath *>::iterator pit = paths.begin();
	for (; pit != paths.end(); pit++)
		(*pit)->unref();
	paths.clean();

	Tst<RenderOpText *>::iterator tit = texts.begin();
	for (; tit != texts.end(); tit++)
		(*tit)->unref();
	texts.clean();

	Style *s = new Style(this, "__basestyle");
	RenderOpSolid *op = new RenderOpSolid();
	s->operations.push_back(op);
	styles.insert(s->name, s);

	LISTDEL(window_styles);
	LISTDEL(root_styles);
	LISTDEL(menu_styles);
	LISTDEL(dockappholder_styles);

	parser = new Parser(this);
	parser->pushElementHandler(new CfgElementHandler(parser));

	parser->parseFile(config.style_file, false);

	delete parser;
}

void AegisScreen::readMenus(void) {
	Parser *parser;

	Tst<Menu *>::iterator it = menus.begin();
	for (; it != menus.end(); it++)
		(*it)->unref();
	menus.clean();

	if (windowlist_menu) windowlist_menu->unref();
	windowlist_menu = NULL;

	parser = new Parser(this);
	parser->pushElementHandler(new CfgElementHandler(parser));

	parser->parseFile(config.menu_file, false);

	delete parser;
}

Pixmap AegisScreen::getRootBgPixmap(Pixmap parent_pixmap,
		unsigned int parent_w,
		unsigned int parent_h,
		int x, int y,
		unsigned int w, unsigned int h) {
	GC gc;
	Pixmap pixmap = XCreatePixmap(display, id, w, h, screen_depth);
	unsigned int src_w, src_h;
	Pixmap src = None;
	if (parent_pixmap) {
		src = parent_pixmap;
		src_w = parent_w;
		src_h = parent_h;
	} else if (bg_surface) {
		src = bg_surface->pixmap;
		src_w = bg_surface->width;
		src_h = bg_surface->height;
	}

	if (src) {
		if (x >= (int) src_w) x = x % (int) src_w;
		if (y >= (int) src_h) y = y % (int) src_h;
		if (src_w < w || src_h <  h) {
			gc = XCreateGC(display, src, 0, NULL);
			XSetTile(display, gc, src);
			XSetTSOrigin(display, gc, w - (x % w), h - (y % h));
			XSetFillStyle(display, gc, FillTiled);
			XFillRectangle(display, pixmap, gc, 0, 0, w, h);
			XFreeGC(display, gc);
		} else {
			gc = DefaultGC(display, screen_number);
			XCopyArea(display, src, pixmap, gc, x, y, w, h, 0, 0);
		}
	} else
		XFillRectangle(display, pixmap, black_gc, 0, 0, w, h);

	return pixmap;
}

unsigned char *AegisScreen::getRootBgImage(unsigned char *parent_data,
		unsigned int parent_w,
		unsigned int parent_h,
		int x, int y,
		unsigned int w, unsigned int h) {
	unsigned char *image = new unsigned char[w * h * 4];
	unsigned int src_w, src_h;
	unsigned char *src = NULL, *dst;


	if (parent_data) {
		src = parent_data;
		src_w = parent_w;
		src_h = parent_h;
	} else if (bg_surface) {
		if (! bg_surface->data) {
			XImage *xim = XGetImage(display, bg_surface->pixmap,
					0, 0, bg_surface->width,
					bg_surface->height, AllPlanes, ZPixmap);
			bg_surface->data = new unsigned char[bg_surface->width *
				bg_surface->height *
				sizeof(AegisPixel)];
			memcpy(bg_surface->data, xim->data, bg_surface->width *
					bg_surface->height * sizeof(AegisPixel));
			XDestroyImage(xim);
		}
		src = bg_surface->data;
		src_w = bg_surface->width;
		src_h = bg_surface->height;
	}

	if (src) {
		if (x >= (int) src_w) x = x % (int) src_w;
		if (y >= (int) src_h) y = y % (int) src_h;
		if (src_w < w || src_h <  h) {
			/* XXX: fix client side rendering support for tiled background
			   pixmaps */
			for (unsigned int _p = 0; _p < (w * h); _p++)
				*(((AegisPixel *) image) + _p) = 0xff000000;
			return image;
		} else {
			dst = image;
			src += y * src_w * sizeof(AegisPixel) + x * sizeof(AegisPixel);
			for (unsigned int i = 0; i < h; i++) {
				memcpy(dst, src, w * sizeof(AegisPixel));
				src += src_w * sizeof(AegisPixel);
				dst += w * sizeof(AegisPixel);
			}
		}
	} else {
		for (unsigned int _p = 0; _p < (w * h); _p++)
			*(((AegisPixel *) image) + _p) = 0xff000000;
	}

	return image;
}

AegisSurface *AegisScreen::rgbaToAegisSurface(unsigned char *rgba,
		unsigned int width, unsigned int height) {
	Pixmap pixmap = None;
	cairo_surface_t *surface = NULL;
	unsigned char *data = NULL;

	if (aegis->client_side_rendering) {
		data = rgba;
		surface = cairo_surface_create_for_image((char *) rgba,
				CAIRO_FORMAT_ARGB32,
				width, height,
				width * sizeof(AegisPixel));
	} else {
		pixmap = XCreatePixmap(display, id, width, height, 32);
		XImage *image = XCreateImage(display, visual, 32, ZPixmap, 0,
				(char *) rgba, width, height, 32,
				width * sizeof(AegisPixel));
		GC gc = XCreateGC(display, pixmap, 0, NULL);
		XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, width, height);
		image->data = NULL;
		XDestroyImage(image);
		XFreeGC(display, gc);
		surface = cairo_xlib_surface_create(display, pixmap, NULL,
				CAIRO_FORMAT_ARGB32,
				colormap);
		delete [] rgba;
	}

	return (new AegisSurface(display, surface, pixmap, None, data,
				width, height));
}

list<ActionRegex *> *AegisScreen::getRegexActionList(char *named) {
	if (! strcasecmp(named, "window")) {
		return &window_actionlists;
	} else if (! strcasecmp(named, "menu")) {
		return &menu_actionlists;
	} else if (! strcasecmp(named, "dockappholder")) {
		return &dockappholder_actionlists;
	} else if (! strcasecmp(named, "dockapp")) {
		return &dockapp_actionlists;
	} else if (! strcasecmp(named, "root")) {
		return &root_actionlists;
	} else if (! strcasecmp(named, "screenedge")) {
		return &screenedge_actionlists;
	} else
		return NULL;
}

list<StyleRegex *> *AegisScreen::getRegexStyleList(char *named) {
	if (! strcasecmp(named, "window")) {
		return &window_styles;
	} else if (! strcasecmp(named, "menu")) {
		return &menu_styles;
	} else if (! strcasecmp(named, "dockappholder")) {
		return &dockappholder_styles;
	} else if (! strcasecmp(named, "root")) {
		return &root_styles;
	} else
		return NULL;
}

void AegisScreen::getRegexTargets(WindowRegex *wreg, long int typemask,
		bool mult, list<AWindowObject *> *list) {
	map<Window, WindowObject *>::iterator it = aegis->window_table.begin();
	for (; it != aegis->window_table.end(); it++) {
		if (((*it).second)->ws == this &&
				((*it).second)->type & ANY_ACTION_WINDOW_TYPE &&
				((*it).second)->type & typemask) {
			AWindowObject *awo = (AWindowObject *) ((*it).second);
			if (wreg->match(awo->ids, wreg->state, awo->window_name)) {
				list->push_back(awo);
				if (! mult) return;
			}
		}
	}
}

void AegisScreen::showMessage(char *command, bool command_dynamic,
		const char *function, const char *msg,
		va_list args) {
	if (command) {
		char *cmd;
		char message[8192];

		vsnprintf(message, 8192, msg, args);

		if (command_dynamic)
			cmd = expand(command, NULL, NULL, function, message);
		else
			cmd = command;

		if (! fork()) {
			setsid();
			putenv(displaystring);
			execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
			exit(0);
		}

		if (command_dynamic)
			delete [] cmd;

	} else {
		WARNING << function << ": ";
		vfprintf(stderr, msg, args);
		cerr << endl;
	}
}

void AegisScreen::showInfoMessage(const char *function, const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	showMessage(config.info_command, config.info_command_dynamic,
			function, msg, args);
	va_end(args);
}

void AegisScreen::showAegisrningMessage(const char *function, const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	showMessage(config.warning_command, config.warning_command_dynamic,
			function, msg, args);
	va_end(args);
}

int AegisScreen::getSubwindowId(char *subwindowname) {
	int i = 0;
	vector<char *>::iterator it = subwindow_names.begin();
	for (; it != subwindow_names.end(); it++, i++)
		if (! strcmp(subwindowname, *it))
			return i;

	subwindow_names.insert(subwindow_names.end(), AE_STRDUP(subwindowname));
	return i;
}

char *AegisScreen::getSubwindowName(int subid) {
	return subwindow_names[subid];
}

void AegisScreen::styleUpdate(bool, bool) {
	pushRenderEvent();
}

void AegisScreen::endRender(Pixmap pixmap) {
	if (pixmap)
		aegis->net->setXRootPMapId(this, sb->surface);
}

void AegisScreen::styleDiff(Style *s1, Style *s2,
		bool *pos_change, bool *size_change) {
	double x1, y1, x2, y2;
	double w1, h1, w2, h2;
	double sp1;
	double sp2;

	s1->calcPositionAndSize(1, 1, hdpi, vdpi, &x1, &y1, &w1, &h1);
	s1->calcPositionAndSize(1, 1, hdpi, vdpi, &x2, &y2, &w2, &h2);

	if (x1 != x2 || y1 != y2) *pos_change = true;
	if (w1 != w2 || h1 !=  h2) *size_change = true;

	if (*size_change) return;

	calc_length(s1->top_spacing, s1->top_spacing_u, vdpi, height, &sp1);
	calc_length(s2->top_spacing, s2->top_spacing_u, vdpi, height, &sp2);
	if (sp1 != sp2) {
		*size_change = true;
		return;
	}

	calc_length(s1->bottom_spacing, s1->bottom_spacing_u, vdpi, height, &sp1);
	calc_length(s2->bottom_spacing, s2->bottom_spacing_u, vdpi, height, &sp2);
	if (sp1 != sp2) {
		*size_change = true;
		return;
	}

	calc_length(s1->left_spacing, s1->left_spacing_u, vdpi, height, &sp1);
	calc_length(s2->left_spacing, s2->left_spacing_u, vdpi, height, &sp2);
	if (sp1 != sp2) {
		*size_change = true;
		return;
	}

	calc_length(s1->right_spacing, s1->right_spacing_u, vdpi, height, &sp1);
	calc_length(s2->right_spacing, s2->right_spacing_u, vdpi, height, &sp2);
	if (sp1 != sp2) {
		*size_change = true;
		return;
	}

	calc_length(s1->grid_spacing, s1->grid_spacing_u, vdpi, height, &sp1);
	calc_length(s2->grid_spacing, s2->grid_spacing_u, vdpi, height, &sp2);
	if (sp1 != sp2)
		*size_change = true;
}

ScreenEdge::ScreenEdge(AegisScreen *wascrn, char *name,
		int x, int y, int width, int height) :
	AWindowObject(NULL, 0, EdgeType, NULL, "edge") {
		XSetWindowAttributes attrib_set;

		ws = wascrn;

		attrib_set.override_redirect = true;
		attrib_set.event_mask = EnterWindowMask | LeaveWindowMask |
			ButtonPressMask | ButtonReleaseMask;

		id = XCreateWindow(ws->display, ws->id, x, y, width, height, 0,
				CopyFromParent, InputOnly, CopyFromParent,
				CWOverrideRedirect | CWEventMask, &attrib_set);

		ws->aegis->window_table.insert(make_pair(id, this));

		ws->aegis->net->wXDNDMakeAwareness(id);

		resetActionList(new AegisStringMap(WindowIDName, name));
	}

ScreenEdge::~ScreenEdge(void) {
	ws->aegis->window_table.erase(id);
	XDestroyWindow(ws->display, id);
}
