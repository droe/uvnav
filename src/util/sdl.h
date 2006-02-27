/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
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

#ifndef SDL_H
#define SDL_H

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"		// SDL_image
#include "SDL_ttf.h"		// SDL_ttf
#include "SDL_rotozoom.h"	// SDL_gfx
#include "SDL_gfxPrimitives.h"	// SDL_gfx

#ifdef WITH_SURFACE_LOCKING
#define LOCK(X) \
	if(SDL_MUSTLOCK(X)) { \
	if(SDL_LockSurface(X) < 0) { \
		throw EXCEPTION("Kann Surface nicht reservieren!"); \
	}}
#define UNLOCK(X) \
	if(SDL_MUSTLOCK(X)) { \
		SDL_UnlockSurface(X); \
	}
#else // WITH_SURFACE_LOCKING
#define LOCK(X) 
#define UNLOCK(X) 
#endif // WITH_SURFACE_LOCKING

#endif // SDL_H
