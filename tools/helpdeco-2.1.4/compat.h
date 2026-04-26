/*
helpdeco -- utility program to dissect Windows help files
Copyright (C) 2001 Ben Collver

This file is part of helpdeco; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
http://www.gnu.org
*/

#ifndef __compat_h__
#define __compat_h__

#include <limits.h>

#ifdef _MSC_VER
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif
#ifndef NAME_MAX
#define NAME_MAX _MAX_FNAME
#endif
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define stricmp _stricmp
#define strcmpi _stricmp
#define memicmp _memicmp
#define getch _getch
#define strupr _strupr
#define strlwr _strlwr
#else
#include <strings.h>
#define stricmp strcasecmp
#define strcmpi strcasecmp
#define memicmp memcmpi
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif

#ifndef _MSC_VER
void _splitpath(const char *, char *, char *, char *, char *);
void _makepath(char *, const char *, const char *, const char *, const char *);
int getch(void);
int memcmpi(const void *, const void *, size_t);
char *strupr(char *);
char *strlwr(char *);
#endif
#endif
