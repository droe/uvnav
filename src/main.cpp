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

#include "global.h"

#include "conf.h"
#include "navigator.h"

#include <getopt.h>
#include <pcre.h>

/*
 * Hauptroutine.
 *
 * Hier werden die Kommandozeilenparameter eingelesen und verarbeitet.
 *
 * Instanziert die globale Konfigurations-Engine UVConf sowie den
 * UVNavigator selber, ruft entsprechende Methoden aus, um den
 * Splashscreen anzuzeigen und Auswertungen zu parsen, und uebergibt
 * die Laufzeitkontrolle dann der run()-Methode von UVNavigator.
 */


/*
 * Hilfe.
 */
void usage()
{
	cout << "uvnav [Optionen...] Auswertung" << endl
		 << "Persistente Optionen: (werden gespeichert)" << endl
		 << "        -w --width  (int)     Fensterbreite" << endl
		 << "        -h --height (int)     Fensterhoehe" << endl
		 << "        -r --resizable        Fenstergroesse aenderbar         (impliziert -F)" << endl
		 << "        -R --no-resizable     Fenstergroesse nicht aenderbar" << endl
		 << "        -f --fullscreen       Vollbildschirmmodus              (impliziert -R)" << endl
		 << "        -F --no-fullscreen    Kein Vollbildschirmmodus" << endl
//		 << "        -d --double-buf       Double Buffering verwenden       (impliziert -S)" << endl
//		 << "        -D --no-double-buf    Kein double Buffering verwenden" << endl
//		 << "        -s --software         Software-Videomodus erzwingen    (impliziert -D)" << endl
//		 << "        -S --no-software      Hardware-Videomodus verwenden" << endl
		 << "        -q --quality          Anti-Aliasing und Smoothing (schoener)" << endl
		 << "        -Q --no-quality       Kein Anti-Aliasing/Smoothing (schneller)" << endl
		 << "Nicht persistente Optionen:" << endl
		 << "        -v --verbose          Debuginformationen anzeigen" << endl
		 << "        -V --version          Version anzeigen und beenden" << endl
		 << "        -H --help             Hilfe anzeigen und beenden" << endl;
	cout << endl;
	cout << "Fehler in diesem Programm bitte melden an <" << PACKAGE_BUGREPORT << ">." << endl;
}


void version();


/*
 * main() - Einstiegspunkt und Fehlerbehandlung.
 */
int main(int argc, char* argv[])
{
	extern char *optarg;
	extern int optind;
	int ch;
	
	static struct option longopts[] = {
		{ "width",			required_argument,	0,	'w' },
		{ "height",			required_argument,	0,	'h' },
		{ "resizable",		no_argument,		0,	'r' },
		{ "no-resizable",	no_argument,		0,	'R' },
		{ "fullscreen",		no_argument,		0,	'f' },
		{ "no-fullscreen",	no_argument,		0,	'F' },
		{ "double-buf",		no_argument,		0,	'd' },
		{ "no-double-buf",	no_argument,		0,	'D' },
		{ "software",		no_argument,		0,	's' },
		{ "no-software",	no_argument,		0,	'S' },
		{ "quality",		no_argument,		0,	'q' },
		{ "no-quality",		no_argument,		0,	'Q' },
		{ "verbose",		no_argument,		0,	'v' },
		{ "version",		no_argument,		0,	'V' },
		{ "help",			no_argument,		0,	'H' },
		{ 0,				0,					0,	0 }
	};

	cout << PACKAGE_STRING << " (" << revision << ")" << endl;
	cout << COPYRIGHT << endl;
	cout << "https://projects.roe.ch/trac/uvnav/" << endl;
	cout << "------------------------------------------------------------------------------" << endl;

	try
	{
		int w = 0;
		int h = 0;
		bool r = false;
		bool R = false;
		bool f = false;
		bool F = false;
		bool d = false;
		bool D = false;
		bool s = false;
		bool S = false;
		bool q = false;
		bool Q = false;
		bool v = false;

		while((ch = getopt_long(argc, argv, "w:h:rRfFdDsSqQvVH", longopts, NULL)) != -1)
		{
			switch(ch)
			{
				case 'w':
					w = atol(optarg);
					break;
				case 'h':
					h = atol(optarg);
					break;
				case 'f':
					f = true;
					R = true;
					break;
				case 'F':
					F = true;
					break;
				case 'r':
					r = true;
					F = true;
					break;
				case 'R':
					R = true;
					break;
				case 'd':
					d = true;
					S = true;
					break;
				case 'D':
					D = true;
					break;
				case 's':
					s = true;
					D = true;
					break;
				case 'S':
					S = true;
					break;
				case 'q':
					q = true;
					break;
				case 'Q':
					Q = true;
					break;
				case 'v':
					v = true;
					break;
				case 'V':
					version();
					exit(EX_OK);
					break;
				case 'H':
					usage();
					exit(EX_OK);
					break;
				default:
					usage();
					exit(EX_USAGE);
			}
		}
		argc -= optind;
		argv += optind;

		// keine Argumente
		if(argc == 0)
		{
			usage();
			exit(EX_USAGE);
		}

		// widerspruechliche Argumente
		if((r && R) || (f && F) || (d && D) || (s && S) || (q && Q))
		{
			cerr << " *** Fehler: Widerspruechliche Argumente!" << endl;
			usage();
			exit(EX_USAGE);
		}

		if(!sysdep_file_exists(argv[0]))
		{
			cerr << " *** Fehler: Auswertungsdatei existiert nicht!" << endl;
			exit(EX_NOINPUT);
		}

		if(S)
		{
			cerr << " *** Warnung: Hardware-Modus ist noch nicht benutzbar!" << endl;
		}

		UVConf* conf = new UVConf();

		if(w > 0)
		{
			conf->l_set("screen-width", w);
		}
		if(h > 0)
		{
			conf->l_set("screen-height", h);
		}
		if(r)
		{
			conf->b_set("screen-resizable", true);
		}
		if(R)
		{
			conf->b_set("screen-resizable", false);
		}
		if(f)
		{
			conf->b_set("screen-fullscreen", true);
		}
		if(F)
		{
			conf->b_set("screen-fullscreen", false);
		}
		if(d)
		{
			conf->b_set("screen-double-buf", true);
		}
		if(D)
		{
			conf->b_set("screen-double-buf", false);
		}
		if(s)
		{
			conf->b_set("screen-software", true);
		}
		if(S)
		{
			conf->b_set("screen-software", false);
		}
		if(q)
		{
			conf->b_set("screen-quality", true);
		}
		if(Q)
		{
			conf->b_set("screen-quality", false);
		}

		UVNavigator* nav = new UVNavigator(conf);
		nav->splash();
		nav->load(argv[0], v);
		nav->wait();
		nav->run();

		delete nav;
		delete conf;
		exit(EX_OK);
	}
	catch(bad_alloc e)
	{
		cerr << "*** STL Bad Allocation:" << endl;
		cerr << e.what() << endl;
		exit(EX_OSERR);
	}
	catch(invalid_argument e)
	{
		cerr << "*** STL Invalid Argument:" << endl;
		cerr << e.what() << endl;
		exit(EX_SOFTWARE);
	}
	catch(logic_error e)
	{
		cerr << "*** STL Logic Error:" << endl;
		cerr << e.what() << endl;
		exit(EX_SOFTWARE);
	}
	catch(runtime_error e)
	{
		cerr << "*** STL Runtime Error:" << endl;
		cerr << e.what() << endl;
		exit(EX_SOFTWARE);
	}
	catch(string e)
	{
		cerr << "*** Exception:" << endl;
		cerr << e << endl;
		exit(EX_DATAERR); // FIXME
	}
	catch(char* e)
	{
		cerr << "*** Exception [char*]:" << endl;
		cerr << e << endl;
		exit(EX_SOFTWARE); // FIXME
	}
	catch(const char* e)
	{
		cerr << "*** Exception [const char*]:" << endl;
		cerr << e << endl;
		exit(EX_SOFTWARE); // FIXME
	}
	catch(...)
	{
		cerr << "*** Exception:" << endl;
		cerr << "Unbekannter Exception-Typ!" << endl;
		exit(EX_SOFTWARE);
	}
	exit(1);
}


/*
 * Versionsinformationen ausgeben.
 */
void version()
{
	cout << PACKAGE_STRING << " (" << revision << ")" << endl;
	cout << COPYRIGHT << endl;
	cout << "https://projects.roe.ch/trac/uvnav/" << endl;
#ifdef DEBUG
	cout << "Achtung: DEBUG-Code aktiviert!" << endl;
#endif // DEBUG
	cout << endl;

	// SDL
	const SDL_version* sdl_link = SDL_Linked_Version();
	cout << "SDL:           "
	     << long(sdl_link->major) << "."
	     << long(sdl_link->minor) << "."
	     << long(sdl_link->patch)
	     << " (built/w "
	     << SDL_MAJOR_VERSION << "."
	     << SDL_MINOR_VERSION << "."
	     << SDL_PATCHLEVEL
	     << ")" << endl;

	// SDL_ttf
	SDL_version ttf_build;
	TTF_VERSION(&ttf_build);
	const SDL_version* ttf_link = TTF_Linked_Version();
	cout << "SDL_ttf:       "
	     << long(ttf_link->major) << "."
	     << long(ttf_link->minor) << "."
	     << long(ttf_link->patch)
	     << " (built/w "
	     << long(ttf_build.major) << "."
	     << long(ttf_build.minor) << "."
	     << long(ttf_build.patch)
	     << ")" << endl;

	// SDL_image
	cout << "SDL_image:     yes" << endl;

	// SDL_gfx
	cout << "SDL_gfx:       yes" << endl;

	// PCRE
	cout << "PCRE:          "
	     << PCRE_MAJOR << "."
	     << PCRE_MINOR << " / "
	     << XSTRING(PCRE_DATE)
	     << " (static)" << endl;

	cout << endl;

	// Konfiguration
	cout << "Conf Version:  " << CONF_VERSION << endl;
	cout << "Conf Dir:      " << sysdep_confdir() << endl;
	cout << "Data Dir:      " << sysdep_datadir() << endl;
	cout << endl;
}


