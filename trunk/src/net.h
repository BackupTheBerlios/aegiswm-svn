/* Net.h

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

#ifndef __NetHandler_hh
#define __NetHandler_hh

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xproto.h>
}

class NetHandler;

#ifndef   UrgencyHint
# define UrgencyHint (1L << 8)
#endif // !UrgencyHint

#define MwmHintsDecorations (1L << 1)

#define MwmDecorAll         (1L << 0)
#define MwmDecorBorder      (1L << 1)
#define MwmDecorHandle      (1L << 2)
#define MwmDecorTitle       (1L << 3)
#define MwmDecorMenu        (1L << 4)
#define MwmDecorMinimize    (1L << 5)
#define MwmDecorMaximize    (1L << 6)

#define PropMotifWmHintsElements 3

typedef struct {
	CARD32 flags;
	CARD32 functions;
	CARD32 decorations;
} MwmHints;

#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD    1
#define _NET_WM_STATE_TOGGLE 2

#define _NET_WM_MOVERESIZE_SIZE_TOPLEFT      0
#define _NET_WM_MOVERESIZE_SIZE_TOP          1
#define _NET_WM_MOVERESIZE_SIZE_TOPRIGHT     2
#define _NET_WM_MOVERESIZE_SIZE_RIGHT        3
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT  4
#define _NET_WM_MOVERESIZE_SIZE_BOTTOM       5
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT   6
#define _NET_WM_MOVERESIZE_SIZE_LEFT         7
#define _NET_WM_MOVERESIZE_MOVE              8
#define _NET_WM_MOVERESIZE_SIZE_KEYBOARD     9
#define _NET_WM_MOVERESIZE_MOVE_KEYBOARD    10

#include "aegis.h"
#include "window.h"

class NetHandler {
	public:
		NetHandler(Aegis *);

		void getWMProtocols(WaWindow *);
		void getWMHints(WaWindow *);
		void getClassHint(WaWindow *);
		void getWMClientMachineHint(WaWindow *);
		void getTransientForHint(WaWindow *);
		void getMWMHints(WaWindow *);
		void getWMNormalHints(WaWindow *);
		void getState(WaWindow *);
		void setState(WaWindow *, int);
		void getWmState(WaWindow *);
		void setWmState(WaWindow *);
		void getVirtualPos(WaWindow *);
		void setVirtualPos(WaWindow *);
		void getWmStrut(WaWindow *);
		void getWmPid(WaWindow *);
		void getWmUserTime(WaWindow *);
		void getXaName(WaWindow *);
		bool getNetName(WaWindow *);
		void setVisibleName(WaWindow *);
		void removeVisibleName(WaWindow *);
		void setDesktop(WaWindow *);
		void setDesktopMask(WaWindow *);
		void getDesktop(WaWindow *);

		void setSupported(WaScreen *);
		void setSupportedWMCheck(WaScreen *, Window);
		void setClientList(WaScreen *);
		void setClientListStacking(WaScreen *);
		void getClientListStacking(WaScreen *);
		void setActiveWindow(WaScreen *, WaWindow *);
		void getActiveWindow(WaScreen *);
		void getDesktopViewPort(WaScreen *);
		void setDesktopViewPort(WaScreen *);
		void setDesktopGeometry(WaScreen *);
		void setNumberOfDesktops(WaScreen *);
		void setCurrentDesktop(WaScreen *);
		void getCurrentDesktop(WaScreen *);
		void setDesktopNames(WaScreen *, char *);

		void wXDNDMakeAwareness(Window);
		void wXDNDClearAwareness(Window);

		void setWorkarea(WaScreen *);
		void deleteSupported(WaScreen *);

		void getXRootPMapId(WaScreen *);
		void setXRootPMapId(WaScreen *, WaSurface *);

		void getWmType(WaWindow *);

		void setAllowedActions(WaWindow *);
		void removeAllowedActions(WaWindow *);

		void getMergedState(WaWindow *);
		void setMergedState(WaWindow *);
		void setMergeAtfront(WaWindow *, Window);
		void getMergeAtfront(WaWindow *);
		void getMergeOrder(WaWindow *);
		void setMergeOrder(WaWindow *);

		bool isSystrayWindow(Window);
		void setSystrayWindows(WaScreen *);

		void sendNotify(AWindowObject *, long int, long int = 0, long int = 0);

		void getWmIconImage(WaWindow *);
		void getWmIconSvg(WaWindow *);
		char *getDockappHandler(Dockapp *);
		void setDockappHandler(Dockapp *);
		void getDockappPrio(Dockapp *);
		void setDockappPrio(Dockapp *);

		void getConfig(WaScreen *, Window, Atom, unsigned int);

		Aegis *aegis;
		Display *display;
		XSizeHints *size_hints;
		XClassHint *classhint;

		Atom utf8_string;

		Atom wm_protocols, wm_delete_window, wm_take_focus;
		Atom wm_state, wm_change_state;

		Atom mwm_hints;

		Atom net_supported, net_supported_wm_check;
		Atom net_client_list, net_client_list_stacking, net_active_window;
		Atom net_desktop_viewport, net_desktop_geometry, net_current_desktop,
			 net_number_of_desktops, net_desktop_names, net_workarea;
		Atom net_wm_desktop, net_wm_name, net_wm_visible_name, net_wm_strut,
			 net_wm_strut_partial, net_wm_pid, net_wm_user_time;
		Atom net_wm_state, net_wm_state_sticky, net_wm_state_shaded,
			 net_wm_state_hidden, net_wm_maximized_vert, net_wm_maximized_horz,
			 net_wm_state_above, net_wm_state_below, net_wm_state_stays_on_top,
			 net_wm_state_stays_at_bottom, net_wm_state_fullscreen,
			 net_wm_state_skip_taskbar;
		Atom net_wm_allowed_actions, net_wm_action_move, net_wm_action_resize,
			 net_wm_action_minimize, net_wm_action_shade, net_wm_action_stick,
			 net_wm_action_maximize_horz, net_wm_action_maximize_vert,
			 net_wm_action_fullscreen, net_wm_action_change_desktop,
			 net_wm_action_close;
		Atom net_wm_window_type, net_wm_window_type_desktop,
			 net_wm_window_type_dock, net_wm_window_type_toolbar,
			 net_wm_window_type_menu, net_wm_window_type_splash,
			 net_wm_window_type_dialog, net_wm_window_type_utility,
			 net_wm_window_type_normal;
		Atom net_close_window, net_moveresize_window, net_wm_moveresize;

		Atom net_wm_icon, net_wm_icon_image, net_wm_icon_svg;

		Atom aegis_net_wm_state_decor, aegis_net_wm_state_decortitle,
			 aegis_net_wm_state_decorborder, aegis_net_wm_state_decorhandles;

		Atom aegis_net_maximized_restore, aegis_net_virtual_pos,
			 aegis_net_wm_desktop_mask;
		Atom aegis_net_wm_merged_to, aegis_net_wm_merged_type,
			 aegis_net_wm_merge_order, aegis_net_wm_merge_atfront;
		Atom aegis_net_restart, aegis_net_shutdown;

		Atom xdndaware, xdndenter, xdndleave;

		Atom kde_net_wm_system_tray_window_for, kde_net_system_tray_windows;

		Atom xrootpmap_id;

		Atom aegis_net_event_notify;

		Atom aegis_net_dockapp_holder, aegis_net_dockapp_prio;

		Atom aegis_net_cfg;

	private:
		XClientMessageEvent cme;

		int real_format;
		Atom real_type;
		unsigned long items_read, items_left;

		Pixmap aegis_xrootpmap_id;
};

#endif // __NetHandler_hh
