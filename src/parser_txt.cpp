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

#define V_RE	verbosity >= 3
#define V_DEBUG	verbosity >= 2
#define V_INFO	verbosity >= 1

/*
 * Konstruktor.
 */
UVParserTXT::UVParserTXT(UVConf* c, int v, UVWelt* w)
: conf(c), welt(w), progress(NULL), verbosity(v)
, stats_schiffe(0), stats_planeten(0)
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
	if(V_DEBUG)
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

	stats_schiffe = 0;
	stats_planeten = 0;

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

	cout << "Auswertung erfolgreich geladen:" << endl;
	cout << "Total " << stats_schiffe << " Schiffe und " << stats_planeten << " Planeten." << endl;
	cout << "------------------------------------------------------------------------------" << endl;
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
	welt->partie = hdr_re.sub(1);
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
	while(cur == "")
	{
		parse_leerzeile();
	}
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
	while(cur == "")
	{
		parse_leerzeile();
	}
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
	while(cur == "")
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
	while(cur == "")
	{
		parse_leerzeile();
	}
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

	static UVRegExp dash_re("^- ");

// ?
	// NMR-Warnung
	// - ACHTUNG! Sollten Sie nächste Runde keinen Zug abgeben, werden Sie aus der Partie unwiderruflich entfernt!
	// - ACHTUNG! Sie können jederzeit aus dem Spiel entfernt werden. Bitte nehmen Sie sofort mit dem Spielleiter Kontakt auf!
	static UVRegExp nmr_re("^- ACHTUNG! S.*? entfernt.*!$");
	if(nmr_re.match(cur))
	{
		debug("nachricht-nmr", &nmr_re);
		// ***
		getline();
	}

// 1. Befehlsgruppe
	// AL - Allianz
	// - Sie haben Sepp die Allianz 1 erklärt.
	static UVRegExp al_o_re("^- Sie haben (.+) die Allianz ([1-3]) erkl.rt\\.$");
	// - Hotzenplotz hat Ihnen die Allianz 2 erklärt.
	static UVRegExp al_i_re("^- (.+) hat Ihnen die Allianz ([1-3]) erkl.rt\\.$");
	// NE - Neutralitaet
	// - Sie haben Sepp die Neutralität erklärt.
	static UVRegExp ne_o_re("^- Sie haben (.+) die Neutralit.t erkl.rt\\.$");
	// - Hotzenplotz hat Ihnen die Neutralität erklärt.
	static UVRegExp ne_i_re("^- (.+) hat Ihnen die Neutralit.t erkl.rt\\.$");
	// KR - Krieg
	// NG - Neutralitaet vs Gesellschaften
	// - Mr. Spock hat ihrer Gesellschaft die Neutralität erklärt.
	static UVRegExp ng_i_re("^- (.+) hat ihrer Gesellschaft die Neutralit.t erkl.rt\\.$");
	// KG - Krieg vs Gesellschaften
	// GS - Geld schicken
	// - Sie haben Doctor Who 1000000000 Cr geschickt.
	static UVRegExp gs_o_re("^- Sie haben (.+) ([0-9]+) Cr geschickt\\.$");
	// - Doctor Who hat Ihnen 1000000000 Cr geschickt.
	static UVRegExp gs_i_re("^- (.+) hat Ihnen ([0-9]+) Cr geschickt\\.$");
	// MSG - Message
	// - Messageempfang von Sepp bestätigt. Nachricht war 154 Zeichen lang.
	static UVRegExp msg_o_re("^- Messageempfang von (.*) best.tigt. Nachricht war ([0-9]+) Zeichen lang\\.$");
	// - Doctor Who: Hey Du Arschgeige!
	static UVRegExp msg_i_re("^- (?!Zentichron [0-9]+: )([^:]{1,30}): (.*)$"); // Ende!
	// - Biberfritz an die Gesellschaft: Testmsg
	static UVRegExp msg_ig_re("^- (?!Zentichron [0-9]+: )([^:]{1,30}) an die Gesellschaft: (.*)$"); // Ende!
	// IN - UDB eintragen
	// FN - UDB abfragen
	// - UDB-Auskunft über Roman Meng: Keine Information vorhanden.
	static UVRegExp fn_re("^- UDB-Auskunft .ber ([^:]+): (.*)$");
	// GX - Gesellschaft beitreten
	// - Sie sind in die Gesellschaft Munchkins, Inc. eingetreten.
	static UVRegExp gx_o_re("^- Sie sind in die Gesellschaft (.+) eingetreten\\.$");
	// - Brummbaerist in die Gesellschaft Munchkins, Inc. eingetreten.
	static UVRegExp gx_i_re("^- (.+?)ist in die Gesellschaft (.+) eingetreten\\.$");
	// - Sie sind aus der Gesellschaft Munchkins, Inc. ausgetreten.
	static UVRegExp gx_o2_re("^- Sie sind aus der Gesellschaft (.+) ausgetreten\\.$");
	// - Brummbaerist aus der Gesellschaft ausgetreten.
	static UVRegExp gx_i2_re("^- (.+?)ist aus der Gesellschaft ausgetreten\\.$");
	// KS - Kopfgeld
	// - Das Kopfgeld auf Sie wurde um 100000 Cr erhöht.
	static UVRegExp ks_re("^- Das Kopfgeld auf Sie wurde um ([0-9]+) Cr erh.ht\\.$");
	// PS - Planet uebertragen
	// ZK - Zone kaufen
	// - Sie haben auf Planet  (1234) Zone 14 gekauft.
	static UVRegExp zk_re("^- Sie haben auf Planet (.*) \\(([0-9]+)\\) Zone ([0-9]+) gekauft\\.$");
	// LO - Aufgeben
	// WA - An Werft andocken
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(al_o_re.match(cur))
		{
			debug("nachricht-al_o", &al_o_re);
			// ***
		}
		else if(al_i_re.match(cur))
		{
			debug("nachricht-al_i", &al_i_re);
			// ***
		}
		else if(ne_o_re.match(cur))
		{
			debug("nachricht-ne_o", &ne_o_re);
			// ***
		}
		else if(ne_i_re.match(cur))
		{
			debug("nachricht-ne_i", &ne_i_re);
			// ***
		}
		else if(ng_i_re.match(cur))
		{
			debug("nachricht-ng_i", &ng_i_re);
			// ***
		}
		else if(gs_o_re.match(cur))
		{
			debug("nachricht-gs_o", &gs_o_re);
			// ***
		}
		else if(gs_i_re.match(cur))
		{
			debug("nachricht-gs_i", &gs_i_re);
			// ***
		}
		else if(msg_o_re.match(cur))
		{
			debug("nachricht-msg_o", &msg_o_re);
			// ***
		}
		else if(fn_re.match(cur))
		{
			debug("nachricht-fn", &fn_re);
			// ***
		}
		else if(gx_o_re.match(cur))
		{
			debug("nachricht-gx_o", &gx_o_re);
			// ***
		}
		else if(gx_i_re.match(cur))
		{
			debug("nachricht-gx_i", &gx_i_re);
			// ***
		}
		else if(gx_o2_re.match(cur))
		{
			debug("nachricht-gx_o2", &gx_o2_re);
			// ***
		}
		else if(gx_i2_re.match(cur))
		{
			debug("nachricht-gx_i2", &gx_i2_re);
			// ***
		}
		else if(ks_re.match(cur))
		{
			debug("nachricht-ks", &ks_re);
			// ***
		}
		else if(zk_re.match(cur))
		{
			debug("nachricht-zk", &zk_re);
			// ***
		}
		else if(msg_ig_re.match(cur)) // Ende!
		{
			debug("nachricht-msg_ig", &msg_ig_re);
			// ***
		}
		else if(msg_i_re.match(cur)) // Ende!
		{
			debug("nachricht-msg_i", &msg_i_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// 2. Befehlsgruppe
	// ZV - Zone vergeben
	// - Sie haben auf Planet Shubito (1234) Zone 1 an Hugo übertragen.
	static UVRegExp zv_o_re("^- Sie haben auf Planet (.*) \\(([0-9]+)\\) Zone ([0-9]+) an (.+) .bertragen\\.$");
	// - Sie haben Planet Shubito (1234) Zone 1 von Hugo übertragen bekommen.
	static UVRegExp zv_i_re("^- Sie haben Planet (.*) \\(([0-9]+)\\) Zone ([0-9]+) von (.+) .bertragen bekommen\\.$");
	// ZB - Zone benennen
	// AP - Agrarfeld bauen
	// AB - Agrarfeld abbauen
	// AT - Agrarfeld abspalten
	// AZ - Agrarfeld zusammenlegen
	// MU - Mine bauen
	// - Auf Planet  (1234) Zone 1 wurde eine Probebohrung für eine Mine vorgenommen und leider nichts gefunden.
	static UVRegExp mu1_re("^- Auf Planet (.*?) \\(([0-9]+)\\) Zone ([0-9]+) wurde eine Probebohrung f.r eine Mine vorgenommen und leider nichts gefunden\\.$");
	// - Auf Planet  (1234) Zone 1 wurde eine Probebohrung für eine Mine vorgenommen und Uran gefunden.
	static UVRegExp mu2_re("^- Auf Planet (.*?) \\(([0-9]+)\\) Zone ([0-9]+) wurde eine Probebohrung f.r eine Mine vorgenommen und (?!leider nichts)(.+) gefunden\\.$");
	// ME - Mine erweitern
	// MN - Mine abreissen
	// AI - Speicher bauen
	// AK - Speicher abspalten
	// AM - Speicherinhalt verschieben
	// AR - Speicher zusammenlegen
	// AU - Speicher abbauen
	// WB - Werft bauen
	// WE - Werft einreissen
	// - Sie haben auf Planet Scheisshaus (1234) die Werft Pfupfikon einreissen lassen.
	static UVRegExp we_re("^- Sie haben auf Planet (.+?) \\(([0-9]+)\\) die Werft (.*?) einreissen lassen\\.$");
	// WZ - Erz zu Werft uebertragen
	// - 1234 BRT Erz wurden von Ihnen an die Werft Yehaa von Doctor Who auf Planet Shubito (1234) geliefert.
	static UVRegExp wz_o_re("^- ([0-9]+) BRT Erz wurden von Ihnen an die Werft (.*?) von (.+?) auf Planet (.*?) \\(([0-9]+)\\) geliefert\\.$");
	// - 1234 BRT Erz wurden an ihre Werft Yehaa auf Planet Shubito (1234) von Doctor Who geliefert.
	static UVRegExp wz_i_re("^- ([0-9]+) BRT Erz wurden an ihre Werft (.*?) auf Planet (.*?) \\(([0-9]+)\\) von (.+) geliefert\\.$");
	// WC - Schiff bauen
	// - Die Cristina (Doctor Who) wurde fertig gebaut und ausgeliefert. Kostenpunkt: 1234567 Cr (und das entsprechende Erz)
	static UVRegExp wc_o_re("^- Die (.+) \\(.+\\) wurde fertig gebaut und ausgeliefert\\. Kostenpunkt: ([0-9]+) Cr \\(und das entsprechende Erz\\)$");
	// - Der Schiffsbau kam aufgrund kompetenter Leute 5% billiger als geplant.
	static UVRegExp wc2_re("^- Der Schiffsbau kam aufgrund kompetenter Leute ([0-9]+)% billiger als geplant\\.$");
	// - Schiffsnamen PiPaPo gibt es schon, es wurde automatisch ein neuer ausgewählt.
	static UVRegExp wc3_re("^- Schiffsnamen (.+?) gibt es schon, es wurde automatisch ein neuer ausgew.hlt\\.$");
	// WL - Komponenten ins Lager liefern
	// - Die Lieferung der Komponente 21 an die Xy kostete 3200 Cr und 100.5 BRT Erz
	static UVRegExp wl_re("^- Die Lieferung der Komponente ([0-9]+) an die (.+?) kostete ([0-9]+) Cr und ([0-9.]+) BRT Erz$");
	// WO - Komponente einbauen
	// - Der Einbau in die Bugaloo kostete 100000 Cr und 1000.5 BRT Erz @<?151
	// - Produktion und Einbau in die Bugaloo kostete 100000 Cr und 1000.5 BRT Erz @151
	static UVRegExp wo_re("^- (?:Der|Produktion und) Einbau in die (.+) kostete ([0-9]+) Cr und ([0-9.]+) BRT Erz$");
	// - Der Komponentenbau kam aufgrund kompetenter Leute 5% billiger als geplant.
	static UVRegExp w2_re("^- Der Komponentenbau kam aufgrund kompetenter Leute ([0-9]+)% billiger als geplant\\.$");
	// - Für den Einbau des Triebwerks mussten zusätzlich 100000 Cr aufgebracht werden.
	static UVRegExp w3_re("^- F.r den Einbau des Triebwerks mussten zus.tzlich ([0-9]+) Cr aufgebracht werden\\.$");
	// WU - Komponente ausbauen
	// - Der Ausbau aus der Bugaloo hat 100000 Cr eingebracht.
	static UVRegExp wu_re("^- Der Ausbau aus der (.+) hat ([0-9]+) Cr eingebracht\\.$");
	// WK - Komponenten ausbauen und ins Lager
	// WR - Komponenten reparieren
	// - Die Reparatur an der Xyz kostete 1234 Cr und 1000.5 BRT Erz.
	static UVRegExp wr_re("^- Die Reparatur an der (.+?) kostete ([0-9]+) Cr und ([0-9.]+) BRT Erz\\.$");
	// FS - Forschungsstation bauen
	// FE - Forschungsstation einreissen
	// FO - Forschen
	// - Die Forschung in der Station Dingsbums hat ihnen 123.2 Wissenspunkte eingebracht.
	static UVRegExp fo_re("^- Die Forschung in der Station (.*?) hat ihnen ([0-9.]+) Wissenspunkte eingebracht\\.$");
	// FW - Werftdoku herstellen
	// FT - Planetendoku herstellen
	// FP - Planetendoku anwenden
	// - Planet Shakizo (1234) ist ein Techlevel aufgestiegen.
	static UVRegExp fp_re("^- Planet (.*) \\([0-9]+\\) ist ein Techlevel aufgestiegen\\.$");
	// - Auf Planet Shakizo (1234) konnte eine neue Zone nutzbar gemacht werden!
	static UVRegExp fp2_re("^- Auf Planet (.*) \\([0-9]+\\) konnte eine neue Zone nutzbar gemacht werden!$");
	// FF - Werftdoku anwenden
	// - Die Formel der Werft 1 auf Planetenbaby (1234) wurde verbessert.
	static UVRegExp ff_re("^- Die Formel der Werft ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde verbessert\\.$");
	// SC - Stadt bauen
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(zv_o_re.match(cur))
		{
			debug("nachricht-zv_o", &zv_o_re);
			// ***
		}
		else if(zv_i_re.match(cur))
		{
			debug("nachricht-zv_i", &zv_i_re);
			// ***
		}
		else if(mu1_re.match(cur))
		{
			debug("nachricht-mu1", &mu1_re);
			// ***
		}
		else if(mu2_re.match(cur))
		{
			debug("nachricht-mu2", &mu2_re);
			// ***
		}
		else if(we_re.match(cur))
		{
			debug("nachricht-we", &we_re);
			// ***
		}
		else if(wz_o_re.match(cur))
		{
			debug("nachricht-wz_o", &wz_o_re);
			// ***
		}
		else if(wz_i_re.match(cur))
		{
			debug("nachricht-wz_i", &wz_i_re);
			// ***
		}
		else if(wc_o_re.match(cur))
		{
			debug("nachricht-wc_o", &wc_o_re);
			// ***
		}
		else if(wc2_re.match(cur))
		{
			debug("nachricht-wc2", &wc2_re);
			// ***
		}
		else if(wc3_re.match(cur))
		{
			debug("nachricht-wc3", &wc3_re);
			// ***
		}
		else if(wl_re.match(cur))
		{
			debug("nachricht-wl", &wl_re);
			// ***
		}
		else if(wo_re.match(cur))
		{
			debug("nachricht-wo", &wo_re);
			// ***
		}
		else if(w2_re.match(cur))
		{
			debug("nachricht-w2", &w2_re);
			// ***
		}
		else if(w3_re.match(cur))
		{
			debug("nachricht-w3", &w3_re);
			// ***
		}
		else if(wu_re.match(cur))
		{
			debug("nachricht-wu", &wu_re);
			// ***
		}
		else if(wr_re.match(cur))
		{
			debug("nachricht-wr", &wr_re);
			// ***
		}
		else if(fo_re.match(cur))
		{
			debug("nachricht-fo", &fo_re);
			// ***
		}
		else if(fp_re.match(cur))
		{
			debug("nachricht-fp", &fp_re);
			// ***
		}
		else if(fp2_re.match(cur))
		{
			debug("nachricht-fp2", &fp2_re);
			// ***
		}
		else if(ff_re.match(cur))
		{
			debug("nachricht-ff", &ff_re);
			// ***
		}
		else if(zk_re.match(cur)) // von 1. Befehlsgruppe - compat 144
		{
			debug("nachricht-zk-compat", &zk_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// 3. Befehlsgruppe
	// WT - Schiff tanken
	// WS - Schiff aus Werft ausklinken
	// BI - Become Imp!
	// ZG - Imp: Zone enteignen
	// - Sie haben die Zone 1 auf Shakiro (1234) von Niemand gewaltsam in ihren Besitz genommen.
	static UVRegExp zg_o_re("^- Sie haben die Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) von (.+) gewaltsam in ihren Besitz genommen\\.$");
	// - Zone 3 auf Shakiro (1234) wurde von ihrem Imperator Tschu-Tschu gewaltsam enteignet.
	static UVRegExp zg_i_re("^- Zone ([0-9]+) auf (.*) \\(([0-9]+)\\) wurde von ihrem Imperator (.+) gewaltsam enteignet\\.$");
	// BN - Planet benennen
	// MB - Imp: Minen bauen
	// FB - Imp: Fabriken bauen
	// PV - Imp: Produktionsverhaeltnis aendern
	// GB - Imp: Geschuetztuerme bauen
	// EA - Imp: Energiegenerator ausbauen
	// SA - Imp: Spionageabwehr
	// BB - Imp: Basis bauen
	// - Sie haben auf Pimp (1234) eine Basis gekauft.
	static UVRegExp bb_re("^- Sie haben auf (.*?) \\(([0-9]+)\\) eine Basis gekauft\\.$");
	// HP - Imp: Basis-Prio
	// FK - Imp: Flotte kaufen
	// RB - Imp: Flotte ausbauen
	// RT - Imp: Flotte transfer
	// FA - Imp: Flotte abstossen
	// TK - Imp: Transporter kaufen
	// TL - Imp: Transporter beladen
	// TB - Imp: Transporter exakt beladen
	// TE - Imp: Transporter entladen
	// - Transporter 213 hat an der Handelsstation Erz und Klunker sein Erz für 12345 Cr verkauft.
	static UVRegExp te1_re("^- Transporter ([0-9]+) hat an der Handelsstation (.+?) sein Erz f.r ([0-9]+) Cr verkauft\\.$");
	// - Transporter 213 hat an der Basis 123 seine 50000 t Erz abgeladen.
	static UVRegExp te2_re("^- Transporter ([0-9]+) hat an der Basis ([0-9]+) seine ([0-9]+) t Erz abgeladen\\.$");
	// TA - Imp: Transporter abstossen
	// EM - Imp: Erzkoenig: Minen bauen
	// EM - Imp: Erzkoenig: Fabriken bauen
	// KF - Imp: Kaempfer: Freiwillige
	// KE - Imp: Kaempfer: Suizidtrupp
	// DR - Imp: Diplomat: Revolution
	// DC - Imp: Diplomat: Abwerben
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(zg_o_re.match(cur))
		{
			debug("nachricht-zg_o", &zg_o_re);
			// ***
		}
		else if(zg_i_re.match(cur))
		{
			debug("nachricht-zg_i", &zg_i_re);
			// ***
		}
		else if(bb_re.match(cur))
		{
			debug("nachricht-bb", &bb_re);
			// ***
		}
		else if(te1_re.match(cur))
		{
			debug("nachricht-te1", &te1_re);
			// ***
		}
		else if(te2_re.match(cur))
		{
			debug("nachricht-te2", &te2_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// 4. Befehlsgruppe
	// SW - Waffenstatus setzen
	// SO - Offensivbereich setzen
	// SF - Fluchtbereich setzen
	// SR - Traktorstrahl toggle
	// SU - Schiff scannen
	// - Ihr Schiff Yehaa wurde von der Bullenschiff (Weltraumbullen) überprüft.
	static UVRegExp su_i_re("^- Ihr Schiff (.+?) wurde von der (.*?) \\((.+?)\\) .berpr.ft\\.$");
	// SE - Schiff in anderes Schiff einladen
	// - Sie haben die S1 in die S2 (Doctor Who) eingeladen.
	static UVRegExp se_o_re("^- Sie haben die (.+?) in die (.+?) \\(.+?\\) eingeladen\\.$");
	// - Die S2 (Doedel) wurde in ihre S1 eingeladen.
	static UVRegExp se_i_re("^- Die (.+?) \\((.+?)\\) wurde in ihre (.+?) eingeladen\\.$");
	// SY - Schiff aus Schiff ausladen
	// - Die S1 wurde aus der S2 (Doctor Who) ausgeladen.
	static UVRegExp sy_re("^- Die (.+) wurde aus der (.+) \\((.+)\\) ausgeladen\\.$");
	// SD - Zone verteidigen
	// LA - Lager aufspalten
	// LB - Lager uebertragen
	// - Sie haben Lager 1 auf dem Schiff Habakuk an Susi übertragen.
	static UVRegExp lb_o_re("^- Sie haben Lager ([0-9]+) auf dem Schiff (.+?) an (.+) .bertragen\\.$");
	// - Hugo hat Ihnen Lager 1 auf dem Schiff Pipapo übertragen.
	static UVRegExp lb_i_re("^- (.+) hat Ihnen Lager ([0-9]+) auf dem Schiff (.+) .bertragen\\.$");
	// CA - Container abwerfen
	// - Lager 1 (1000 BRT Hanf) von Rudi Völler auf dem Schiff Radieschen wurde ins All gefeuert.
	static UVRegExp ca1_re("^- Lager ([0-9]+) \\(([0-9]+) BRT (.+?)\\) von (.*?) auf dem Schiff (.+) wurde ins All gefeuert\\.$");
	// - Lager 2 (2000 BRT Hanf) auf dem Schiff Tuckerkahn (Depp) wurde ins All gefeuert.
	static UVRegExp ca2_re("^- Lager ([0-9]+) \\(([0-9]+) BRT (.+?)\\) auf dem Schiff (.+) \\((.*?)\\) wurde ins All gefeuert\\.$");
	// CU - Container uebergeben
	// - Lager 1 (100 BRT Dattel) auf dem Schiff S1 (Susi) wurde auf das Schiff S1 (Hugo), Lager 1 übertragen.
	static UVRegExp cu1_re("^- Lager ([0-9]+) \\(([0-9]+) BRT (.+)\\) auf dem Schiff (.+) \\((.+)\\) wurde auf das Schiff (.+) \\((.+)\\), Lager ([0-9]+) .bertragen\\.$");
	// - Lager 1 auf ihrem Schiff Cargo2 wurde auf Schiff Wislwisl (Goh) übertragen.
	static UVRegExp cu2_re("^- Lager ([0-9]+) auf ihrem Schiff (.+) wurde auf Schiff (.+) \\((.+)\\) .bertragen\\.$");
	// - Von Schiff Xy (Hugo) wurde ein Lager von Susi auf ihr Schiff Zzz in Lager 1 übertragen.
	static UVRegExp cu3_re("^- Von Schiff (.+?) \\((.+?)\\) wurde ein Lager von (.+?) auf ihr Schiff (.+?) in Lager ([0-9]+) .bertragen\\.$");
	// CW - Lager anwenden
	// - Sie haben Lager 2 auf dem Schiff PiPaPo fürs Schiff benutzt.
	static UVRegExp cw1_re("^- Sie haben Lager ([0-9]+) auf dem Schiff (.+?) f.rs Schiff benutzt\\.$");
	// - Lager 2 auf dem Schiff PiPaPo wurde fürs Schiff benutzt.
	static UVRegExp cw2_re("^- Lager ([0-9]+) auf dem Schiff (.+?) wurde f.rs Schiff benutzt\\.$");
	// - Der Treibstoff wurde in den Tank abgefüllt.
	static UVRegExp cw3_re("^- Der Treibstoff wurde in den Tank abgef.llt\\.$");
	// - Der Tarngenerator wurde aktiviert, anscheinend baut er die Treibsubstanz mit 1 BRT pro Zentichron ab. Es scheint, als ob wir komplett unsichtbar für unsere eigene Sensoren - Phalanx sind, ob das wohl auch so bei fremden Schiffen ist? Die Waffen scheinen allerdings durch das Gerät gestört zu werden! Wir müssten also vor einem Angriff den Generator abschalten.
	static UVRegExp cw4_re("^- Der Tarngenerator wurde aktiviert, anscheinend baut er die Treibsubstanz mit 1 BRT pro Zentichron ab\\. Es scheint, als ob wir komplett unsichtbar f.r unsere eigene Sensoren - Phalanx sind, ob das wohl auch so bei fremden Schiffen ist\\? Die Waffen scheinen allerdings durch das Ger.t gest.rt zu werden! Wir m.ssten also vor einem Angriff den Generator abschalten\\.$");
	// - Die Boosterpacks verpufften im Tank und plötzlich war alles voller Treibstoff...
	static UVRegExp cw5_re("^- Die Boosterpacks verpufften im Tank und pl.tzlich war alles voller Treibstoff\\.\\.\\.$");
	// - Der Tarngenerator wurde deaktiviert.
	static UVRegExp cw6_re("^- Der Tarngenerator wurde deaktiviert\\.$");
	// MA - Auftrag annehmen
	// MF - Auftrag abschliessen
	// SG - Schiff uebertragen
	// Eigentlich in 10. Befehlsgruppe.
	// - Sie haben die HMS Crapper an Moechtegern übergeben.
	static UVRegExp sg_o_re("^- Sie haben die (.+?) an (.+?) .bergeben\\.$");
	// - Irgendwer hat Ihnen die Xyz übergeben.
	static UVRegExp sg_i_re("^- (.+?) hat Ihnen die (.+?) .bergeben\\.$");
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(su_i_re.match(cur))
		{
			debug("nachricht-su_i", &su_i_re);
			// ***
		}
		else if(se_o_re.match(cur))
		{
			debug("nachricht-se_o", &se_o_re);
			// ***
		}
		else if(se_i_re.match(cur))
		{
			debug("nachricht-se_i", &se_i_re);
			// ***
		}
		else if(sy_re.match(cur))
		{
			debug("nachricht-sy", &sy_re);
			// ***
		}
		else if(lb_o_re.match(cur))
		{
			debug("nachricht-lb_o", &lb_o_re);
			// ***
		}
		else if(lb_i_re.match(cur))
		{
			debug("nachricht-lb_i", &lb_i_re);
			// ***
		}
		else if(ca1_re.match(cur))
		{
			debug("nachricht-ca1", &ca1_re);
			// ***
		}
		else if(ca2_re.match(cur))
		{
			debug("nachricht-ca2", &ca2_re);
			// ***
		}
		else if(cu1_re.match(cur))
		{
			debug("nachricht-cu1", &cu1_re);
			// ***
		}
		else if(cu2_re.match(cur))
		{
			debug("nachricht-cu2", &cu2_re);
			// ***
		}
		else if(cu3_re.match(cur))
		{
			debug("nachricht-cu3", &cu3_re);
			// ***
		}
		else if(cw1_re.match(cur))
		{
			debug("nachricht-cw1", &cw1_re);
			// ***
		}
		else if(cw2_re.match(cur))
		{
			debug("nachricht-cw2", &cw2_re);
			// ***
		}
		else if(cw3_re.match(cur))
		{
			debug("nachricht-cw3", &cw3_re);
			// ***
		}
		else if(cw4_re.match(cur))
		{
			debug("nachricht-cw4", &cw4_re);
			// ***
		}
		else if(cw5_re.match(cur))
		{
			debug("nachricht-cw5", &cw5_re);
			// ***
		}
		else if(cw6_re.match(cur))
		{
			debug("nachricht-cw6", &cw6_re);
			// ***
		}
		else if(sg_o_re.match(cur))
		{
			debug("nachricht-sg_o", &sg_o_re);
			// ***
		}
		else if(sg_i_re.match(cur))
		{
			debug("nachricht-sg_i", &sg_i_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// 5. Befehlsgruppe
	// AH - Imp: Agent anheuern
	// KH - Imp: Kopfgeldjaeger anheuern
	// PH - Imp: Pirat anheuern
	// BP - Imp: Pirat pluendern
	// PE - Imp: Pirat entladen
	// BT - Imp: Pirat Terror
	// SB - Imp: Agent Sabotage
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else
		{
			break;
		}
		getline();
	}

// 6. Befehlsgruppe
	// BU - Erz von Basis auf Schiff laden
	// - Es wurde von Basis 84 12297 BRT Erz auf die Warfighter Void verladen.
	static UVRegExp bu_re("^- Es wurde von Basis ([0-9]+) ([0-9]+) BRT Erz auf die (.+) verladen\\.$");
	// OS - Speicherinhalt auf Schiff laden
	// - Sie haben 1000 BRT Kaffee auf die Huga transportiert.
	static UVRegExp os_o_re("^- Sie haben ([0-9]+) BRT (.+) auf die (.+) transportiert\\.$");
	// - Auf ihr Schiff Huga wurde 1000 BRT Kaffee von Hulahooper transportiert.
	static UVRegExp os_i_re("^- Auf ihr Schiff (.+) wurde ([0-9]+) BRT (.+) von (.*) transportiert\\.$");
	// OG - Schiffslager in Speicher abladen
	// - In ihren Speicher 1 auf Erde (1234) wurde durch Arschgeige 1234 BRT Handwaffen geladen.
	static UVRegExp og_i_re("^- In ihren Speicher ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde durch (.+?) ([0-9]+) BRT (.+?) geladen\\.$");
	// OV - Ware verkaufen
	// - Sie haben das Lager 1 (Tee, 1000 BRT) auf der Huga zu 64.935 Cr pro BRT für gesamthaft 12345678 Cr veräussert. Das Geld wurde zu Ihren Gunsten überwiesen.
	static UVRegExp ov1_re("^- Sie haben das Lager ([0-9]+) \\((.+), ([0-9]+) BRT\\) auf der (.+) zu ([0-9.]+) Cr pro BRT f.r gesamthaft ([0-9]+) Cr ver.ussert\\. Das Geld wurde zu Ihren Gunsten .berwiesen\\.$");
	// - Lager 4 (Kaffee, 1000 BRT) auf der Huga wurde zu 123.1234 Cr pro BRT für gesamthaft 123123 Cr veräussert. Das Geld wurde an Hulahooper überwiesen.
	static UVRegExp ov2_re("^- Lager ([0-9]+) \\((.+), ([0-9]+) BRT\\) auf der (.+) wurde zu ([0-9.]+) Cr pro BRT f.r gesamthaft ([0-9]+) Cr ver.ussert\\. Das Geld wurde an (.+) .berwiesen\\.$");
	// - Ein Lager (1234 BRT,Dattel) auf der Gugus (Dada) wurde zu Ihren Gunsten zu 11.23455 Cr pro BRT für gesamthaft 123456 Cr veräussert.
	static UVRegExp ov3_re("^- Ein Lager \\(([0-9]+) BRT,(.+?)\\) auf der (.+?) \\((.+?)\\) wurde zu Ihren Gunsten zu ([0-9.]+) Cr pro BRT f.r gesamthaft ([0-9]+) Cr ver.ussert\\.$");
	// - Lager 1 (Handwaffen, 10000 BRT) auf der SuperDuper konnte nicht komplett verkauft werden, die Handelsstation wollte nicht soviel ankaufen.
	static UVRegExp ov4_re("^- Lager ([0-9]+) \\((.+?), ([0-9]+) BRT\\) auf der (.+?) konnte nicht komplett verkauft werden, die Handelsstation wollte nicht soviel ankaufen\\.$");
	// OK - Ware kaufen
	// - Sie haben 100 BRT Erz zu 12.345 für 1234 Cr eingekauft.
	static UVRegExp ok1_re("^- Sie haben ([0-9]+) BRT Erz zu ([0-9.]+) f.r ([0-9]+) Cr eingekauft\\.$");
	// - Doctor Who hat 10000 BRT Kaffee auf die HMS Fuchur eingekauft.
	static UVRegExp ok2_re("^- (.+) hat ([0-9]+) BRT (.+) auf die (.+) eingekauft\\.$");
	// - Sie haben 10000 BRT Kaffee zu 2345.045 für 123445 Cr auf die HMS Fuchur eingekauft.
	static UVRegExp ok3_re("^- Sie haben ([0-9]+) BRT (.+) zu ([0-9.]+) f.r ([0-9]+) Cr auf die (.+) eingekauft\\.$");
	// - Produkt Colabaum auf der Handelsstation Gier und Plünder war nicht mehr in genügender Menge vorhanden.
	static UVRegExp ok4_re("^- Produkt (.+?) auf der Handelsstation (.+?) war nicht mehr in gen.gender Menge vorhanden\\.$");
	// SK - Schiff an HS tanken
	// PI - Planet beschreiben
	// ZI - Zone beschreiben
	// WI - Werft beschreiben
	// TI - Stadt beschreiben
	// FI - FS beschreiben
	// SI - Schiff beschreiben
	// CI - Charakter beschreiben
	// SL - Auswertung Spalten
	// FR - Auswertung Farbset
	// AY - Auswertung Typ
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(bu_re.match(cur))
		{
			debug("nachricht-bu", &bu_re);
			// ***
		}
		else if(os_o_re.match(cur))
		{
			debug("nachricht-os_o", &os_o_re);
			// ***
		}
		else if(os_i_re.match(cur))
		{
			debug("nachricht-os_i", &os_i_re);
			// ***
		}
		else if(og_i_re.match(cur))
		{
			debug("nachricht-og_i", &og_i_re);
			// ***
		}
		else if(ov1_re.match(cur))
		{
			debug("nachricht-ov1", &ov1_re);
			// ***
		}
		else if(ov2_re.match(cur))
		{
			debug("nachricht-ov2", &ov2_re);
			// ***
		}
		else if(ov3_re.match(cur))
		{
			debug("nachricht-ov3", &ov3_re);
			// ***
		}
		else if(ov4_re.match(cur))
		{
			debug("nachricht-ov4", &ov4_re);
			// ***
		}
		else if(ok1_re.match(cur))
		{
			debug("nachricht-ok1", &ok1_re);
			// ***
		}
		else if(ok2_re.match(cur))
		{
			debug("nachricht-ok2", &ok2_re);
			// ***
		}
		else if(ok3_re.match(cur))
		{
			debug("nachricht-ok3", &ok3_re);
			// ***
		}
		else if(ok4_re.match(cur))
		{
			debug("nachricht-ok4", &ok4_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// 7. Befehlsgruppe
	// AS - Artikel anmelden
	// AW - Artikel bewerten
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else
		{
			break;
		}
		getline();
	}

// 8. Befehlsgruppe Zentichron 1-100
	// SV - Schiff Speed
	// ST - Schiff Richtung
	// SP - Sprung
	// SH - Hyperdimensionssprung
	// VM - Verfolgungsmodus
	// IA - Infosonde absetzen
	// RA - Sensorsonde absetzen
	// Kampfbeginn
	// - Die Schiffe Shubiduu (Minime), Killerbiene (Q) beginnen einen Kampf in Zentichron 45. @144
	static UVRegExp kampf1_re("^- Die Schiffe .* beginnen einen Kampf in Zentichron [0-9]+\\.$");
	// - Zentichron 1: Die Schiffe Xy (Dumpfbacke), Blaster (Dagobert) beginnen einen Kampf.
	static UVRegExp kampf2_re("^- Zentichron [0-9]+: Die Schiffe .* beginnen einen Kampf\\.$");
	// Verfolgung
	// - Zentichron 13: Schiff Zzz nimmt Verfolgung der Xy (Dumpfbacke) auf.
	static UVRegExp verfolgung_on_re("^- Zentichron ([0-9]+): Schiff (.+?) nimmt Verfolgung der (.+?) \\((.+?)\\) auf\\.$");
	// - Zentichron 13: Schiff Zzz deaktiviert Verfolgungsmodus.
	static UVRegExp verfolgung_off1_re("^- Zentichron ([0-9]+): Schiff (.+?) deaktiviert Verfolgungsmodus\\.$");
	// - Zentichron 13: Schiff Zzz deaktiviert Verfolgungsmodus, Zielobjekt nicht gefunden.
	static UVRegExp verfolgung_off2_re("^- Zentichron ([0-9]+): Schiff (.+?) deaktiviert Verfolgungsmodus, Zielobjekt nicht gefunden\\.$");
	// - Zentichron 13: Schiff Zzz musste Verfolgung abbrechen. Zielobjekt existiert nicht mehr. Antrieb wird deaktiviert.
	static UVRegExp verfolgung_off3_re("^- Zentichron ([0-9]+): Schiff (.+?) musste Verfolgung abbrechen\\. Zielobjekt existiert nicht mehr\\. Antrieb wird deaktiviert\\.$");
	// Anomalien
	// - Zentichron 13: Schiff Zzz geriet in die Anziehungskraft einer kosmischen Anomalie. Das Schiff geriet in in einen Strudel und eine unbekannte Anziehungskraft die das Schiff zerstörten.
	static UVRegExp anomalie_re("^- Zentichron ([0-9]+): Schiff (.+?) geriet in die Anziehungskraft einer kosmischen Anomalie\\. Das Schiff geriet in in einen Strudel und eine unbekannte Anziehungskraft die das Schiff zerst.rten\\.$");
	// Container einladen
	// - Ein Container wurde vom Schiff Gigabyte eingeladen. Er enthielt 100000 BRT Militärlaser Klasse A.
	static UVRegExp container1_re("^- Ein Container wurde vom Schiff (.+) eingeladen\\. Er enthielt ([0-9]+) BRT (.+)\\.$");
	// - Es wurde versucht einen Container auf Schiff Gigabyte einzuladen. Er war allerdings zu gross.
	static UVRegExp container2_re("^- Es wurde versucht einen Container auf Schiff (.+) einzuladen\\. Er war allerdings zu gross\\.$");
	// Infosonde
	// - Sonde bei (-123,123,4) sendet Nachricht: ...
	static UVRegExp infosonde_re("^- Sonde bei \\((-?[0-9]+),(-?[0-9]+),([0-9])\\) sendet Nachricht: (.*)$");
	// Treibstoffmangel
	// - Zentichron 13: Schiff Xyz Triebwerk 1 musste aufgrund Treibstoffmangels abgestellt werden.
	static UVRegExp treibstoff_re("^- Zentichron ([0-9]+): Schiff (.+?) Triebwerk ([0-9]+) musste aufgrund Treibstoffmangels abgestellt werden\\.$");
	// Ausfall von Systemen
	// - Das Sensorensystem des Schiffes Xyz ist aufgrund zu schwachem Bordcomputer oder Energiegenerator nicht einsatzfähig.
	static UVRegExp sensoren_re("^- Das Sensorensystem des Schiffes (.+?) ist aufgrund zu schwachem Bordcomputer oder Energiegenerator nicht einsatzf.hig\\.$");
	// - Waffe XY ist aufgrund mangelnder Energiegenerator- oder Bordcomputerleistung nicht einsatzfähig.
	static UVRegExp waffen_re("^- (.+?) ist aufgrund mangelnder Energiegenerator- oder Bordcomputerleistung nicht einsatzf.hig\\.$");
	// Sondenzerstoerung
	// - Die Sensorsonde bei 1234,5678 wurde von der Hotzenplotz (Arschgeige) zerstört.
	static UVRegExp sensorsonde_i_re("^- Die Sensorsonde bei (-?[0-9]+),(-?[0-9]+) wurde von der (.+?) \\((.+)\\) zerst.rt\\.$");
	// - Die Hotzenplotz hat die Sensorsonde bei -123,123 von Doofie zerstört.
	static UVRegExp sensorsonde_o_re("^- Die (.+?) hat die Sensorsonde bei (-?[0-9]+),(-?[0-9]+) von (.+) zerst.rt\\.$");

	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
			getline();
		}
		else if(kampf1_re.match(cur) || kampf2_re.match(cur))
		{
			parse_kampfreport();
		}
		else if(verfolgung_on_re.match(cur))
		{
			debug("nachricht-verfolgung_on", &verfolgung_on_re);
			// ***
			getline();
		}
		else if(verfolgung_off1_re.match(cur))
		{
			debug("nachricht-verfolgung_off1", &verfolgung_off1_re);
			// ***
			getline();
		}
		else if(verfolgung_off2_re.match(cur))
		{
			debug("nachricht-verfolgung_off2", &verfolgung_off2_re);
			// ***
			getline();
		}
		else if(verfolgung_off3_re.match(cur))
		{
			debug("nachricht-verfolgung_off3", &verfolgung_off3_re);
			// ***
			getline();
		}
		else if(anomalie_re.match(cur))
		{
			debug("nachricht-anomalie", &anomalie_re);
			// ***
			getline();
		}
		else if(container1_re.match(cur))
		{
			debug("nachricht-container1", &container1_re);
			// ***
			getline();
		}
		else if(container2_re.match(cur))
		{
			debug("nachricht-container2", &container2_re);
			// ***
			getline();
		}
		else if(infosonde_re.match(cur))
		{
			debug("nachricht-infosonde", &infosonde_re);
			// ***
			getline();
		}
		else if(treibstoff_re.match(cur))
		{
			debug("nachricht-infosonde", &infosonde_re);
			// ***
			getline();
		}
		else if(sensoren_re.match(cur))
		{
			debug("nachricht-sensoren", &sensoren_re);
			// ***
			getline();
		}
		else if(waffen_re.match(cur))
		{
			debug("nachricht-waffen", &waffen_re);
			// ***
			getline();
		}
		else if(sensorsonde_i_re.match(cur))
		{
			debug("nachricht-sensorsonde_i", &sensorsonde_i_re);
			// ***
			getline();
		}
		else if(sensorsonde_o_re.match(cur))
		{
			debug("nachricht-sensorsonde_o", &sensorsonde_o_re);
			// ***
			getline();
		}
		else
		{
			break;
		}
	}

	// Orbit einklinken
	// - Schiff Blubb hat sich in den Orbit von Wuerg (1234) eingeklinkt.
	static UVRegExp orbit_re("^- Schiff (.+?) hat sich in den Orbit von (.*) \\(([0-9]+)\\) eingeklinkt\\.$");
	while(orbit_re.match(cur))
	{
		debug("nachricht-orbit", &orbit_re);
		// ***
		getline();
	}

// 9. Befehlsgruppe
	// SZ - Zone angreifen
	// - Die Zone 1 auf Teletubbieland (1234) wurde auftragsgemäss von der Xyz vernichtet.
	static UVRegExp sz_o1_re("^- Die Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde auftragsgem.ss von der (.+) vernichtet\\.$");
	// - Sie haben für die Zerstörung der Zone 1 auf Teletubbieland (1234) Kopfgeld in der Höhe von 0 Cr erhalten.
	static UVRegExp sz_o2_re("^- Sie haben f.r die Zerst.rung der Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) Kopfgeld in der H.he von ([0-9]+) Cr erhalten\\.$");
	// - Das Schiff Surfboard (Globi) wurde durch den Angriff auf die Werft Schiffbaue (1) von Teletubbieland (1234) zerstört.
	static UVRegExp sz_o3_re("^- Das Schiff (.+?) \\((.+?)\\) wurde durch den Angriff auf die Werft (.+?) \\(([0-9]+)\\) von (.*?) \\(([0-9]+)\\) zerst.rt\\.$");
	// - Zone 1 auf Pfupfike (1234) wurde von der Killerschiff (Feind TM) angegriffen und komplett zerstört.
	static UVRegExp sz_i1_re("^- Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde von der (.+?) \\((.+)\\) angegriffen und komplett zerstört\\.$");
	// - Das Schiff Liblingschiff wurde durch einen Angriff von Eeeevil auf die Werft Schiffbaue (1) von Pfupfike (1234) zerstört.
	static UVRegExp sz_i2_re("^- Das Schiff (.+?) wurde durch einen Angriff von (.+?) auf die Werft (.+?) \\(([0-9]+)\\) von (.*?) \\(([0-9]+)\\) zerst.rt\\.$");
	// - Sie haben für die Killerbiene 1000 Cr Kopfgeld kassiert. (DUP!)
	static UVRegExp kopfgeld_re("^- Sie haben f.r die (.+) ([0-9]+) Cr Kopfgeld kassiert\\.$");
	// PL - Zone pluendern
	// - Die Zone 1 auf Teletubbieland (1234) wurde auftragsgemäss von der Xyz geplündert.
	static UVRegExp pl_o_re("^- Die Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde auftragsgem.ss von der (.+) gepl.ndert\\.$");
	// - Zone 1 auf Teletubbieland (1234) wurde von der Katze (Eeevil) geplündert.
	static UVRegExp pl_i_re("^- Zone ([0-9]+) auf (.*?) \\(([0-9]+)\\) wurde von der (.+?) \\((.+)\\) gepl.ndert\\.$");
	// BV - Imp: Basis verschieben
	// FV - Imp: Flotte verschieben
	// TV - Imp: Transporter verschieben
	// CV - Imp: Charakter verschieben
	// HM - Imp: Haendler Extrasprung
	// CM - Imp: Charakter Extrasprung
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(sz_o1_re.match(cur))
		{
			debug("nachricht-sz_o1", &sz_o1_re);
			// ***
		}
		else if(sz_o2_re.match(cur))
		{
			debug("nachricht-sz_o2", &sz_o2_re);
			// ***
		}
		else if(sz_o3_re.match(cur))
		{
			debug("nachricht-sz_o3", &sz_o3_re);
			// ***
		}
		else if(sz_i1_re.match(cur))
		{
			debug("nachricht-sz_i1", &sz_i1_re);
			// ***
		}
		else if(sz_i2_re.match(cur))
		{
			debug("nachricht-sz_i2", &sz_i2_re);
			// ***
		}
		else if(kopfgeld_re.match(cur))
		{
			debug("nachricht-kopfgeld", &kopfgeld_re);
			// ***
		}
		else if(pl_o_re.match(cur))
		{
			debug("nachricht-pl_o", &pl_o_re);
			// ***
		}
		else if(pl_i_re.match(cur))
		{
			debug("nachricht-pl_i", &pl_i_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// ?
	// Imp: Charakterkampf
	// Imp: Flottenkampf
	// Imp: Planeten erobern
	// - Flotte 123 hat den Planeten  (1234) von Niemand erobert.
	static UVRegExp eroberung_re("^- Flotte ([0-9]+) hat den Planeten (.*?) \\(([0-9]+)\\) von (.+) erobert\\.$");
	// Imp: Erztribute
	// - Der Planet  (1234) ist an keine Basis angeschlossen.
	static UVRegExp erz_nobase_re("^- Der Planet (.*?) \\(([0-9]+)\\) ist an keine Basis angeschlossen\\.$");
	// - Der Transporter 1 hat 1234 t Erz vom Planeten  (1234) aufgeladen.
	static UVRegExp erz_transp_re("^- Der Transporter ([0-9]+) hat ([0-9]+) t Erz vom Planeten (.*?) \\(([0-9]+)\\) aufgeladen\\.$");
	// Imp: Revolutionen
	// - Auf Planet ASdfds (1234) hat sich die Lage beruhigt.
	static UVRegExp rev_ende_re("^- Auf Planet (.*?) \\(([0-9]+)\\) hat sich die Lage beruhigt\\.$");
	// - Die Stimmung der Bevölkerung auf Planet  (1234) ist ziemlich unruhig geworden.
	static UVRegExp rev_unruhig_re("^- Die Stimmung der Bev.lkerung auf Planet (.*?) \\(([0-9]+)\\) ist ziemlich unruhig geworden\\.$");
	// - Auf Planet  (1234) hat sich die Situation drastisch zugespitzt. Massnahmen wären dringend erforderlich. Beinahe wäre es zu einer Katastrophe gekommen.
	static UVRegExp rev_beinahe_re("^- Auf Planet (.*?) \\(([0-9]+)\\) hat sich die Situation drastisch zugespitzt\\. Massnahmen w.ren dringend erforderlich\\. Beinahe w.re es zu einer Katastrophe gekommen\\.$");
	// - Auf Planet  (1234) ist ein Revolution ausgebrochen!
	static UVRegExp rev_olution_re("^- Auf Planet (.*?) \\(([0-9]+)\\) ist ein Revolution ausgebrochen!$");

	while(dash_re.match(cur))
	{
		if(eroberung_re.match(cur))
		{
			debug("nachricht-eroberung", &eroberung_re);
			// ***
		}
		else if(erz_nobase_re.match(cur))
		{
			debug("nachricht-erz_nobase", &erz_nobase_re);
			// ***
		}
		else if(erz_transp_re.match(cur))
		{
			debug("nachricht-erz_transp", &erz_transp_re);
			// ***
		}
		else if(rev_ende_re.match(cur))
		{
			debug("nachricht-rev_ende", &rev_ende_re);
			// ***
		}
		else if(rev_unruhig_re.match(cur))
		{
			debug("nachricht-rev_unruhig", &rev_unruhig_re);
			// ***
		}
		else if(rev_beinahe_re.match(cur))
		{
			debug("nachricht-rev_beinahe", &rev_beinahe_re);
			// ***
		}
		else if(rev_olution_re.match(cur))
		{
			debug("nachricht-rev_olution", &rev_olution_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

	// Imp: Random-Gags
	// - Calgooros haben den Planeten  (1234) unsicher gemacht.
	static UVRegExp random1_re("^- Calgooros haben den Planeten (.*?) \\(([0-9]+)\\) unsicher gemacht\\.$");
	// - Schwere Stürme donnerten über den Planeten  (1234).
	static UVRegExp random2_re("^- Schwere Stürme donnerten über den Planeten (.*?) \\(([0-9]+)\\)\\.$");
	while(dash_re.match(cur))
	{
		if(random1_re.match(cur))
		{
			debug("nachricht-random1", &random1_re);
			// ***
		}
		else if(random2_re.match(cur))
		{
			debug("nachricht-random2", &random2_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}


// 10. Befehlsgruppe
	// DS - Imp: Pirat Digital Scan
	// DG - Imp: Diplomat Geld
	// SS - Imp: Agent Spionage
	// SG - Schiff uebertragen
	// Siehe 4. Befehlsgruppe.
	// PP - Station Preisliste
	// - Preisinfo 1234 / Handelsstation Mamma Mia - [...]
	static UVRegExp pp_re("^- Preisinfo ([0-9]+) / Handelsstation (.+) - (.*)$");
	// ZD - Zonendaten abfragen
	// - Zonedaten von Zone  ( (1234),1)= Besitzer: Niemand, Grösse: 123 FUs, -22.9 °C / -26.4 °C / -22.9 °C / -15.7 °C / -8.6 °C / -1.4 °C /  5.7 °C / -1.4 °C / -8.6 °C / -15.7 °C / -21.1 °C / -22.9 °C,  13 mm /  9 mm /  6 mm /  4 mm /  6 mm /  9 mm /  4 mm /  5 mm /  6 mm /  3 mm /  6 mm /  9 mm, DT:-13.5 °C / GN: 80 mm
	static UVRegExp zd_re("^- Zonedaten von Zone (.*) \\((.*) \\(([0-9]+)\\),([0-9]+)\\)= Besitzer: (.+), Gr.sse: ([0-9]+) FUs,  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C /  ?([0-9.-]+) °C,  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm /  ([0-9]+) mm, DT: ?([0-9.-]+) °C / GN: ([0-9]+) mm$");
	while(dash_re.match(cur))
	{
		if(cur[2] == '!')
		{
			// *** Fehlermeldung skipped
		}
		else if(sg_o_re.match(cur))
		{
			debug("nachricht-sg_o", &sg_o_re);
			// ***
		}
		else if(sg_i_re.match(cur))
		{
			debug("nachricht-sg_i", &sg_i_re);
			// ***
		}
		else if(pp_re.match(cur))
		{
			debug("nachricht-pp", &pp_re);
			// ***
		}
		else if(zd_re.match(cur))
		{
			debug("nachricht-zd", &zd_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

// ?
	// Ernte/Ertrag
	// - Die Ernte der Zone Gaga (Planet Gugu (1234), Zone 4, Agrarfeld 5) konnte nicht komplett gelagert werden und es gingen 1000 BRT Kaffee verloren.
	static UVRegExp ernte_re("^- Die Ernte der Zone (.*) \\(Planet (.*) \\(([0-9]+)\\), Zone ([0-9]+), Agrarfeld ([0-9]+)\\) konnte nicht komplett gelagert werden und es gingen ([0-9]+) BRT (.+) verloren\\.$");
	// - Der Ertrag der Zone  (Planet Gaga (1234), Zone 2, Mine 6) konnte nicht komplett gelagert werden und es gingen 1000 BRT Kristall verloren.
	static UVRegExp ertrag_re("^- Der Ertrag der Zone (.*) \\(Planet (.*) \\(([0-9]+)\\), Zone ([0-9]+), Mine ([0-9]+)\\) konnte nicht komplett gelagert werden und es gingen ([0-9]+) BRT (.+) verloren\\.$");
	// - Die Ernte der Zone  (Planet Gugu (1234), Zone 4, Agrarfeld 5) konnte nicht eingebracht werden, da sie als Imperator keine Zeit haben, sich darum zu kümmern.
	static UVRegExp ernte_imp_re("^- Die Ernte der Zone (.*) \\(Planet (.*) \\(([0-9]+)\\), Zone ([0-9]+), Agrarfeld ([0-9]+)\\) konnte nicht eingebracht werden, da sie als Imperator keine Zeit haben, sich darum zu k.mmern\\.$");
	// - Die Minenarbeit der Zone Michikaze6 (Planet Yggdrasil (4559), Zone 4, Mine 2) konnte nicht fortgesetzt werden, da sie als Imperator keine Zeit haben, sich darum zu kümmern.
	static UVRegExp ertrag_imp_re("^- Die Minenarbeit der Zone (.*) \\(Planet (.*) \\(([0-9]+)\\), Zone ([0-9]+), Mine ([0-9]+)\\) konnte nicht fortgesetzt werden, da sie als Imperator keine Zeit haben, sich darum zu k.mmern.$");
	while(dash_re.match(cur))
	{
		if(ernte_re.match(cur))
		{
			debug("nachricht-ernte", &ernte_re);
			// ***
		}
		else if(ertrag_re.match(cur))
		{
			debug("nachricht-ertrag", &ertrag_re);
			// ***
		}
		else if(ernte_imp_re.match(cur))
		{
			debug("nachricht-ernte_imp", &ernte_imp_re);
			// ***
		}
		else if(ertrag_imp_re.match(cur))
		{
			debug("nachricht-ertrag_imp", &ertrag_imp_re);
			// ***
		}
		else
		{
			break;
		}
		getline();
	}

	// Stadt-Steuern
	// - Die Stadt Ruzzfos auf Gwandibuz (1234) hat 1234567 Steuern eingebracht.
	static UVRegExp steuern_re("^- Die Stadt (.+?) auf (.*?) \\(([0-9]+)\\) hat ([0-9]+) Steuern eingebracht\\.$");
	while(steuern_re.match(cur))
	{
		debug("nachricht-steuern", &steuern_re);
		// ***
		getline();
	}
	// - Die Stadt Ruzzfos auf Gwandibuz (1234) verweigerte ihnen die Steuern in Höhe von 123456. Ich fürchte sie werden einen Freischaffenden als Verwalter einstellen, der die Besteuerung übernimmt.
	static UVRegExp steuern_imp_re("^- Die Stadt (.+?) auf (.*?) \\(([0-9]+)\\) verweigerte ihnen die Steuern in H.he von ([0-9]+)\\. Ich f.rchte sie werden einen Freischaffenden als Verwalter einstellen, der die Besteuerung .bernimmt\\.$");
	while(steuern_imp_re.match(cur))
	{
		debug("nachricht-steuern_imp", &steuern_imp_re);
		// ***
		getline();
	}
	// Arbeitslosengeld
	// - Da sie dieses Jahr kein Einkommen hatten, wurde ihnen das Arbeitslosengeld/IV (1000 Cr) ausbezahlt.
	static UVRegExp income_re("^- Da sie dieses Jahr kein Einkommen hatten, wurde ihnen das Arbeitslosengeld/IV \\(([0-9]+) Cr\\) ausbezahlt\\.$");
	if(income_re.match(cur))
	{
		debug("nachricht-income", &income_re);
		// ***
		getline();
	}
	// Tod
	// - Du bist leider tot! Du kannst, wenn Du möchtest jederzeit mit einem neuen Charakter wieder einsteigen. Danke fürs Mitspielen!
	static UVRegExp tod_re("^- Du bist leider tot! Du kannst, wenn Du m.chtest jederzeit mit einem neuen Charakter wieder einsteigen\\. Danke f.rs Mitspielen!$");
	if(tod_re.match(cur))
	{
		debug("nachricht-tod", &tod_re);
		// ***
		getline();
	}
	// Willkommen
	// - Willkommen im Universum! Im Moment bist Du noch nirgends im Universum angesiedelt. Du kannst nun bei einer Werft einen Schiffsbau aufgeben (informier Dich vorher über ihre Preise). Damit hast Du Dich dann auch für einen Startort entschieden und das Abenteuer kann beginnen! (Werftsliste im aktuellen Chronator / siehe Homepage)
	static UVRegExp willkommen_re("^- Willkommen im Universum! Im Moment bist Du noch nirgends im Universum angesiedelt\\. Du kannst nun bei einer Werft einen Schiffsbau aufgeben \\(informier Dich vorher .ber ihre Preise\\)\\. Damit hast Du Dich dann auch f.r einen Startort entschieden und das Abenteuer kann beginnen! \\(Werftsliste im aktuellen Chronator / siehe Homepage\\)$");
	if(willkommen_re.match(cur))
	{
		debug("nachricht-willkommen", &willkommen_re);
		// ***
		getline();
	}


	if(dash_re.match(cur))
	{
		throw EXCEPTION("Unbekannter Nachrichtentyp!");
	}
}


/*
 * Ein Kampfreport aus den Nachrichten parsen.
 */
void UVParserTXT::parse_kampfreport()
{
	static UVRegExp dash_re("^- ");

	// - Die Schiffe Shubiduu (Minime), Killerbiene (Q) beginnen einen Kampf in Zentichron 45. @144
	static UVRegExp start1a_re("^- Die Schiffe (.+?) \\((.+?)\\)(?=(?:, | beginnen einen Kampf in Zentichron [0-9]+\\.$))");
	static UVRegExp start1b_re("^, (.+?) \\((.+?)\\)(?=(?:, | beginnen einen Kampf in Zentichron [0-9]+\\.$))");
	static UVRegExp start1c_re("^ beginnen einen Kampf in Zentichron ([0-9]+)\\.$");
	// - Zentichron 13: Die Schiffe Xy (Dumpfbacke), Blaster (Dagobert) beginnen einen Kampf.
	static UVRegExp start2a_re("^- Zentichron ([0-9]+): Die Schiffe (.+?) \\((.+?)\\)(?=(?:, | beginnen einen Kampf\\.$))");
	static UVRegExp start2b_re("^, (.+?) \\((.+?)\\)(?=(?:, | beginnen einen Kampf\\.$))");
	static UVRegExp start2c_re("^ beginnen einen Kampf\\.$");

	if(start1a_re.match(cur))
	{
		debug("nachricht-kampf-start1a", &start1a_re);
		// ***
		shiftline(&start1a_re);
		while(start1b_re.match(cur))
		{
			debug("nachricht-kampf-start1b", &start1b_re);
			// ***
			shiftline(&start1b_re);
		}
		if(!start1c_re.match(cur))
		{
			throw EXCEPTION("Fehler in Kampfreport!");
		}
		debug("nachricht-kampf-start1c", &start1c_re);
		// ***
	}
	else if(start2a_re.match(cur))
	{
		debug("nachricht-kampf-start2a", &start2a_re);
		// ***
		shiftline(&start2a_re);
		while(start2b_re.match(cur))
		{
			debug("nachricht-kampf-start2b", &start2b_re);
			// ***
			shiftline(&start2b_re);
		}
		if(!start2c_re.match(cur))
		{
			throw EXCEPTION("Fehler in Kampfreport!");
		}
		debug("nachricht-kampf-start2c", &start2c_re);
		// ***
	}
	else
	{
		throw EXCEPTION("Fehler in Kampfreport!");
	}
	getline();

	// - Die Sensoren konnten leider keine verwertbaren Daten über die Xy empfangen.
	static UVRegExp noint_re("^- Die Sensoren konnten leider keine verwertbaren Daten .ber die (.+) empfangen\\.$");
	// - Die Xy hat eine geschätzte Schadenstauglichkeit von 123 HP, wovon Schild und Panzer ca. 23 % Prozent ausmachen. _
	// - Die Xy hat eine geschätzte Schadenstauglichkeit von 123 HP, der Anteil von Schild und Panzer ist aber nicht bekannt. _
	// - Die Schadenstauglichkeit sowie Schutzmassnahmen der Xy sind nicht erkennbar. _
	static UVRegExp int_defense1_re("^- Die (.+?) hat eine gesch.tzte Schadenstauglichkeit von ([0-9]+) HP, wovon Schild und Panzer ca\\. ([0-9]+) % Prozent ausmachen\\. ");
	static UVRegExp int_defense2_re("^- Die (.+?) hat eine gesch.tzte Schadenstauglichkeit von ([0-9]+) HP, der Anteil von Schild und Panzer ist aber nicht bekannt\\. ");
	static UVRegExp int_defense3_re("^- Die Schadenstauglichkeit sowie Schutzmassnahmen der (.+?) sind nicht erkennbar\\. ");
	//   Ihre Zerstörungskraft umfasst ca. 5 Punkte Schaden pro Salve. _
	//   Ihre Zerstörungskraft ist unbekannt. _
	static UVRegExp int_offense1_re("^Ihre Zerst.rungskraft umfasst ca\\. ([0-9]+) Punkte Schaden pro Salve\\. ");
	static UVRegExp int_offense2_re("^Ihre Zerst.rungskraft ist unbekannt\\. ");
	//   Die Fluchtgeschwindigkeit ist bei 5.6 mal Lichtgeschwindigkeit anzusetzen.
	//   Die Fluchtgeschwindigkeit kann nicht ermittelt werden.
	static UVRegExp int_flucht1_re("^Die Fluchtgeschwindigkeit ist bei ([0-9.]+) mal Lichtgeschwindigkeit anzusetzen\\.$");
	static UVRegExp int_flucht2_re("^Die Fluchtgeschwindigkeit kann nicht ermittelt werden\\.$");
	static UVRegExp int_re("^- Die .*Schadenstauglichkeit .*\\. Ihre Zerst.rungskraft .*\\. Die Fluchtgeschwindigkeit .*\\.$");

	// - Detonationen bei Raketegeschütz auf der Xy durch Treffer von der Xyz.
	// - Detonationen bei Kanone auf der Xy durch Treffer von der Xyz.
	static UVRegExp det_re("^- Detonationen bei (.+) auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Raketenlafette (3fach) der Xy durch Treffer von der Xyz zerstört.
	static UVRegExp zer_re("^- (.+?) der (.+?) durch Treffer von der (.+?) zerst.rt\\.$");
	// - Schwere Detonationen auf der Xy durch Treffer von der Xyz.
	static UVRegExp sdet_re("^- Schwere Detonationen auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Zusammenbruch des Energiesystems auf der Xy durch Treffer von der Xyz.
	static UVRegExp energie_re("^- Zusammenbruch des Energiesystems auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Schildzusammenbruch auf der Xy durch Treffer von der Xyz.
	static UVRegExp schild_re("^- Schildzusammenbruch auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Feuer im Lagersektor auf der Xy durch Treffer von der Xyz.
	static UVRegExp lagerfeuer_re("^- Feuer im Lagersektor auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Lager 1 (Uran) der Xy durch Treffer von der Xyz zerstört.
	static UVRegExp lager_re("^- Lager ([0-9]+) \\((.+?)\\) der (.+?) durch Treffer von der (.+) zerst.rt\\.$");
	// - Ausfall des Sensorbereichs auf der Xy durch Treffer von der Xyz.
	static UVRegExp sensor_re("^- Ausfall des Sensorbereichs auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Zusammenbruch der Schiffskommunikation auf der Xy durch Treffer von der Xyz.
	static UVRegExp komm_re("^- Zusammenbruch der Schiffskommunikation auf der (.+?) durch Treffer von der (.+?)\\.$");
	// - Schwere Explosion bei den Treibstofftanks der Xy durch Treffer von der Xyz.
	static UVRegExp treibstoff_re("^- Schwere Explosion bei den Treibstofftanks der (.+?) durch Treffer von der (.+?)\\.$");
	// - Explosion von Triebwerk auf der Xy durch Treffer von der Xyz.
	static UVRegExp triebwerk_re("^- Explosion von Triebwerk auf der (.+?) durch Treffer von der (.+?)\\.$");

	// - Die Hypertrans ist aus dem Kampf geflüchtet.
	static UVRegExp flucht_re("^- Die (.+) ist aus dem Kampf gefl.chtet\\.$");
	// - Schiff Xy wurde von einem Kommandotrupp der Xyz geentert und übernommen.
	static UVRegExp enter_re("^- Schiff (.+?) wurde von einem Kommandotrupp der (.+?) geentert und .bernommen\\.$");
	// - Die Killerbiene ist explodiert.
	static UVRegExp explosion_re("^- Die (.+) ist explodiert\\.$");
	// - Sie haben für die Killerbiene 1000 Cr Kopfgeld kassiert. (DUP!)
	static UVRegExp kopfgeld_re("^- Sie haben f.r die (.+) ([0-9]+) Cr Kopfgeld kassiert\\.$");
	// - Der Kampf wurde beendet, als die beteiligten Schiffe erkannten, dass sie sich aufgrund zu schwacher Waffen gegenseitig nicht beschädigen konnten.
	static UVRegExp patt_re("^- Der Kampf wurde beendet, als die beteiligten Schiffe erkannten, dass sie sich aufgrund zu schwacher Waffen gegenseitig nicht besch.digen konnten\\.$");

	while(dash_re.match(cur))
	{
		if(noint_re.match(cur))
		{
			debug("nachricht-kampf-noint", &noint_re);
			// ***
		}
		else if(int_re.match(cur))
		{
			if(int_defense1_re.match(cur))
			{
				debug("nachricht-kampf-int_defense1", &int_defense1_re);
				// ***
				shiftline(&int_defense1_re);
			}
			else if(int_defense2_re.match(cur))
			{
				debug("nachricht-kampf-int_defense2", &int_defense2_re);
				// ***
				shiftline(&int_defense2_re);
			}
			else if(int_defense3_re.match(cur))
			{
				debug("nachricht-kampf-int_defense3", &int_defense3_re);
				// ***
				shiftline(&int_defense3_re);
			}
			else
			{
				throw EXCEPTION("Fehler in Kampfreport!");
			}

			if(int_offense1_re.match(cur))
			{
				debug("nachricht-kampf-int_offense1", &int_offense1_re);
				// ***
				shiftline(&int_offense1_re);
			}
			else if(int_offense2_re.match(cur))
			{
				debug("nachricht-kampf-int_offense2", &int_offense2_re);
				// ***
				shiftline(&int_offense2_re);
			}
			else
			{
				throw EXCEPTION("Fehler in Kampfreport!");
			}

			if(int_flucht1_re.match(cur))
			{
				debug("nachricht-kampf-int_flucht1", &int_flucht1_re);
				// ***
			}
			else if(int_flucht2_re.match(cur))
			{
				debug("nachricht-kampf-int_flucht2", &int_flucht2_re);
				// ***
			}
			else
			{
				throw EXCEPTION("Fehler in Kampfreport!");
			}
		}
		else if(det_re.match(cur))
		{
			debug("nachricht-kampf-det", &det_re);
			// ***
		}
		else if(zer_re.match(cur))
		{
			debug("nachricht-kampf-zer", &zer_re);
			// ***
		}
		else if(sdet_re.match(cur))
		{
			debug("nachricht-kampf-sdet", &sdet_re);
			// ***
		}
		else if(energie_re.match(cur))
		{
			debug("nachricht-kampf-energie", &energie_re);
			// ***
		}
		else if(schild_re.match(cur))
		{
			debug("nachricht-kampf-schild", &schild_re);
			// ***
		}
		else if(lagerfeuer_re.match(cur))
		{
			debug("nachricht-kampf-lagerfeuer", &lagerfeuer_re);
			// ***
		}
		else if(lager_re.match(cur))
		{
			debug("nachricht-kampf-lager", &lager_re);
			// ***
		}
		else if(sensor_re.match(cur))
		{
			debug("nachricht-kampf-sensor", &sensor_re);
			// ***
		}
		else if(komm_re.match(cur))
		{
			debug("nachricht-kampf-komm", &komm_re);
			// ***
		}
		else if(treibstoff_re.match(cur))
		{
			debug("nachricht-kampf-treibstoff", &treibstoff_re);
			// ***
		}
		else if(triebwerk_re.match(cur))
		{
			debug("nachricht-kampf-triebwerk", &triebwerk_re);
			// ***
		}
		else if(flucht_re.match(cur))
		{
			debug("nachricht-kampf-flucht", &flucht_re);
			// ***
		}
		else if(enter_re.match(cur))
		{
			debug("nachricht-kampf-enter", &enter_re);
			// ***
		}
		else if(explosion_re.match(cur))
		{
			debug("nachricht-kampf-explosion", &explosion_re);
			// ***
		}
		else if(kopfgeld_re.match(cur))
		{
			debug("nachricht-kampf-kopfgeld", &kopfgeld_re);
			// ***
		}
		else if(patt_re.match(cur))
		{
			debug("nachricht-kampf-patt", &patt_re);
			// ***
			getline();
			break;
		}
		else
		{
			break;
		}
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
	static UVRegExp cpu_re("^  (?!<[0-9]+\\. )(?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein Bordcomputer)), (?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein Energiegenerator))(?:, (.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT)?$");
	if(cpu_re.match(cur))
	{
		debug("schiff-bordcomputer", &cpu_re);
		if(cpu_re.sub(5) != "Kein Bordcomputer")
		{
			s->bordcomputer = new UVKomponente(cpu_re.sub(1),
							cpu_re.subtol(2),
							cpu_re.subtol(3),
							cpu_re.subtol(4));
		}
		if(cpu_re.sub(10) != "Kein Energiegenerator")
		{
			s->energiegenerator = new UVKomponente(cpu_re.sub(6),
							cpu_re.subtol(7),
							cpu_re.subtol(8),
							cpu_re.subtol(9));
		}
		if((cpu_re.subs() > 10) && (cpu_re.sub(11) != ""))
		{
			s->sensoren = new UVKomponente(cpu_re.sub(11),
							cpu_re.subtol(12),
							cpu_re.subtol(13),
							cpu_re.subtol(14));
			s->sichtweite = get_sichtweite(s->sensoren->name);
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
	stats_schiffe++;
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

	if(V_INFO)
	{
		cout << "  " << p->to_string_terse() << endl;
	}

	welt->set_planet(p);
	stats_planeten++;

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
	stats_planeten++;

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

	if(abs((long int)(z->N - z->get_N())) > 0.11)
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (N:" << z->N << "!=" << z->get_N() << ")" << endl;
	}
	if(abs((long int)(z->T - z->get_T())) > 0.11)
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (T:" << z->T << "!=" << z->get_T() << ")" << endl;
	}

	if(V_INFO)
	{
		cout << "    " << z->to_string_terse() << endl;
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
	stats_schiffe++;
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
 * Findet die Sichtweite zu einem Sensorentyp.
 */
long UVParserTXT::get_sichtweite(const string& s) const
{
	if(s == "ScanCipher 1a")
	{
		return 1000;
	}
	else if(s == "ScanCipher 3e")
	{
		return 2000;
	}
	else if(s == "SensoStar II")
	{
		return 4000;
	}
	else if(s == "InfoReceiver 3D")
	{
		return 6000;
	}
	else if(s == "Starvision II")
	{
		return 9000;
	}
	else if(s == "Starvision III")
	{
		return 12000;
	}
	else if(s == "Detector 32cI")
	{
		return 15000;
	}
	else if(s == "Detector Ultimate")
	{
		return 20000;
	}
	else if(s == "DigitalScan 2")
	{
		return 30000;
	}
	else if(s == "Anti-Disguise EL")
	{
		return 50000;
	}
	else if(s == "LeechSpector")
	{
		return 100000;
	}
	else if(s == "CQ-Doomsday Ed.")
	{
		return 200000;
	}
	else
	{
		throw EXCEPTION("Unbekannter Sensorentyp!");
	}
}


/*
 * Den letztem Regexp-Match zum Debuggen ausgeben.
 */
#ifdef DEBUG
void UVParserTXT::parse_debug(const string& s, UVRegExp* re) const
{
	if(V_RE)
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
	return text + " in " + src_func + "() at " + src_file + ":"
	            + to_string(src_line) + " while processing:\n"
	            + to_string(line) + ":" + to_string(cur);
}

