/* Png.h

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

#ifndef __AE_Png_hh
#define __AE_Png_hh

#include <stdio.h>

bool check_if_png(FILE *);
unsigned char *read_png_to_rgba(FILE *, int *, int *);

#endif // __Png_hh

