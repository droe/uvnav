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

#include "sysdep.h"

/*
 * Globale betriebssystemabhaengige Hilfsroutinen.
 *
 * Systemspezifische Pfade werden ueber Autotools und CXXFLAGS -D
 * fest im Binary codiert, und ueber globale Funktionen bereitgestellt.
 *
 * Unix     - defined(__unix__)
 *          - getestet unter FreeBSD 5.2.1-p8, GCC 3.3.2
 *          - voll implementiert und unterstuetzt
 *          - koennte noch besser mit Autotools-Resultaten aus
 *            config.h verknuepft werden, um auch exotische
 *            Systeme zu unterstuetzen
 * Win32    - defined(WIN32)
 *          - ungetestet
 *          - noch nicht komplett implementiert
 */


#if defined(__unix__)
	#if defined(HAVE_SYS_TYPES_H)
		#include <sys/types.h>	// mkdir(2)
	#endif
	#if defined(HAVE_SYS_STAT_H)
		#include <sys/stat.h>	// mkdir(2), stat(2)
	#endif
	#define PATH_SEP "/"
#elif defined(WIN32)
	#define PATH_SEP "\\"
	#error Plattform nicht vollstaendig unterstuetzt!
#else
	#error Plattform noch nicht unterstuetzt!
#endif


/*
 * Verzeichnis erstellen.
 */
void sysdep_mkdir(const string& path, int mode)
{
#if defined(__unix__)
	mkdir(path.c_str(), mode);
#elif defined(WIN32)
	mkdir(path.c_str());
#else
	#error Plattform noch nicht unterstuetzt!
#endif
}


/*
 * Dateigroesse ermitteln.
 */
unsigned long sysdep_filesize(const string& file)
{
#if defined(__unix__)
	struct stat res;
	if(!stat(file.c_str(), &res))
	{
		return res.st_size;
	}
	else
	{
		throw EXCEPTION("Kann Dateigroesse nicht bestimmen!");
	}
#else
	#error Plattform noch nicht unterstuetzt!
#endif
}


/*
 * Test ob Datei existiert.
 */
bool sysdep_file_exists(const string& file)
{
#if defined(__unix__)
	return !eaccess(file.c_str(), F_OK);
#else
	#error Plattform noch nicht unterstuetzt!
#endif
}


/*
 * Pfad zu Heimverzeichnis des Users.
 */
string sysdep_homedir()
{
#if defined(__unix__)
	string home = getenv("HOME");
	if(home == "")
	{
		throw EXCEPTION("Fehler: $HOME nicht gesetzt oder leer!");
	}
	return home + PATH_SEP;
#else
	#error Plattform noch nicht unterstuetzt!
#endif
}


/*
 * Pfad zu Konfigurationsdatei.
 *
 * Liefert Pfad zu Konfigurationsdatei.
 */
string sysdep_confdir()
{
	return sysdep_homedir() + "." + string(PACKAGE_TARNAME) + PATH_SEP;
}


/*
 * Pfad zu plattformunabhaengigen Daten (normalerweise $prefix/share/uvnav/).
 *
 * Liefert Pfad mit abschliessendem Slash.
 * DATADIR wird via Autotools definiert.
 *
 * ACHTUNG WIN32: ueberpruefen ob DATADIR via Autotools korrekt gesetzt
 *                werden kann!
 */
string sysdep_datadir()
{
#if defined(__unix__)
	string d = sysdep_homedir() \
	         + "share" + PATH_SEP \
	         + PACKAGE_TARNAME + PATH_SEP;
	if(sysdep_file_exists(d))
	{
		return d;
	}
#endif
	return string(DATADIR) + PATH_SEP + PACKAGE_TARNAME + PATH_SEP;
}


/*
 * Pfad zu Schriftartendatei finden.
 */
string sysdep_fontfile(const string& font)
{
	return sysdep_datadir() + "fonts" + PATH_SEP + font;
}


/*
 * Pfad zu Bilddatei finden.
 */
string sysdep_imagefile(const string& image)
{
	return sysdep_datadir() + "images" + PATH_SEP + image;
}


