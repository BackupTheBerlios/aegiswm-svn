/* main.cc

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

#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#include "aegis.h"

void usage(void);
void help(void);
void version(void);

char *program_name;
//{{{
static const struct argument_def {
	char *name;
	char *short_name;
	char *short_description;
	char *description;
	void (*function)(void);
	char *default_value;
} argument_map[] = {
	{ "--display", NULL, "[--display=DISPLAYNAME] ",
		"      --display=DISPLAYNAME    X server to contact", NULL, NULL },
	{ "--rcfile", NULL, "[--rcfile=FILE]\n    ",
		"      --rcfile=RCFILE          Resource-file to use", NULL, NULL },
	{ "--screen-mask", NULL, "[--screen-mask=SCREENS] ",
		"      --screen-mask=SCREENS    Screens to manage", NULL, NULL },
	{ "--script-dir", NULL, "[--script-dir=PATH] ",
		"      --script-dir=PATH        Path to script directory", NULL, NULL },
	{ "--double-click", NULL, "[--double-click=INTERVAL]\n    ",
		"      --double-click=INTERVAL  Double-click interval in milliseconds",
		NULL, NULL },
	{ "--client-side", "-c", "[-c] [--client-side] ",
		"  -c, --client-side            Force client-side rendering", NULL,
		"true" },

#ifdef    THREAD
	{ "--thread", "-t", "[-t] [--thread] ",
		"   -t, --thread               Use threading", NULL, "false" },
	{ "--thread-priority", NULL, "[--thread-priority=VALUE]\n    ",
		"      --thread-priority=VALUE  Rendering thread priority value",
		NULL, NULL },
#endif // THREAD

	{ "--actionfile", NULL, "[--actionfile=FILE] ",
		"      --actionfile=FILE        Action-file to use", NULL, NULL },
	{ "--stylefile", NULL, "[--stylefile=FILE]\n    ",
		"      --stylefile=FILE         Style-file to use", NULL, NULL },
	{ "--menufile", NULL, "[--menufile=FILE] ",
		"      --menufile=FILE          Menu-file to use", NULL, NULL },
	{ "--desktops", NULL, "[--desktops=NUMBER] ",
		"      --desktops=NUMBER        Number of desktops", NULL, NULL },
	{ "--desktop-names", NULL, "[--desktop-names=LIST]\n    ",
		"      --desktop-names=LIST     Comma seperated list of desktops",
		NULL, NULL },
	{ "--virtual-size", NULL, "[--virtual-size=SIZE] ",
		"      --virtual-size=SIZE      Virtual desktop size, e.g. '3x3'",
		NULL, NULL },
	{ "--menu-stacking", NULL, "[--menu-stacking=TYPE] ",
		"      --menu-stacking=TYPE     Menu stacking type, e.g. 'AlwaysOnTop'",
		NULL, NULL },
	{ "--dh-stacking", NULL, "[--dh-stacking=TYPE]\n    ",
		"      --dh-stacking=TYPE       Dockapp-holder stacking type",
		NULL, NULL },
	{ "--transient-not-above", "-t", "[-t] [--transient-not-above] ",
		"  -t, --transient-not-above    Do not keep transient windows above",
		NULL, "true" },
	{ "--revert-to-root", "-r", "[-r] [--revert-to-root]\n    ",
		"  -r, --revert-to-root         Revert focus to window",
		NULL, "true" },
	{ "--external-bg", "-e", "[-e] [--external-bg]\n    ",
		"  -e, --external-bg            Use external background",
		NULL, "true" },
	{ "--info-command", NULL, "[--info-command=COMMAND]\n    ",
		"      --info-command           Info command",
		NULL, NULL },
	{ "--warning-command", NULL, "[--warning-command=COMMAND]\n    ",
		"      --warning-command        Aegisrning command",
		NULL, NULL },
	{ "--usage", NULL, "[--usage] ",
		"      --usage                  Display brief usage message", usage,
		NULL },
	{ "--help", NULL, "[--help] ",
		"      --help                   Show this help message", help, NULL },
	{ "--version", NULL, "[--version]",
		"      --version                Output version information and exit",
		version, NULL }
};
//}}}

//{{{
void parse_arguments(int argc, char **argv, char **values) {
	int i, j, size = sizeof(argument_map) /
		sizeof(struct argument_def);
	for (i = 1; i < argc; i++) {
		for (j = 0; j < size; j++) {
			int match = 0;
			int len = strlen(argument_map[j].name);
			if (! strncmp(argv[i], argument_map[j].name, len))
				match = 1;
			else if (argument_map[j].short_name) {
				len = strlen(argument_map[j].short_name);
				if (! strncmp(argv[i], argument_map[j].short_name, len))
					match = 1;
			}

			if (match) {
				if (argument_map[j].function) {
					(argument_map[j].function)();
					exit(0);
				} else {
					if (*(argv[i] + len) == '\0') {
						if (argument_map[j].default_value)
							values[j] = argument_map[j].default_value;
						else {
							if (i + 1 < argc) values[j] = argv[++i];
							else {
								fprintf(stderr, "%s: option `%s' requires "
										"an argument\n",
										program_name, argv[i]);
								exit(1);
							}
						}
					} else if (*(argv[i] + len) == '=' &&
							(int) strlen(argv[i]) >= (len + 1)) {
						values[j] = argv[i] + len + 1;
					} else
								continue;
				}
				break;
			}
		}
		if (j == size) {
			fprintf(stderr, "%s: unrecognized option `%s'\n", program_name,
					argv[i]); usage(); exit(1);
		}
	}
}
//}}}
//{{{
int main(int argc, char **argv) {
	XEvent e;
	char *arg_values[sizeof(argument_map) / sizeof(struct argument_def)];
	memset(&arg_values, 0, sizeof(argument_map) / sizeof(struct argument_def) *
			sizeof(char *));

	program_name = AE_STRDUP(argv[0]);

	parse_arguments(argc, argv, arg_values);

	delete [] program_name;

	Aegis * aegis = new Aegis(argv, arg_values);
	aegis->eh->eventLoop(&aegis->eh->empty_return_mask, &e);

	exit(1);
}
//}}}
//{{{
void usage(void) {
	unsigned int i, size = sizeof(argument_map) / sizeof(struct argument_def);
	cout << "Usage: " << program_name << " ";
	for (i = 0; i < size; i++)
		cout << argument_map[i].short_description;

	cout << endl << endl << "Type " << program_name <<
		" --help for a full description." << endl << endl;
}
//}}}
//{{{
void help(void) {
	unsigned int i, size = sizeof(argument_map) / sizeof(struct argument_def);

	cout << "Usage: " << program_name << " [OPTION...]" << endl;
	cout << "aegis - an X11 window manager" << endl <<
		endl;

	for (i = 0; i < size; i++)
		cout << argument_map[i].description << endl;

	cout << endl << "Features compiled in:" << endl;
	cout << "   " <<

#ifdef   THREAD
		"thread "
#endif // THREAD

#ifdef    SHAPE
		"shape "
#endif // SHAPE

#ifdef    XINERAMA
		"xinerama "
#endif // XINERAMA

#ifdef    RANDR
		"randr "
#endif // RANDR

#ifdef    PNG
		"png "
#endif // PNG

#ifdef    SVG
		"svg "
#endif // SVG

#ifdef    XCURSOR
		"xcursor "
#endif // XCURSOR

		"" << endl << endl;

	cout << "Report bugs to <david@aegis.org>." << endl;
}
//}}}
//{{{
void version(void) {
	cout << PACKAGE << " " << VERSION << endl;
}
//}}}
