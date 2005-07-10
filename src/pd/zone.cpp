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

#include "zone.h"

#include "pd/agrarfeld.h"
#include "pd/speicherfeld.h"
#include "pd/minenfeld.h"
#include "pd/werft.h"
#include "pd/forschungsstation.h"
#include "pd/stadt.h"

#include <math.h>

/*
 * UVZone - Stellt eine Zone dar.
 */


/*
 * Konstruktor.
 */
UVZone::UVZone(long n)
: nummer(n), name(""), besitzer(""), groesse(0), beschreibung(""), T(0.0), N(0.0)
{
}


/*
 * Destruktor.
 */
/*
UVZone::~UVZone()
{
}
*/


/*
 * Klimazone aus den Klimadaten berechnen.
 */
#define WINTER(x) ((x < 3) || (x >= 9))
#define SOMMER(x) ((x >= 3) || (x < 9))
string UVZone::get_klimazone() const
{
	int i;

	int t_min = 0;	// kaeltester Monat
	int t_max = 0;	// waermster Monat
	int n_min = 0;	// regenaermster Monat
	int n_max = 0;	// regenreichster Monat
	for(i = 1; i < 12; i++)
	{
		if(temperatur[i] < temperatur[t_min]) { t_min = i; }
		if(temperatur[i] > temperatur[t_max]) { t_max = i; }
		if(niederschlag[i] < niederschlag[n_min]) { n_min = i; }
		if(niederschlag[i] > niederschlag[n_max]) { n_max = i; }
	}

	int wn_max = 0;	// feuchtester Wintermonat
	int sn_max = 3;	// feuchtester Sommermonat
	double T_winter = 0;	// mittlere Wintertemperatur
	double T_sommer = 0;	// mittlere Sommertemperatur
	double N_winter = 0;		// total Sommerregen
	double N_sommer = 0;		// total Winterregen
	for(i = 0; i < 12; i++)
	{
		if(WINTER(i))
		{
			if(niederschlag[i] > niederschlag[wn_max]) { wn_max = i; }
			T_winter += temperatur[i];
			N_winter += niederschlag[i];
		}
		else
		{
			if(niederschlag[i] > niederschlag[sn_max]) { sn_max = i; }
			T_sommer += temperatur[i];
			N_sommer += niederschlag[i];
		}
	}
	T_winter /= 6;
	T_sommer /= 6;

	// 2: Sommerregen
	// 1: ohne Regenperiode
	// 0: Winterregen
	double toleranz = 0.25;
	double faktor = 1;
	if(N_sommer > N_winter * (1 + toleranz))
	{
		faktor = 2;
	}
	else if(N_winter > N_sommer * (1 + toleranz))
	{
		faktor = 0;
	}

	// Algorithmus Seite 11 uv292.pdf
	if(temperatur[t_max] >= 10.0)
	{
		if((N < ((2.0 * T) + (faktor * 14.0))) && (N >= (T + (faktor * 7.0))))
		{
			return "BS";
		}
		else if(N < (T + (faktor * 7.0)))
		{
			return "BW";
		}
		else
		{
			if(temperatur[t_min] > 18.0)
			{
				if(niederschlag[n_min] >= 60.0)
				{
					return "Af";
				}
				else
				{
					return "Aw";
				}
			}
			else
			{
				// regenaermster Monat in waermerer Jahreszeit
				if(WINTER(n_min) != (T_sommer > T_winter))
				{
					if(niederschlag[n_min] < niederschlag[wn_max]/3)
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Ds";
						}
						else
						{
							return "Cs";
						}
					}
					else
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Df";
						}
						else
						{
							return "Cf";
						}
					}
				}
				else
				{
					if(niederschlag[n_min] < niederschlag[sn_max]/10)
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Dw";
						}
						else
						{
							return "Cw";
						}
					}
					else
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Df";
						}
						else
						{
							return "Cf";
						}
					}
				}
			}
		}
	}
	else
	{
		if(temperatur[t_max] >= 0.0)
		{
			return "ET";
		}
		else
		{
			return "EF";
		}
	}
}
#undef WINTER
#undef SOMMER


/*
 * Jahresmitteltemperatur berechnen.
 */
double UVZone::get_T() const
{
	double t = 0;
	for(int i = 0; i < 12; i++)
	{
		t += temperatur[i];
	}
	return rint(10 * t / 12) / 10;
}


/*
 * Jahresniederschlag berechnen.
 */
double UVZone::get_N() const
{
	double n = 0;
	for(int i = 0; i < 12; i++)
	{
		n += niederschlag[i];
	}
	return n / 10;
}


/*
 * Zone als Einzeiler ausgeben.
 */
string UVZone::to_string_terse() const
{
	return "Zone " + name + " ("
	               + to_string(nummer) + ") ("
	               + besitzer + ") ("
	               + to_string(groesse) + " FUs) ("
	               + get_klimazone() + ")";
}

