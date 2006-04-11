/* file name  : builtinids.h
 * authors    : Michael Brailsford
 * created    : Sat May 08 17:36:24 CDT 2004
 * copyright  : (c) 2004 Michael Brailsford
 */

#ifndef BUILTINIDS_H
#define BUILTINIDS_H

/// This enumerates all of the builtin Aegis Actions.
enum builtin_ids {
	AEGIS_QUIT = 0,
	AEGIS_RESTART,

    //Window management builtin ids are provided below
    AEGIS_EXPOSE_EVENT,
    AEGIS_MAP_REQUEST_EVENT,

	/// this is the last member of the builtins enum.
	AEGIS_LAST_BUILTIN
};

/// Make the builtin ids act like a type.
typedef enum builtin_ids builtin_t;

/// This just allows some error detection.
#define AEGIS_FIRST_BUILTIN		AEGIS_QUIT

#endif
