/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004 Daniel Roethlisberger <roe@chronator.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/copyleft/
 *
 * $Id$
 */

#ifndef SYSDEP_H
#define SYSDEP_H

#include "global.h"

void sysdep_mkdir(const string& path, int mode = 0755);
unsigned long sysdep_filesize(const string&);
bool sysdep_file_exists(const string&);

void sysdep_screensize(SDL_Rect*);

string sysdep_homedir();
string sysdep_confdir();
string sysdep_datadir();
string sysdep_fontfile(const string&);
string sysdep_imagefile(const string&);

#if defined(HAVE_SYSEXITS_H)
	#include <sysexits.h>
#endif

// successful termination
#if !defined(EX_OK)
	#define EX_OK       0
#endif

// command line usage error
#if !defined(EX_USAGE)
	#define EX_USAGE    64
#endif

// data format error
#if !defined(EX_DATAERR)
	#define EX_DATAERR  65
#endif

// cannot open input
#if !defined(EX_NOINPUT)
	#define EX_NOINPUT  66
#endif

// internal software error
#if !defined(EX_SOFTWARE)
	#define EX_SOFTWARE 70
#endif

// system error (e.g., can't fork)
#if !defined(EX_OSERR)
	#define EX_OSERR    71
#endif

// can't create (user) output file
#if !defined(EX_CANTCREAT)
	#define EX_CANTCREAT    73
#endif

#endif // SYSDEP_H
