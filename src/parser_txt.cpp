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

#include "parser_txt.h"

#include "images.h"
#include "sysdep.h"

/*
 * UVParserTXT - Parser fuer Text-Auswertungen
 *
 * Parst Textauswertungen und erstellt eine entsprechende UVWelt,
 * welche die Daten der Auswertung repraesentiert.
 *
 * Es muss ::parse() aufgerufen werden, um eine Auswertung zu parsen.
 *
 * *** FIXME: Nachrichten parsen!
 * *** FIXME: Imperatoren-Toys parsen!
 */


#ifdef DEBUG
#define debug(x,y) parse_debug(x,y)
#else
#define debug(x,y) 
#endif

#undef EXCEPTION
#define EXCEPTION(x) get_exception(x, __FILE__, __LINE__, __FUNCTION__)


/*
 * Konstruktor.
 */
UVParserTXT::UVParserTXT(UVConf* c, UVWelt* w)
: conf(c), welt(w), progress(NULL), verbose(false)
{
	if(welt == NULL)
	{
		welt = new UVWelt();
	}
}


/*
 * Destruktor.
 */
UVParserTXT::~UVParserTXT()
{
}


/*
 * Resultat des Parsens als UVWelt* zurueckgeben.
 */
UVWelt* UVParserTXT::get_welt() const
{
	return welt;
}


/*
 * Parser in Debug-Mode versetzen.
 */
void UVParserTXT::set_verbose()
{
	verbose = true;
}


/*
 * Koennen noch Daten aus Auswertung gelesen werden?
 */
bool UVParserTXT::good() const
{
	return stream.good();
}


/*
 * Eine Zeile aus Auswertung lesen.
 */
string UVParserTXT::getline()
{
	if(!stream.good())
	{
		throw EXCEPTION("Unerwartetes Ende der Auswertung!");
	}
	line++;
	::getline(stream, cur, '\n');
	bytecount += cur.length() + 1;
	if(cur[cur.length() - 1] == '\r')
	{
		cur = cur.substr(0, cur.length() - 1);
	}
	if(verbose)
	{
		cerr << line << ": " << cur << endl;
	}
	if((line % 500 == 0) && (progress))
	{
		progress->update(bytecount);
	}
	return cur;
}


/*
 * Gematchten Bereich aus der aktuellen Zeile entfernen.
 * Falls ganze Zeile gematcht hat, neue Zeile einlesen.
 */
void UVParserTXT::shiftline(UVRegExp* re)
{
	cur = cur.substr(re->get_match_end(), cur.length() - re->get_match_end());
	if(cur == "")
	{
		getline();
	}
}


/*
 * Leerschlaege an Anfang und Ende eines Strings entfernen.
 */
string UVParserTXT::strip(string& s) const
{
	while(s[0] == ' ')
	{
		s = s.substr(1, s.length() - 1);
	}
	while(s[s.length() - 1] == ' ')
	{
		s = s.substr(0, s.length() - 1);
	}
	return s;
}


/*
 * Laedt eine Datei und jagt sie durch den Parser.
 */
void UVParserTXT::parse(const string& file, UVProgress* pro)
{
	cout << "Lade Auswertung: " << file << endl;
	progress = pro;
	if(progress)
	{
		progress->init(sysdep_filesize(file));
	}

	stream.open(file.c_str());
	line = 0;
	bytecount = 0;

	try
	{
		parse_auswertung();
	}
	catch(string e)
	{
		// Exception mit umdefiniertem Makro nochmals werfen
		throw EXCEPTION(e);
	}

	stream.clear();
	stream.close();
	cout << "Auswertung erfolgreich geladen." << endl;
}


/*
 * Eine ganze Auswertung parsen.
 */
void UVParserTXT::parse_auswertung()
{
	parse_header();

	if(welt->copyright == "a PBM (c) 1994-97 by Black Bird Software")
	{
		// Uralte Auswertung
		parse_oldschool_header();
		parse_oldschool_planeten();
		if(cur == "Besondere Nachrichten:")
		{
			// Rest ueberspringen
			while(good())
			{
				getline();
			}
		}
	}
	else
	{
		parse_spielerinfos();
		parse_spielstand();

		static UVRegExp sp_re("^Spionageabwehr:");
		if(sp_re.match(cur))
		{
			parse_imperatorinfos();
		}

		parse_allianzen();
		parse_schiffe();
		parse_planeten();

		if(cur == "Sensorenreport:")
		{
			parse_sensorenreport();
		}

		static UVRegExp fl_re("^Lagerbest.nde auf fremden Schiffen:");
		if(fl_re.match(cur))
		{
			parse_fremde_lager();
		}

		if(cur == "Besondere Nachrichten:")
		{
			parse_nachrichten();
		}
	}

}


/*
 * Die Kopfzeile suchen und parsen.
 */
void UVParserTXT::parse_header()
{
	//   --- Universum V --- a PBM (c) 1994-2004 by Black Bird Software
	static UVRegExp hdr_re("--- (.*?) --- (.*)$");
	while(!hdr_re.match(cur))
	{
		getline();
	}
	debug("header", &hdr_re);
	welt->set_partie(hdr_re.sub(1));
	welt->copyright = hdr_re.sub(2);
	getline();

	parse_leerzeile();
}


/*
 * Header einer uralten Auswertung parsen.
 */
void UVParserTXT::parse_oldschool_header()
{
	UVSpieler* s = new UVSpieler();

	// Name: Pflocky der Erzkönig
	// Name: Busy Eagle der Kämpfer
	// Name: Ritter der Kokosnuss der Diplomat
	static UVRegExp name_re("^Name: +(.*?) der (Diplomat|K.mpfer|H.ndler|Freibeuter|Spion|Killer|Erzk.nig)$");
	if(!name_re.match(cur))
	{
		throw EXCEPTION("Name: fehlt!");
	}
	s->name = name_re.sub(1);
	s->talent = name_re.sub(2);
	getline();

	// Spieler: Daniel Röthlisberger
	// Spieler: NSC
	static UVRegExp sp_re("^Spieler: +(.*)$");
	if(!sp_re.match(cur))
	{
		throw EXCEPTION("Spieler: fehlt!");
	}
	s->spieler = sp_re.sub(1);
	getline();

	s->status = "Imperator";

	// Galaxie: Awakening
	static UVRegExp gal_re("^Galaxie: +(.*)$");
	if(!gal_re.match(cur))
	{
		throw EXCEPTION("Galaxie: fehlt!");
	}
	welt->galaxie = gal_re.sub(1);
	getline();

	// MotU: Roman Meng
	static UVRegExp motu_re("^MotU: +(.*)$");
	if(!motu_re.match(cur))
	{
		throw EXCEPTION("MotU: fehlt!");
	}
	welt->motu = motu_re.sub(1);
	getline();

	// Sternzeit: 55
	static UVRegExp sz_re("^Sternzeit: +(.*)$");
	if(!sz_re.match(cur))
	{
		throw EXCEPTION("Sternzeit: fehlt!");
	}
	welt->sternzeit = sz_re.subtol(1);
	getline();

	// Punkte: 846000
	static UVRegExp pt_re("^Punkte: +(.*)$");
	if(!pt_re.match(cur))
	{
		throw EXCEPTION("Punkte: fehlt!");
	}
	s->legal = pt_re.subtoll(1);
	getline();

	//?Einkommen: 543074 t Erz
	static UVRegExp ein_re("^Einkommen: +(.*) t Erz$");
	if(ein_re.match(cur))
	{
		s->erzertrag = ein_re.subtol(1);
		getline();
	}

	// Konto: 376761 Credits
	static UVRegExp kt_re("^Konto: +(.*) Credits$");
	if(!kt_re.match(cur))
	{
		throw EXCEPTION("Konto: fehlt!");
	}
	s->konto = kt_re.subtoll(1);
	getline();

	welt->set_spieler(s);

	parse_leerzeile();

	// Allianz mit: Duncan (2), Zwellar (2), Black Guy (1), IG Weltraum (1), _
	// 	Loco (2), Commander Strike (2), Proteus (1), Lord McFalcon (1), _
	// 	Johannes-Urban IX (3), Kane (2), Idaho Roschd (2), Mith (2), _
	// 	Huitzilopochtli (3), Phan-Thomas (3), Starlord (2)
	static UVRegExp al_re("^Allianz mit: +(.*)$");
	if(!al_re.match(cur))
	{
		throw EXCEPTION("Allianz mit fehlt!");
	}
//	cerr << "IGNORED: [" << al_re.sub(1) << "]" << endl;
	getline();

	// 0 Prozent des Erzertrages (an die Lager) für
	static UVRegExp erz_re("^([0-9]+) Prozent des Erzertrages");
	if(erz_re.match(cur))
	{
//		cerr << "IGNORED: [" << erz_re.sub(1) << "]" << endl;
		getline();
	}

	// Spionageabwehr
	static UVRegExp spio_re("^Spionageabwehr\\.$");
	if(spio_re.match(cur))
	{
		getline();
	}

	// Erzkurs: 2 Credits pro Tonne Erz
	static UVRegExp kurs_re("^Erzkurs: ([0-9]+) Credits pro Tonne Erz$");
	if(kurs_re.match(cur))
	{
//		cerr << "IGNORED: [" << kurs_re.sub(1) << "]" << endl;
		getline();
	}

	parse_leerzeile();
}


/*
 * Die Spielerinfos parsen.
 */
void UVParserTXT::parse_spielerinfos()
{
	UVSpieler* s = new UVSpieler();

	// Name: Doctor Who
	// Name: Pflocky der Erzkönig
	// Name: Busy Eagle der Kämpfer
	// Name: Ritter der Kokosnuss der Diplomat
	static UVRegExp name_re("^Name: +(.*?)(?: der (Diplomat|K.mpfer|H.ndler|Freibeuter|Spion|Killer|Erzk.nig))?$");
	if(!name_re.match(cur))
	{
		throw EXCEPTION("Name: fehlt!");
	}
	s->name = name_re.sub(1);
	if(name_re.subs() > 1)
	{
		s->talent = name_re.sub(2);
	}
	getline();

	// Spieler: Daniel Röthlisberger
	// Spieler: NSC
	static UVRegExp sp_re("^Spieler: +(.*)$");
	if(!sp_re.match(cur))
	{
		throw EXCEPTION("Spieler: fehlt!");
	}
	s->spieler = sp_re.sub(1);
	getline();

	// Status: Freischaffender
	// Status: Imperator
	static UVRegExp status_re("^Status: +(.*)$");
	if(!status_re.match(cur))
	{
		throw EXCEPTION("Status: fehlt!");
	}
	s->status = status_re.sub(1);
	getline();

	// Gesellschaft: Keine
	static UVRegExp gesellschaft_re("^Gesellschaft: +(.*)$");
	if(!gesellschaft_re.match(cur))
	{
		throw EXCEPTION("Gesellschaft: fehlt!");
	}
	s->gesellschaft = gesellschaft_re.sub(1);
	getline();

	parse_leerzeile();

	// Legalitätsstatus:  5
	static UVRegExp legal_re("^Legalit.tsstatus: +(.*)$");
	if(!legal_re.match(cur))
	{
		throw EXCEPTION("Legalitätsstatus: fehlt!");
	}
	s->legal = legal_re.subtol(1);
	getline();

	// Punkte: 886
	static UVRegExp pt_re("^Punkte: +(.*)$");
	if(!pt_re.match(cur))
	{
		throw EXCEPTION("Punkte: fehlt!");
	}
	s->legal = pt_re.subtoll(1);
	getline();

	//?Erzertrag: 120 t Erz
	static UVRegExp erz_re("^Erzertrag: +(.*) t Erz$");
	if(erz_re.match(cur))
	{
		s->erzertrag = erz_re.subtol(1);
		getline();
	}

	// Konto: 214461 Credits
	static UVRegExp kt_re("^Konto: +(.*) Credits$");
	if(!kt_re.match(cur))
	{
		throw EXCEPTION("Konto: fehlt!");
	}
	s->konto = kt_re.subtoll(1);
	getline();

	welt->set_spieler(s);

	parse_leerzeile();
}


/*
 * Die Spielstand-Infos parsen.
 */
void UVParserTXT::parse_spielstand()
{
	// MotU: Roman Meng
	static UVRegExp motu_re("^MotU: +(.*)$");
	if(!motu_re.match(cur))
	{
		throw EXCEPTION("MotU: fehlt!");
	}
	welt->motu = motu_re.sub(1);
	getline();

	// Sternzeit: 214
	static UVRegExp sz_re("^Sternzeit: +(.*)$");
	if(!sz_re.match(cur))
	{
		throw EXCEPTION("Sternzeit: fehlt!");
	}
	welt->sternzeit = sz_re.subtol(1);
	getline();

	parse_leerzeile();
}


/*
 * Die Imperatoren-Infos parsen.
 */
void UVParserTXT::parse_imperatorinfos()
{
	// Spionageabwehr: 0% Erzertrag
	static UVRegExp spio_re("^Spionageabwehr: +(.*)% Erzertrag$");
	if(!spio_re.match(cur))
	{
		throw EXCEPTION("Spionageabwehr: fehlt!");
	}
	welt->get_spieler()->spionageabwehr = spio_re.subtol(1);
	getline();

	parse_leerzeile();
}


/*
 * Die Allianzen und Kriege parsen.
 */
void UVParserTXT::parse_allianzen()
{
	// *** TODO: Parsing der Allianzpartner vollenden

	//?Mitgesellschafter: Plinius, Rumba Joe, Gigabyte Goeffrey
	static UVRegExp mitges_re("^Mitgesellschafter: +(.*)$");
	if(mitges_re.match(cur))
	{
//		cerr << "IGNORED: [" << mitges_re.sub(1) << "]" << endl;
		getline();
	}

	// Allianz mit Spieler: Zsakash (2), Xantes hocar (1), Kynar (2)
	// Allianz mit: Tass (1), Knorrly Intergalaktisch (1), Aiwendil (2)
	static UVRegExp alsp_re("^Allianz mit(?: Spieler)?: +(.*)$");
	if(!alsp_re.match(cur))
	{
		throw EXCEPTION("Allianzen mit Spieler fehlen!");
	}
//	cerr << "IGNORED: [" << alsp_re.sub(1) << "]" << endl;
	getline();

	// Sie haben diesen Spielern den Krieg erklärt: 
	// Krieg mit Spieler: 
	// Krieg mit: Minion Zork, Great Zork, Drjakard Ceptor, Drjakard I
	static UVRegExp krsp_re("^(?:Sie haben diesen Spielern den Krieg erkl.rt|Krieg mit(?: Spieler)?): +(.*)$");
	if(!krsp_re.match(cur))
	{
		throw EXCEPTION("Kriegserklärungen an Spieler fehlen!");
	}
//	cerr << "IGNORED: [" << krsp_re.sub(1) << "]" << endl;
	getline();

	//?Diese Spieler haben Ihnen den Krieg erklärt: 
	static UVRegExp spkr_re("^Diese Spieler haben Ihnen den Krieg erkl.rt: +(.*)$");
	if(spkr_re.match(cur))
	{
//		cerr << "IGNORED: [" << spkr_re.sub(1) << "]" << endl;
		getline();
	}

	//?Sie haben diesen Gesellschaften den Krieg erklärt: 
	//?Krieg mit Gesellschaft: 
	static UVRegExp krges_re("^(?:Sie haben diesen Gesellschaften den Krieg erkl.rt|Krieg mit Gesellschaft): +(.*)$");
	if(krges_re.match(cur))
	{
//		cerr << "IGNORED: [" << krges_re.sub(1) << "]" << endl;
		getline();
	}

	//?Diese Spieler haben Ihrer Gesellschaft den Krieg erklärt: 
	static UVRegExp geskr_re("^Diese Spieler haben Ihrer Gesellschaft den Krieg erkl.rt: +(.*)$");
	if(geskr_re.match(cur))
	{
//		cerr << "IGNORED: [" << geskr_re.sub(1) << "]" << endl;
		getline();
	}

	//?Diese Spieler haben allen den Krieg erklärt: Clingons Wadish, _
	//     Clingons Emen, Pirat Fjodr, Pirat Knork, Pirat Killerjoe, _
	//     Clingons Ishmani, Pirat Langfinger-Ede
	static UVRegExp allkr_re("^Diese Spieler haben allen den Krieg erkl.rt: +(.*)$");
	if(allkr_re.match(cur))
	{
//		cerr << "IGNORED: [" << allkr_re.sub(1) << "]" << endl;
		getline();
	}

	parse_leerzeile();
}


/*
 * Ein Block Schiffe parsen.
 *
 * Planet p ist der Kontext. Wenn p == NULL handelt es sich um die
 * globale Schiffssektion am Auswertungsanfang, ansonsten werden die
 * Schiffe Planet p zugeordnet.
 */
void UVParserTXT::parse_schiffe(UVPlanet* p)
{
	// * Schiff Cristina (Doctor Who) 100000 BRT  (118314,15922,4 - Vaeroch Agei)
	//   [...]
	static UVRegExp schiff_re("^\\* Schiff ");
	while(schiff_re.match(cur))
	{
		parse_schiff(p);
	}

	if(p == NULL)
	{
		parse_leerzeile();
	}
}


/*
 * Die Planeten parsen.
 */
void UVParserTXT::parse_planeten()
{
	// Coventina (5201) (Niemand) (120588,16267,4 - Vaeroch Agei) (5498,5527)
	//  (5152) (Niemand) (123048,21142,4 - Vaeroch Agei)
	static UVRegExp planet_re("^.*? \\([0-9]+\\) \\(.*?\\) \\(-?[0-9]+,-?[0-9]+,[0-9]+ - .+?\\)(?: \\([0-9,]+\\))?$");
	while(planet_re.match(cur))
	{
		parse_planet();
	}

	parse_leerzeile();
}


/*
 * Die Planeten uralter Auswertungen parsen.
 */
void UVParserTXT::parse_oldschool_planeten()
{
	// Bespin (172) (Busy Eagle) (-15842,-14782) (178,813)
	static UVRegExp old_planet_re("^.*? \\([0-9]+\\) \\(.*?\\) \\( ?-?[0-9]+, ?-?[0-9]+\\)(?: \\([0-9,]+\\))?$");
	while(old_planet_re.match(cur))
	{
		parse_oldschool_planet();
	}

	parse_leerzeile();
}


/*
 * Den Sensorenreport parsen.
 */
void UVParserTXT::parse_sensorenreport()
{
	// Sensorenreport:
	if(cur != "Sensorenreport:")
	{
		throw EXCEPTION("Fehler in Sensorenreport!");
	}
	getline();
	
	parse_leerzeile();

	static UVRegExp report_re("^(?:  )?(Schiff|Container|Kosmische|SensorSonde|InfoSonde)");
	while(report_re.match(cur))
	{
		if(report_re.sub(1) == "Schiff")
		{
			parse_report_schiff();
		}
		else if(report_re.sub(1) == "Container")
		{
			parse_report_container();
		}
		else if(report_re.sub(1) == "Kosmische")
		{
			parse_report_anomalie();
		}
		else if(report_re.sub(1) == "SensorSonde")
		{
			parse_report_sensorsonde();
		}
		else if(report_re.sub(1) == "InfoSonde")
		{
			parse_report_infosonde();
		}
		else
		{
			throw EXCEPTION("Fehler im Sensorenreport!");
		}
	}

	parse_leerzeile();
	if(cur == "")
	{
		parse_leerzeile();
	}
}


/*
 * Lagerbestaende auf fremden Schiffen parsen.
 */
void UVParserTXT::parse_fremde_lager()
{
	static UVRegExp lager_re("^Lagerbest.nde auf fremden Schiffen:$");
	if(!lager_re.match(cur))
	{
		throw EXCEPTION("Fehler in Lagerbestaende auf fremden Schiffen!");
	}
	getline();

	parse_leerzeile();

	// *** Schiffe und Lager parsen
	static UVRegExp indent_re("^  ");
	while(indent_re.match(cur))
	{
		getline();
	}
	parse_leerzeile();
	parse_leerzeile();
}


/*
 * Nachrichten parsen.
 */
void UVParserTXT::parse_nachrichten()
{
	static UVRegExp beso_re("^Besondere Nachrichten:$");
	if(!beso_re.match(cur))
	{
		throw EXCEPTION("Fehler in besonderen Nachrichten!");
	}
	getline();

	parse_leerzeile();

	// *** Besondere Nachrichten (selektiv) parsen
	static UVRegExp dash_re("^- ");
	while(dash_re.match(cur))
	{
		getline();
	}
}


/*
 * Ein Schiff parsen.
 *
 * Planet p ist der Kontext. Wenn p == NULL handelt es sich um ein Schiff
 * in der globalen Schiffssektion am Auswertungsanfang, ansonsten wird das
 * Schiff dem Planet p zugeordnet.
 */
void UVParserTXT::parse_schiff(UVPlanet* p)
{
	// * Schiff Cristina (Doctor Who) 100000 BRT 
	// * Schiff Cristina (Doctor Who) 100000 BRT in der Werft Hubertus (1) 
	// * Schiff Cristina (Doctor Who) 100000 BRT verteidigt auch allierte Zonen
	// * Schiff Cristina (Doctor Who) 100000 BRT  (118314,15922,4 - Vaeroch Agei)
	static UVRegExp schiff_re("^\\* Schiff (.*?) \\((.*?)\\) (\\d+) BRT((?: in der Werft .*? \\([0-9]+\\))?) ((?:verteidigt auch allierte Zonen)?)(?: \\((-?[0-9]+),(-?[0-9]+),([0-9]+) - (.*?)\\))?$");
	if(!schiff_re.match(cur))
	{
		throw EXCEPTION("Fehler in Schiff!");
	}
	debug("schiff", &schiff_re);
	UVSchiff* s = new UVSchiff(
		schiff_re.sub(1),
		schiff_re.sub(2),
		schiff_re.subtol(3));
	if(schiff_re.sub(4) != "")
	{
		static UVRegExp werft_re("^ in der Werft .*? \\(([0-9]+)\\)$");
		werft_re.match(schiff_re.sub(4));
		s->werft = werft_re.subtol(1);
	}
	else
	{
		s->werft = 0;
	}
	s->zonenstatus = (schiff_re.sub(5) != "");
	if(schiff_re.subs() == 5)
	{
		if(p == NULL)
		{
			throw EXCEPTION("Schiff in Orbit ohne Planet!");
		}
	}
	else
	{
		s->x = schiff_re.subtol(6);
		s->y = schiff_re.subtol(7);
		s->dim = schiff_re.subtol(8);
		welt->set_dim(s->dim, schiff_re.sub(9));
		s->planet = 0;
	}
	getline();

	//?  Gemaechlich tuckert die Cristina vor sich hin. [...]
	static UVRegExp beschr_re("^(?:[^ ] |  (?:Geschwindigkeit|Waffenstatus): )");
	if((cur != "") && !beschr_re.match(cur))
	{
		debug("schiff-beschreibung", &beschr_re);
		s->beschreibung = cur.substr(2, cur.length() - 2);
		getline();
	}
	else
	{
		s->beschreibung = "";
	}

	//?  Geschwindigkeit: 17.81 KpZ / Ausrichtung: 252°
	static UVRegExp gesch_re("^  Geschwindigkeit: ([0-9.]+) KpZ / Ausrichtung: ([0-9]+)°$");
	if(gesch_re.match(cur))
	{
		debug("schiff-geschwindigkeit", &gesch_re);
		s->v = gesch_re.subtof(1);
		s->w = gesch_re.subtol(2);
		getline();
	}
	else
	{
		s->v = 0;
		s->w = 0;
	}

	//?  Waffenstatus: 3 / Offensivbereich: 0 / Fluchtwert: 296/367 HP
	//?  Waffenstatus: 3 / Offensivbereich: 0 / Fluchtwert: 120 HP
	static UVRegExp waff_re("^  Waffenstatus: ([0-9]) / Offensivbereich: ([0-9]+) / Fluchtwert: ([0-9]+)(?:/([0-9]+))? HP$");
	if(waff_re.match(cur))
	{
		debug("schiff-waffenstatus", &waff_re);
		s->waffenstatus = waff_re.subtol(1);
		s->offensivbereich = waff_re.subtol(2);
		s->fluchtwert = waff_re.subtol(3);
		if(waff_re.subs() > 3)
		{
			s->hitpoints = waff_re.subtol(4);
		}
		getline();
	}

	//?  Traktorstrahl: An / Treibstofftanks: 14058.008/20000 BRT 
	//?  Traktorstrahl: Aus / Treibstofftanks: 20000 / 20000 BRT
	static UVRegExp trakt_re("^  Traktorstrahl: (An|Aus) / Treibstofftanks: ([0-9.]+) ?/ ?([0-9]+) BRT $");
	if(trakt_re.match(cur))
	{
		debug("schiff-traktorstrahl", &trakt_re);
		s->traktorstrahl = (trakt_re.sub(1) == "An");
		s->treibstoff = trakt_re.subtof(2);
		s->treibstofftanks = trakt_re.subtol(3);
		getline();
	}

	//?  =------------------------------------------------------------=
	static UVRegExp sep_re("^  =-+=$");
	if(sep_re.match(cur))
	{
		getline();
	}

	//?  Pray CX (40/40) 4000 BRT, Argongenerator (40/40) 4000 BRT, SensoStar II (20/20) 3000 BRT
	//?  Kein Bordcomputer, Kein Energiegenerator
	static UVRegExp cpu_re("^  (?!<[0-9]+\\. )(?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein) (B)(o)(r)dcomputer), (?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein) (E)(n)(e)rgiegenerator)(?:, (.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT)?$");
	if(cpu_re.match(cur))
	{
		debug("schiff-bordcomputer", &cpu_re);
		if(cpu_re.sub(1) != "Kein")
		{
			s->bordcomputer = new UVKomponente(cpu_re.sub(1),
							cpu_re.subtol(2),
							cpu_re.subtol(3),
							cpu_re.subtol(4));
		}
		if(cpu_re.sub(5) != "Kein")
		{
			s->energiegenerator = new UVKomponente(cpu_re.sub(5),
							cpu_re.subtol(6),
							cpu_re.subtol(7),
							cpu_re.subtol(8));
		}
		if(cpu_re.subs() > 8)
		{
			s->sensoren = new UVKomponente(cpu_re.sub(9),
							cpu_re.subtol(10),
							cpu_re.subtol(11),
							cpu_re.subtol(12));
		}
		getline();
	}

	//?  Brücke, Standardkabinen (40/40) 2000 BRT
	static UVRegExp bridge_re("^  (?!<[0-9]+\\. )(Br.cke.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT$");
	if(bridge_re.match(cur))
	{
		debug("schiff-mannschaftsraum", &bridge_re);
		s->mannschaftsraum = new UVKomponente(bridge_re.sub(1),
						bridge_re.subtol(2),
						bridge_re.subtol(3),
						bridge_re.subtol(4));
		getline();
	}

	//?  Energieschild I (50/50) 1000 BRT
	//?  Energieschild I (50/50) 1000 BRT, Metallpanzer (200/200) 10000 BRT
	static UVRegExp schild_re("^  (?!<[0-9]+\\. )(.*?(?:[Ss]child|Atomd.mpfer|schirm).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	if(schild_re.match(cur))
	{
		debug("schiff-energieschild", &schild_re);
		s->schild = new UVKomponente(schild_re.sub(1),
						schild_re.subtol(2),
						schild_re.subtol(3),
						schild_re.subtol(4));
		shiftline(&schild_re);
	}

	//?  Metallpanzer (200/200) 10000 BRT
	static UVRegExp panzer_re("^(?:  )?(?!<[0-9]+\\. )(.*?(?:panzer|beschichtung|Zellneuralnetz|Dunkelmaterie).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT$");
	if(panzer_re.match(cur))
	{
		debug("schiff-metallpanzer", &panzer_re);
		s->panzerung = new UVKomponente(panzer_re.sub(1),
						panzer_re.subtol(2),
						panzer_re.subtol(3),
						panzer_re.subtol(4));
		getline();
	}

	//?  1. Impulslaser (30/30) 10000 BRT, 2. Impulslaser (30/30) 10000 BRT
	static UVRegExp ek_re("^(?:  )?([0-9]+)\\. (.*?(?:laser|beschleuniger|Desintegrations-Strahler|Nukleoidenwerfer|kanone).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(ek_re.match(cur))
	{
		debug("schiff-energiekanone", &ek_re);
		s->set_energiekanone(new UVKomponente(
				ek_re.sub(2),
				ek_re.subtol(3),
				ek_re.subtol(4),
				ek_re.subtol(5),
				ek_re.subtol(1)));
		shiftline(&ek_re);
	}

	//?  1. Raketenwerfer (30/30) 10000 BRT, 2. Raketenwerfer (30/30) 10000 BRT
	static UVRegExp rak_re("^(?:  )?([0-9]+)\\. (.*?(?:[Rr]akete|[Tt]orpedo).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(rak_re.match(cur))
	{
		debug("schiff-raketenwerfer", &rak_re);
		s->set_rakete(new UVKomponente(
				rak_re.sub(2),
				rak_re.subtol(3),
				rak_re.subtol(4),
				rak_re.subtol(5),
				rak_re.subtol(1)));
		shiftline(&rak_re);
	}

	//?  1. Erweitertes Schubtriebwerk (89.097% Leistung) (37/40) 25000 BRT, 2. Erweitertes Schubtriebwerk (89.097% Leistung) (37/40) 25000 BRT
	static UVRegExp tw_re("^(?:  )?([0-9]+)\\. (.*?)(?: \\(([0-9.]+)% Leistung\\))? \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(tw_re.match(cur))
	{
		debug("schiff-schubtriebwerk", &tw_re);
		s->set_triebwerk(new UVKomponente(
				tw_re.sub(2),
				tw_re.subtol(4),
				tw_re.subtol(5),
				tw_re.subtol(6),
				tw_re.subtol(1),
				tw_re.subtof(3)));
		shiftline(&tw_re);
	}

	//?  =------------------------------------------------------------=
	// static UVRegExp sep_re("^  =-+=$");
	if(sep_re.match(cur))
	{
		getline();
	}

	//?  Lagerraum: 10000 BRT gesamt, 5259 BRT frei
	//?  Lagerraum: 10000 BRT
	static UVRegExp lager_re("^  Lagerraum: ([0-9]+) BRT(?: gesamt, ([0-9]+) BRT frei)?$");
	if(lager_re.match(cur))
	{
		debug("schiff-lagerraum", &lager_re);
		s->lagerraum = lager_re.subtol(1);
		if(lager_re.subs() > 1)
		{
			s->lagerraum_frei = lager_re.subtol(2);
		}
		getline();
	}

	//*   1. Hopfen (4741 BRT, Doctor Who)
	//*   2. Militaerlaser Klasse X (25000 BRT, Doctor Who)
	//*   3. Boosterpack (10 BRT, Doctor Who)
	//*   4. Forschungsdokument WAB (110 BRT, Doctor Who)
	static UVRegExp ladung_re("^   ([0-9]+)\\. (.*?) \\(([0-9]+) BRT, (.*?)\\)$");
	while(ladung_re.match(cur))
	{
		debug("schiff-lagerraum-ladung", &ladung_re);
		s->set_ladung(new UVLadung(
				ladung_re.subtol(1),
				ladung_re.sub(2),
				ladung_re.subtol(3),
				ladung_re.sub(4)));
		getline();
	}

	welt->set_schiff(s, p);
}


/*
 * Ein Planet parsen.
 */
void UVParserTXT::parse_planet()
{
	// Coventina (5201) (Niemand) (120588,16267,4 - Vaeroch Agei) (5498,5527)
	static UVRegExp planet_re("^(.*?) \\(([0-9]+)\\) \\((.*?)\\) \\((-?[0-9]+),(-?[0-9]+),([0-9]+) - (.*?)\\)(?: \\(([0-9,]+)\\))?$");
	if(!planet_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet", &planet_re);
	UVPlanet* p = new UVPlanet(
		planet_re.subtol(2),
		planet_re.sub(1),
		planet_re.sub(3),
		planet_re.subtol(4),
		planet_re.subtol(5),
		planet_re.subtol(6));
	welt->set_dim(p->dim, planet_re.sub(7));
	if(planet_re.subs() > 7)
	{
		string links_str = planet_re.sub(8);
		static UVRegExp links_re("^,?([0-9]+)");
		while(links_re.match(links_str))
		{
			links_str = links_str.substr(links_re.get_match_end(), \
			                             links_str.length() - links_re.get_match_end());
			p->nachbarn.push_back(links_re.subtol(1));
		}
	}
	getline();

	// Ausserlich einer unscheinbaren Murmel gleich, ...
	static UVRegExp beschr_re("^Bev.lkerung: ");
	if(!beschr_re.match(cur))
	{
		debug("planet-beschreibung", &beschr_re);
		p->beschreibung = cur;
		getline();
	}

	// Bevölkerung: 1485.2 Millionen (Ruhig)
	// Bevölkerung: 623.8 Millionen (REVOLUTION!)
	static UVRegExp bev_re("^Bev.lkerung: ([0-9.]+) Millionen \\((.+?)\\)$");
	if(!bev_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	p->bevoelkerung = bev_re.subtof(1);
	p->zustand = bev_re.sub(2);
	debug("planet-bevoelkerung", &bev_re);
	getline();

	// Minen: 7/65 Fabriken: 5/43
	static UVRegExp minen_re("^Minen: ([0-9]+)/([0-9]+) Fabriken: ([0-9]+)/([0-9]+)$");
	if(!minen_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-minenfabriken", &minen_re);
	p->minen = minen_re.subtol(1);
	p->minen_max = minen_re.subtol(2);
	p->fabriken = minen_re.subtol(3);
	p->fabriken_max = minen_re.subtol(4);
	getline();

	//?Produktion: 0% für Lager, 100% für Bevölkerung
	static UVRegExp prod_re("^Produktion: ([0-9]+)% f.r Lager, [0-9]+% f.r Bev.lkerung$");
	if(prod_re.match(cur))
	{
		debug("planet-produktion", &prod_re);
		p->produktion = prod_re.subtol(1);
		getline();
	}

	// Tropische Vegetation, sehr fruchtbar, 29710 km Diameter
	static UVRegExp dia_re("^(.*), ([0-9]+) km Diameter$");
	if(!dia_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-klima", &dia_re);
	p->klima = dia_re.sub(1);
	p->image = get_image_planet(p->klima);
	p->diameter = dia_re.subtol(2);
	getline();

	// Techlevel: 1
	// Techlevel: 1 Energiegenerator: 5 Tribut: 1234 t Erz
	static UVRegExp tech_re("^Techlevel: ([0-9]+)(?: Energiegenerator: ([0-9]+) Tribut: ([0-9]+) t Erz)?$");
	if(!tech_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-techlevel", &tech_re);
	p->techlevel = tech_re.subtol(1);
	if(tech_re.subs() == 3)
	{
		p->energiegenerator = tech_re.subtol(2);
		p->tribut = tech_re.subtol(3);
	}
	getline();

	// X-Batts: 1 Y-Batts: 1 Z-Batts: 1
	static UVRegExp batts_re("^X-Batts: ([0-9]+) Y-Batts: ([0-9]+) Z-Batts: ([0-9]+) $");
	if(batts_re.match(cur))
	{
		debug("planet-batts", &batts_re);
		p->xbatts = batts_re.subtol(1);
		p->ybatts = batts_re.subtol(2);
		p->zbatts = batts_re.subtol(3);
		getline();
	}

	welt->set_planet(p);

	// Zone  (3) (Niemand) (278 FUs)
	static UVRegExp zone_re("^Zone ");
	while(zone_re.match(cur))
	{
		p->set_zone(parse_zone(p));
	}

	// § Handelsstation 'Red Blood'
	static UVRegExp hs_re("^§ Handelsstation ");
	if(hs_re.match(cur))
	{
		parse_handelsstation(p);
	}

	// *** IMP Basen
	static UVRegExp imp_base_re("^\\+ ");
	while(imp_base_re.match(cur))
	{
		debug("planet-impbase-skip", &imp_base_re);
		getline();
		static UVRegExp imp_base_skip_re("^  ");
		while(imp_base_skip_re.match(cur))
		{
			debug("planet-impbase-skip-whitespace", &imp_base_skip_re);
			getline();
		}
	}

	parse_schiffe(p);

	// *** IMP Toys
	static UVRegExp imp_toys_re("^\\+ ");
	while(imp_toys_re.match(cur))
	{
		debug("planet-imptoys-skip", &imp_toys_re);
		getline();
		static UVRegExp imp_toys_skip_re("^  ");
		while(imp_toys_skip_re.match(cur))
		{
			debug("planet-imptoys-skip-whitespace", &imp_toys_skip_re);
			getline();
		}
	}

	parse_leerzeile();
}


/*
 * Ein Planet einer uralten Auswertung parsen.
 */
void UVParserTXT::parse_oldschool_planet()
{
	// Bespin (172) (Busy Eagle) (-15842,-14782) (178,813)
	static UVRegExp planet_re("^(.*?) \\(([0-9]+)\\) \\((.*?)\\) \\( ?(-?[0-9]+), ?(-?[0-9]+)\\)(?: \\(([0-9,]+)\\))?$");
	if(!planet_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("oldschool-planet", &planet_re);
	UVPlanet* p = new UVPlanet(
		planet_re.subtol(2),
		planet_re.sub(1),
		planet_re.sub(3),
		planet_re.subtol(4),
		planet_re.subtol(5),
		1);
	welt->set_dim(p->dim, welt->galaxie);
	if(planet_re.subs() > 5)
	{
		string links_str = planet_re.sub(6);
		static UVRegExp links_re("^,?([0-9]+)");
		while(links_re.match(links_str))
		{
			links_str = links_str.substr(links_re.get_match_end(), \
			                             links_str.length() - links_re.get_match_end());
			p->nachbarn.push_back(links_re.subtol(1));
		}
	}
	getline();

	// Bevölkerung: 1485.2 Millionen (Ruhig)
	// Bevölkerung: 623.8 Millionen (REVOLUTION!)
	static UVRegExp bev_re("^Bev.lkerung: ([0-9.]+) Millionen \\((.+?)\\)$");
	if(!bev_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	p->bevoelkerung = bev_re.subtof(1);
	p->zustand = bev_re.sub(2);
	debug("oldschool-planet-bevoelkerung", &bev_re);
	getline();

	// Minen: 7/65 
	static UVRegExp minen_re("^Minen: ([0-9]+)/([0-9]+) $");
	if(!minen_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("oldschool-planet-minen", &minen_re);
	p->minen = minen_re.subtol(1);
	p->minen_max = minen_re.subtol(2);
	getline();

	// Fabriken: 5/43
	static UVRegExp fabr_re("^Fabriken: ([0-9]+)/([0-9]+)$");
	if(!fabr_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("oldschool-planet-fabriken", &fabr_re);
	p->fabriken = fabr_re.subtol(1);
	p->fabriken_max = fabr_re.subtol(2);
	getline();

	//?Produktion: 0% für Lager, 100% für Bevölkerung
	static UVRegExp prod_re("^Produktion: ([0-9]+)% f.r Lager, [0-9]+% f.r Bev.lkerung$");
	if(prod_re.match(cur))
	{
		debug("oldschool-planet-produktion", &prod_re);
		p->produktion = prod_re.subtol(1);
		getline();
	}

	// Tropische Vegetation, sehr fruchtbar, 29710 km Diameter
	static UVRegExp dia_re("^(.*), ([0-9]+) km Diameter$");
	if(!dia_re.match(cur))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("oldschool-planet-klima", &dia_re);
	p->klima = dia_re.sub(1);
	p->image = get_image_planet(p->klima);
	p->diameter = dia_re.subtol(2);
	getline();

	//?X-Batts: 1 Y-Batts: 1 Z-Batts: 1
	static UVRegExp batts_re("^X-Batts: ([0-9]+) Y-Batts: ([0-9]+) Z-Batts: ([0-9]+) $");
	if(batts_re.match(cur))
	{
		debug("oldschool-planet-batts", &batts_re);
		p->xbatts = batts_re.subtol(1);
		p->ybatts = batts_re.subtol(2);
		p->zbatts = batts_re.subtol(3);
		getline();
	}

	//?Energiegenerator: 5 Tribut: 1234 t Erz
	static UVRegExp gen_re("^Energiegenerator: ([0-9]+) Tribut: ([0-9]+) t Erz$");
	if(gen_re.match(cur))
	{
		debug("oldschool-planet-energietribut", &gen_re);
		p->techlevel = 0;
		p->energiegenerator = gen_re.subtol(1);
		p->tribut = gen_re.subtol(2);
		getline();
	}

	welt->set_planet(p);

	// Handelsstation 'Paradysse'
	static UVRegExp hs_re("^Handelsstation ");
	if(hs_re.match(cur))
	{
		parse_handelsstation(p);
	}

	// *** Skip alle Imp-Toys
	while(cur != "")
	{
		debug("oldschool-planet-imp-skip", NULL);
		getline();
	}
	parse_leerzeile();
}


/*
 * Eine Zone parsen.
 */
UVZone* UVParserTXT::parse_zone(UVPlanet* p)
{
	UVZone* z;

	// Zone  (4) (Doctor Who) (193 FUs)
	static UVRegExp zone_re("^Zone (.*?) \\(([0-9]+)\\) \\((.+?)\\) \\(([0-9]+) FUs\\)$");
	if(!zone_re.match(cur))
	{
		throw EXCEPTION("Fehler in Zone!");
	}
	debug("zone", &zone_re);
	z = new UVZone(zone_re.subtol(2));
	z->name = zone_re.sub(1);
	z->besitzer = zone_re.sub(3);
	z->groesse = zone_re.subtol(4);
	getline();

	// Beschreibung
	static UVRegExp beschr_re("^(?:[ -][0-9.]+ \\t){12}D?T:[ -][0-9.]+ °C$");
	if(!beschr_re.match(cur))
	{
		debug("zone-beschreibung", &beschr_re);
		z->beschreibung = cur;
		getline();
	}

	int i = 0;

	//  5.8 	 5.7 	 6.3 	 7.7 	 9.7 	 11.3 	 12.5 	 10.6 	 9.3 	 7 	 5.7 	 5.8 	T: 8.1 °C
	// -3.3 	-3.5 	-1.4 	 3.8 	 11.2 	 16.9 	 21.3 	 14.5 	 9.6 	 1.1 	-3.5 	-3.3 	T: 5.2 °C
	// -25 	-27.5 	-25 	-20.1 	-15.1 	-10.1 	-5.2 	-10.1 	-15.1 	-20.1 	-23.8 	-25 	T:-18.6 °C
	//  22.8 	 22.2 	 21.6 	 20.8 	 20.2 	 20.2 	 19.8 	 20.8 	 20.1 	 19.1 	 20.2 	 19.5 	DT: 20.6 °C
	static UVRegExp temp_re("^([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\tD?T:([ -][0-9.]+) °C$");
	if(!temp_re.match(cur))
	{
		throw EXCEPTION("Fehler in Klimadaten!");
	}
	debug("klima-temperaturen", &temp_re);
	for(i = 0; i < 12; i++)
	{
		z->temperatur[i] = temp_re.subtof(i + 1);
	}
	z->T = temp_re.subtof(i + 1);
	getline();

	//  138 	 124 	 111 	 69 	 76 	 41 	 21 	 14 	 32 	 111 	 124 	 138 	N: 99.9 cm
	//  127 	 164 	 146 	 91 	 73 	 84 	 100 	 120 	 91 	 146 	 164 	 127 	N: 143.3 cm
	//  17 	 13 	 9 	 6 	 8 	 13 	 6 	 7 	 8 	 4 	 9 	 13 	N: 11.3 cm
	static UVRegExp ncm_re("^ ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\tN: ([0-9.]+) cm$");
	//  149 	 196 	 164 	 161 	 201 	 214 	 262 	 264 	 299 	 342 	 358 	 405 	GN: 3015 mm
	static UVRegExp nmm_re("^ ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\tGN: ([0-9]+) mm$");
	if(ncm_re.match(cur))
	{
		debug("klima-niederschlag-cm", &ncm_re);
		for(i = 0; i < 12; i++)
		{
			z->niederschlag[i] = ncm_re.subtof(i + 1);
		}
		z->N = ncm_re.subtof(i + 1);
	}
	else if(nmm_re.match(cur))
	{
		debug("klima-niederschlag-mm", &nmm_re);
		for(i = 0; i < 12; i++)
		{
			z->niederschlag[i] = nmm_re.subtof(i + 1);
		}
		z->N = nmm_re.subtof(i + 1) / 10.0;
	}
	else
	{
		throw EXCEPTION("Fehler in Klimadaten!");
	}
	getline();

	if(abs(z->N - z->get_N()) > 0.11)
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (N:" << z->N << "!=" << z->get_N() << ")" << endl;
	}
	if(abs(z->T - z->get_T()) > 0.11)
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (T:" << z->T << "!=" << z->get_T() << ")" << endl;
	}

	// # Agrarfeld 1, 10 FUs
	// # Agrarfeld 1, 10 FUs: Hirse angebaut.200 BRT Ertrag.
	static UVRegExp agrar_re("^# Agrarfeld ([0-9]+), ([0-9]+) FUs(?:: (.*?) angebaut\\.([0-9]+) BRT Ertrag\\.)?$");
	while(agrar_re.match(cur))
	{
		debug("zone-agrarfeld", &agrar_re);
		if(agrar_re.subs() == 2)
		{
			p->set_agrarfeld(new UVAgrarfeld(
				agrar_re.subtol(1),
				z,
				agrar_re.subtol(2)), z);
		}
		else
		{
			p->set_agrarfeld(new UVAgrarfeld(
				agrar_re.subtol(1),
				z,
				agrar_re.subtol(2),
				agrar_re.sub(3),
				agrar_re.subtol(4)), z);
		}
		getline();
	}

	// # SpeicherFeld 1, 10 FUs
	// # SpeicherFeld 1, 10 FUs: 1000 BRT Speicherplatz. Lager leer.
	// # SpeicherFeld 1, 10 FUs: 1000 BRT Speicherplatz. Gelagert werden 1000 BRT Hirse.
	static UVRegExp speicher_re("^# SpeicherFeld ([0-9]+), ([0-9]+) FUs(?:: ([0-9]+) BRT Speicherplatz\\. (?:Lager leer\\.|Gelagert werden ([0-9]+) BRT (.*)\\.)?)?$");
	while(speicher_re.match(cur))
	{
		debug("zone-speicherfeld", &speicher_re);
		if(speicher_re.subs() == 2)
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				speicher_re.subtol(1),
				z,
				speicher_re.subtol(2)), z);
		}
		else if(speicher_re.subs() == 3)
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				speicher_re.subtol(1),
				z,
				speicher_re.subtol(2),
				speicher_re.subtol(3)), z);
		}
		else
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				speicher_re.subtol(1),
				z,
				speicher_re.subtol(2),
				speicher_re.subtol(3),
				speicher_re.subtol(4),
				speicher_re.sub(5)), z);
		}
		getline();
	}

	// # Mine 1, 30 FUs
	// # Mine 1, 30 FUs: Nichts gefunden.
	// # Mine 1, 30 FUs: Phospor entdeckt.1234 BRT Ertrag.
	static UVRegExp mine_re("^# Mine ([0-9]+), ([0-9]+) FUs(?:: )?(?:(Nichts) gefunden\\.)?(?:(.*?) entdeckt\\.([0-9]+) BRT Ertrag\\.)?$");
	while(mine_re.match(cur))
	{
		debug("zone-mine", &mine_re);
		if(mine_re.subs() == 2)
		{
			p->set_minenfeld(new UVMinenfeld(
				mine_re.subtol(1),
				z,
				mine_re.subtol(2)), z);
		}
		else if(mine_re.subs() == 3)
		{
			p->set_minenfeld(new UVMinenfeld(
				mine_re.subtol(1),
				z,
				mine_re.subtol(2),
				mine_re.sub(3),
				0), z);
		}
		else
		{
			p->set_minenfeld(new UVMinenfeld(
				mine_re.subtol(1),
				z,
				mine_re.subtol(2),
				mine_re.sub(4),
				mine_re.subtol(5)), z);
		}
		getline();
	}

	static UVRegExp werft_re("^# Werft ");
	while(werft_re.match(cur))
	{
		p->set_werft(parse_werft(z), z);
	}

	static UVRegExp fs_re("^# Forschungsstation ");
	while(fs_re.match(cur))
	{
		p->set_forschungsstation(parse_forschungsstation(z), z);
	}

	static UVRegExp stadt_re("^# Stadt ");
	while(stadt_re.match(cur))
	{
		p->set_stadt(parse_stadt(z), z);
	}

	return z;
}


/*
 * Eine Werft parsen.
 */
UVWerft* UVParserTXT::parse_werft(UVZone* z)
{
	UVWerft* w;

	// # Werft Blah (1), 200 FUs
	// # Werft Blah (1), 200 FUs: Formel: 110000001, Erzlager: 21234
	static UVRegExp werft_re("^# Werft (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: Formel: ([0-9A-F]{9}), Erzlager: ([0-9]+))?$");
	if(!werft_re.match(cur))
	{
		throw EXCEPTION("Fehler in Werft!");
	}
	debug("werft", &werft_re);
	w = new UVWerft(werft_re.subtol(2), z);
	w->name = werft_re.sub(1);
	w->groesse = werft_re.subtol(3);
	if(werft_re.subs() != 3)
	{
		w->formel = werft_re.sub(4);
		w->erzlager = werft_re.subtol(5);
	}
	getline();

	//?  Beschreibung
	static UVRegExp indent_re("^  ");
	static UVRegExp beschr_re("^  [0-9]+\\. [^ ]");
	if(indent_re.match(cur) && !beschr_re.match(cur))
	{
		debug("werft-beschreibung", NULL);
		w->beschreibung = strip(cur);
		getline();
	}
	//   1. Standardgenerator, 1000 BRT, 20 HP, 15000 Cr
	//      Energiegenerator, Energie 100, Regeneration 10
	//   13. Pray I, 1000 BRT, 20 HP, 10000 Cr
	//       Bordcomputer
	//   97. Normales Schubtriebwerk, 20000 BRT, 20 HP, 5000 Cr
	//       Triebwerk, Verbrauch 0.0005, Geschwindigkeit 10 KpZ
	while(indent_re.match(cur))
	{
		debug("werft-katalog-skip", NULL);
		// *** katalog 2x
		// *** w->set_bauteil(...)
		// *** welt->set_bauteil(...)
		getline();
		getline();
	}
	return w;
}


/*
 * Eine Forschungsstation parsen.
 */
UVForschungsstation* UVParserTXT::parse_forschungsstation(UVZone* z)
{
	UVForschungsstation* f;

	// # Forschungsstation Blah (1), 200 FUs
	// # Forschungsstation Blah (1), 200 FUs: 123 Wissenspunkte
	static UVRegExp fs_re("^# Forschungsstation (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: ([0-9]+) Wissenspunkte)?$");
	if(!fs_re.match(cur))
	{
		throw EXCEPTION("Fehler in Forschungsstation!");
	}
	debug("forschungsstation", &fs_re);
	f = new UVForschungsstation(fs_re.subtol(2), z);
	f->name = fs_re.sub(1);
	f->groesse = fs_re.subtol(3);
	if(fs_re.subs() != 3)
	{
		f->wissenspunkte = fs_re.subtol(4);
	}
	getline();

	//?  Beschreibung
	static UVRegExp beschr_re("^  (.*)$");
	if(beschr_re.match(cur))
	{
		debug("forschungsstation-beschreibung", &beschr_re);
		f->beschreibung = beschr_re.sub(1);
		getline();
	}

	return f;
}


/*
 * Eine Stadt parsen.
 */
UVStadt* UVParserTXT::parse_stadt(UVZone* z)
{
	UVStadt* s;

	// # Stadt Belleville (1), 74 FUs
	// # Stadt Belleville (1), 74 FUs: 1397599 Seelen
	static UVRegExp stadt_re("^# Stadt (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: ([0-9]+) Seelen)?$");
	if(!stadt_re.match(cur))
	{
		throw EXCEPTION("Fehler in Stadt!");
	}
	debug("stadt", &stadt_re);
	s = new UVStadt(stadt_re.subtol(2), z);
	s->name = stadt_re.sub(1);
	s->groesse = stadt_re.subtol(3);
	if(stadt_re.subs() != 3)
	{
		s->einwohner = stadt_re.subtol(4);
	}
	getline();

	//?  Beschreibung
	static UVRegExp beschr_re("^  (.*)$");
	if(beschr_re.match(cur))
	{
		debug("stadt-beschreibung", &beschr_re);
		s->beschreibung = beschr_re.sub(1);
		getline();
	}

	return s;
}


/*
 * Eine Handelsstation parsen.
 */
void UVParserTXT::parse_handelsstation(UVPlanet* p)
{
	UVHandelsstation* h;

	// § Handelsstation 'Red Blood'
	// Handelsstation 'Paradysse'
	static UVRegExp hs_re("^(?:§ )?Handelsstation '(.*)'$");
	if(!hs_re.match(cur))
	{
		throw EXCEPTION("Fehler in Handelsstation!");
	}
	debug("handelsstation", &hs_re);
	h = new UVHandelsstation(hs_re.sub(1), p);
	getline();

	//   Beschreibung
	static UVRegExp beschr_re("^  (.*)$");
	if(beschr_re.match(cur))
	{
		debug("handelsstation-beschreibung", &beschr_re);
		h->beschreibung = beschr_re.sub(1);
		getline();
	}

	welt->set_handelsstation(h, p);
}


/*
 * Ein Schiff im Sensorreport parsen.
 */
void UVParserTXT::parse_report_schiff()
{

	//   Schiff Foo (Doctor Who) 20000 BRT (-12345,12345,4)
	static UVRegExp schiff_re("^(?:  )?Schiff (.*) \\((.*)\\) ([0-9]+) BRT \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$");
	if(!schiff_re.match(cur))
	{
		throw EXCEPTION("Fehler in Schiff!");
	}
	debug("report-schiff", &schiff_re);
	UVSchiff* s = new UVSchiff(
		schiff_re.sub(1),
		schiff_re.sub(2),
		schiff_re.subtol(3));
	s->x = schiff_re.subtol(4);
	s->y = schiff_re.subtol(5);
	s->dim = schiff_re.subtol(6);
	getline();

	//     Geschwindigkeit: 54.62 KpZ / Ausrichtung: 123°
	static UVRegExp geschw_re("^(?:  )?  Geschwindigkeit: ([0-9.]+) KpZ / Ausrichtung: ([0-9]+)°$");
	if(geschw_re.match(cur))
	{
		debug("report-schiff-geschwindigkeit", &geschw_re);
		s->v = geschw_re.subtof(1);
		s->w = geschw_re.subtol(2);
		getline();
	}

	welt->set_schiff(s);
}


/*
 * Ein Container im Sensorreport parsen.
 */
void UVParserTXT::parse_report_container()
{
	UVContainer* c = new UVContainer();

	//   Container 10 BRT  (-45433,74445,4)
	static UVRegExp cont_re("^(?:  )?Container ([0-9]+) BRT  \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$");
	if(!cont_re.match(cur))
	{
		throw EXCEPTION("Fehler in Container!");
	}
	debug("report-container", &cont_re);
	c->groesse = cont_re.subtol(1);
	c->x = cont_re.subtol(2);
	c->y = cont_re.subtol(3);
	c->dim = cont_re.subtol(4);
	getline();

	welt->set_container(c);
}


/*
 * Eine Anomalie im Sensorreport parsen.
 */
void UVParserTXT::parse_report_anomalie()
{
	UVAnomalie* a = new UVAnomalie();

	//   Kosmische Anomalie mit 6 Lichtjahren Durchmesser (-1316,-26761,1)
	static UVRegExp anomalie_re("^(?:  )?Kosmische Anomalie mit ([0-9]+) Lichtjahren Durchmesser \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$");
	if(!anomalie_re.match(cur))
	{
		throw EXCEPTION("Fehler in Anomalie!");
	}
	debug("report-anomalie", &anomalie_re);
	a->radius = anomalie_re.subtol(1) / 2;
	a->x = anomalie_re.subtol(2);
	a->y = anomalie_re.subtol(3);
	a->dim = anomalie_re.subtol(4);
	getline();

	welt->set_anomalie(a);
}


/*
 * Eine Sensorsonde aus dem Sensorenreport parsen.
 */
void UVParserTXT::parse_report_sensorsonde()
{
	UVSensorsonde* s;

	//   SensorSonde 123 (33339,3333,4)
	//   SensorSonde 123 - Lebensdauer: 12 (33339,3333,4)
	static UVRegExp sensor_re("^(?:  )?SensorSonde ([0-9]+) (?:- Lebensdauer: ([0-9]+) )?\\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$");
	if(!sensor_re.match(cur))
	{
		throw EXCEPTION("Fehler in Sensorsonde!");
	}
	debug("report-sensorsonde", &sensor_re);
	s = new UVSensorsonde(sensor_re.subtol(1));
	if(sensor_re.subs() == 4)
	{
		s->x = sensor_re.subtol(2);
		s->y = sensor_re.subtol(3);
		s->dim = sensor_re.subtol(4);
	}
	else
	{
		s->lebensdauer = sensor_re.subtol(2);
		s->x = sensor_re.subtol(3);
		s->y = sensor_re.subtol(4);
		s->dim = sensor_re.subtol(5);
	}
	getline();

	welt->set_sensorsonde(s);
}


/*
 * Eine Infosonde aus dem Sensorenreport parsen.
 */
void UVParserTXT::parse_report_infosonde()
{
	UVInfosonde* i;

	//   InfoSonde 123 (33339,3333,4)
	//   InfoSonde 123 - Lebensdauer: 12 (33339,3333,4)
	static UVRegExp info_re("^(?:  )?InfoSonde ([0-9]+) (?:- Lebensdauer: ([0-9]+) )?\\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$");
	if(!info_re.match(cur))
	{
		throw EXCEPTION("Fehler in Infosonde!");
	}
	debug("report-infosonde", &info_re);
	i = new UVInfosonde(info_re.subtol(1));
	if(info_re.subs() == 4)
	{
		i->x = info_re.subtol(2);
		i->y = info_re.subtol(3);
		i->dim = info_re.subtol(4);
	}
	else
	{
		i->lebensdauer = info_re.subtol(2);
		i->x = info_re.subtol(3);
		i->y = info_re.subtol(4);
		i->dim = info_re.subtol(5);
	}
	getline();

	welt->set_infosonde(i);
}


/*
 * Eine Leerzeile parsen.
 */
void UVParserTXT::parse_leerzeile()
{
	// 
	if(cur != "")
	{
		throw EXCEPTION("Leerzeile erwartet!");
	}
	getline();
}


/*
 * Findet die Bild-ID zu einer textuellen Klimabeschreibung.
 *
 * Basiert auf setKlima() aus Planet.java 1.9 von ToVU
 * Copyright (C) 1998-2004 Daniel Straessle <d@niel.ch>
 * Lizenziert unter der GNU GPL
 * http://www.duffy.ch/universum/tovu/
 */
long UVParserTXT::get_image_planet(const string& s) const
{
	if(s.find("Starke St") != string::npos)
	{
		return IMG_PLANET_01;
	}
	else if(s.find("Toxische Atmosph") != string::npos)
	{
		return IMG_PLANET_02;
	}
	else if(s.find("Kraterlandschaft") != string::npos)
	{
		return IMG_PLANET_03;
	}
	else if(s.find("Vulkanisches Klima") != string::npos)
	{
		return IMG_PLANET_04;
	}
	else if(s.find("Arktisches Klima") != string::npos)
	{
		return IMG_PLANET_05;
	}
	else if(s.find("stenklima") != string::npos)
	{
		return IMG_PLANET_06;
	}
	else if(s.find("Wasseroberfl") != string::npos)
	{
		return IMG_PLANET_07;
	}
	else if(s.find("Gebirge vorherrschend") != string::npos)
	{
		return IMG_PLANET_08;
	}
	else if(s.find("ssigtes Klima") != string::npos)
	{
		return IMG_PLANET_09;
	}
	else if(s.find("Tropische Vegetation") != string::npos)
	{
		return IMG_PLANET_10;
	}
	else
	{
		throw EXCEPTION("Unbekannte Klimabeschreibung!");
	}
}


/*
 * Den letztem Regexp-Match zum Debuggen ausgeben.
 */
#ifdef DEBUG
void UVParserTXT::parse_debug(const string& s, UVRegExp* re) const
{
	if(verbose)
	{
		cerr << ">>> " << s;
		if(re != NULL)
		{
			for(long i = 1; i <= re->subs(); i++)
			{
				cerr << " $" << i << "=" << re->sub(i);
			}
		}
		cerr << endl;
	}
}
#endif


/*
 * Fehlermeldung aufbereiten.
 */
string UVParserTXT::get_exception(const string& text, const string& src_file, const int src_line, const string& src_func) const
{
	return str_stream() << text << " in " << src_func << "() at " << src_file << ":" << src_line << " while processing:\n"
	                    << line << ":" << cur;
}

