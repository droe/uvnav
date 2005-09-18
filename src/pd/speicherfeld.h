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

#ifndef SPEICHERFELD_H
#define SPEICHERFELD_H

#include "util/stl_string.h"

class UVSpeicherfeld
{
	public:
		UVSpeicherfeld(long n, long z, long gr = 0, long sp = 0, long me = 0, std::string wa = "");
		virtual ~UVSpeicherfeld();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		long groesse;			// in FUs
		long speicherplatz;		// in BRT
		long menge;				// in BRT
		std::string ware;
};

#endif // SPEICHERFELD_H

