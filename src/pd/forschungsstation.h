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

#ifndef FORSCHUNGSSTATION_H
#define FORSCHUNGSSTATION_H

#include "lib/stlstring.h"

class UVForschungsstation
{
	public:
		UVForschungsstation(long, long);
//		~UVForschungsstation();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		string name;
		long groesse;			// in FUs
		long wissenspunkte;
		string beschreibung;
};

#endif // FORSCHUNGSSTATION_H

