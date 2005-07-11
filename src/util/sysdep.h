/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2005 Daniel Roethlisberger <roe@chronator.ch>
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

#include "util/stl_string.h"
#include "util/sdl.h"

void sysdep_mkdir(const std::string& path, int mode = 0755);
unsigned long sysdep_filesize(const std::string&);
bool sysdep_file_exists(const std::string&);

void sysdep_screensize(SDL_Rect*);

std::string sysdep_homedir();
std::string sysdep_confdir();
std::string sysdep_datadir();
std::string sysdep_fontfile(const std::string&);
std::string sysdep_imagefile(const std::string&);

#endif // SYSDEP_H
