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

#ifndef GLOBAL_H
#define GLOBAL_H

#include "config.h"

#include "stl.h"
#include "debug.h"

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"			// SDL_image
#include "SDL_ttf.h"			// SDL_ttf
#include "SDL_rotozoom.h"		// SDL_gfx
#include "SDL_gfxPrimitives.h"	// SDL_gfx

#define TITLE			"Universum V Navigator"
#define COPYRIGHT		"Copyright (C) 2004 Daniel Roethlisberger"

#define STRING(x)	# x
#define XSTRING(x)	STRING(x)

extern const char* revision;

#define PI 3.141592

#endif // GLOBAL_H
