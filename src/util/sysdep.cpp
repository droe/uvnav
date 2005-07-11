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

#include "sysdep.h"

#include "util/exceptions.h"
#include "util/ac_config.h"

using namespace std;

/*
 * Globale betriebssystemabhaengige Hilfsroutinen.
 *
 * Systemspezifische Pfade werden ueber Autotools und CXXFLAGS -D
 * fest im Binary codiert, und ueber globale Funktionen bereitgestellt.
 *
 * Unix     - defined(__unix__)
 *          - getestet unter FreeBSD 5.2.1-p9, GCC 3.3.2
 *          - voll implementiert und unterstuetzt
 *          - koennte noch besser mit Autotools-Resultaten aus
 *            config.h verknuepft werden, um auch exotische
 *            Systeme zu unterstuetzen
 * Win32    - defined(WIN32)
 *          - ungetestet
 *          - noch nicht komplett implementiert
 */


// System erkennen
#if   ( defined(unix) || defined(__unix__) )
	#define SYSDEP_UNIX
	#define SYSDEP_X11
#elif ( defined(WIN32) || defined(__WIN32__) )
	#define SYSDEP_W32
#else
	#error Plattform nicht unterstuetzt! Bitte melden...
#endif


// Header
#if defined(HAVE_SYS_TYPES_H)
	#include <sys/types.h>	// mkdir(2)
#endif
#if defined(HAVE_SYS_STAT_H)
	#include <sys/stat.h>	// mkdir(2), stat(2)
#endif


// Plattformabhengige Definitionen
#if defined(SYSDEP_UNIX)
	#define PATH_SEP "/"
#elif defined(SYSDEP_W32)
	#undef DATADIR
	#include <windows.h>
	#include <io.h>
	#include <shlwapi.h>	// PathRemoveFileSpec
	#define PATH_SEP "\\"
#endif


/*
 * Verzeichnis erstellen.
 */
void sysdep_mkdir(const string& path, int mode)
{
#if defined(SYSDEP_UNIX)
	mkdir(path.c_str(), mode);
#elif defined(SYSDEP_W32)
	mkdir(path.c_str());
	mode++; mode--;
#endif
}


/*
 * Dateigroesse ermitteln.
 */
unsigned long sysdep_filesize(const string& file)
{
	struct stat res;
	if(!stat(file.c_str(), &res))
	{
		return res.st_size;
	}
	else
	{
		throw EXCEPTION("Kann Dateigroesse nicht bestimmen!");
	}
}


/*
 * Test ob Datei existiert.
 */
bool sysdep_file_exists(const string& file)
{
#if defined(HAVE_EACCESS)
	return !eaccess(file.c_str(), F_OK);
#else
	FILE *fp = fopen(file.c_str(), "r");
	if(!fp)
	{
		return false;
	}
	fclose(fp);
	return true;
#endif
}


/*
 * Bildschirmgroesse ermitteln.
 *
 * Basiert auf Video::init() aus src/client/video/sdlopengl/video.cpp 1.11
 * von EGachine
 * Copyright (C) 2004 Jens Thiele <karme@berlios.de>
 * Copyright (C) 2003 Stephan Ferraro <stephan@lazyfellow.com>
 * Lizenz: GNU GPL
 * http://egachine.berlios.de/
 */
void sysdep_screensize(SDL_Rect* rect)
{
	rect->x = 0;
	rect->y = 0;
#if defined(SYSDEP_X11)
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if(SDL_GetWMInfo(&wmi))
	{
		if(wmi.subsystem == SDL_SYSWM_X11)
		{
			rect->w = DisplayWidth(wmi.info.x11.display, 0);
			rect->h = DisplayHeight(wmi.info.x11.display, 0);
		}
		else
		{
			throw EXCEPTION("Unbekanntes Grafik-Subsystem. " + to_string(wmi.subsystem));
		}
	}
	else
	{
		throw EXCEPTION("Kann Bildschirmgroesse nicht ermitteln.");
	}
#elif defined(SYSDEP_W32)
	rect->w = GetSystemMetrics(SM_CXSCREEN);
	rect->h = GetSystemMetrics(SM_CYSCREEN);
#endif
}


/*
 * Pfad zu Heimverzeichnis des Users.
 */
string sysdep_homedir()
{
	char* p_home = getenv("HOME");
	if((p_home == NULL) || (p_home[0] == '\0'))
	{
#if defined(SYSDEP_UNIX)
		throw EXCEPTION("Fehler: $HOME nicht gesetzt oder leer!");
#elif defined(SYSDEP_W32)
		p_home = getenv("USERPROFILE");
		if((p_home == NULL) || (p_home[0] == '\0'))
		{
			throw EXCEPTION("Fehler: Weder $HOME noch $USERPROFILE enthalten Heimverzeichnis!");
		}
#endif
	}
	return string(p_home) + PATH_SEP;
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
 */
string sysdep_datadir()
{
#if defined(SYSDEP_UNIX)
	string d = sysdep_homedir() \
	         + "share" + PATH_SEP \
	         + PACKAGE_TARNAME + PATH_SEP;
	if(sysdep_file_exists(d))
	{
		return d;
	}
	return string(DATADIR) + PATH_SEP + PACKAGE_TARNAME + PATH_SEP;
#elif defined(SYSDEP_W32)
	char buf[1024];
	if(GetModuleFileName(NULL, buf, sizeof(buf) - 1) == 0)
	{
		throw EXCEPTION("GetModuleFileName() schlug fehl!");
	}
	buf[sizeof(buf) - 1] = '\0';
	if(PathRemoveFileSpec(buf) == 0)
	{
		throw EXCEPTION("PathRemoveFileSpec() schlug fehl!");
	}
	return string(buf) + PATH_SEP;
#endif
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


