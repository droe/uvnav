# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/copyleft/
#
# $Id$

bin_PROGRAMS = uvnav

uvnav_SOURCES = rev.h rev.cpp \
	util/version.h \
	util/stl_hash_map.h util/stl_string.h \
	util/exceptions.h \
	util/sysexits.h \
	util/sysdep.h util/sysdep.cpp \
	util/observer.h util/observer.cpp \
	util/singleton.h \
	util/minmax.h \
	util/regexp.h util/regexp.cpp \
	util/sdl.h \
	util/ac_config.h \
	si/conf.h si/conf.cpp \
	si/draw.h si/draw.cpp \
	si/events.h si/events.cpp \
	si/font.h si/font.cpp \
	si/fonthandler.h si/fonthandler.cpp \
	si/imagehandler.h si/imagehandler.cpp \
	si/video.h si/video.cpp \
	pd/spieler.h pd/spieler.cpp \
	pd/anomalie.h pd/anomalie.cpp \
	pd/container.h pd/container.cpp \
	pd/infosonde.h pd/infosonde.cpp \
	pd/sensorsonde.h pd/sensorsonde.cpp \
	pd/agrarfeld.h pd/agrarfeld.cpp \
	pd/speicherfeld.h pd/speicherfeld.cpp \
	pd/minenfeld.h pd/minenfeld.cpp \
	pd/werft.h pd/werft.cpp \
	pd/forschungsstation.h pd/forschungsstation.cpp \
	pd/stadt.h pd/stadt.cpp \
	pd/zone.h pd/zone.cpp \
	pd/handelsstation.h pd/handelsstation.cpp \
	pd/planet.h pd/planet.cpp \
	pd/ladung.h pd/ladung.cpp \
	pd/komponente.h pd/komponente.cpp \
	pd/schiff.h pd/schiff.cpp \
	pd/universum.h pd/universum.cpp \
	dm/abstractimporter.h \
	dm/import_txt.h dm/import_txt.cpp \
	dm/importhandler.h dm/importhandler.cpp \
	ui/core/orientation.h \
	ui/core/halign.h ui/core/valign.h \
	ui/core/size.h ui/core/size.cpp \
	ui/core/rect.h ui/core/rect.cpp \
	ui/core/widget.h ui/core/widget.cpp \
	ui/core/compositewidget.h ui/core/compositewidget.cpp \
	ui/core/window.h ui/core/window.cpp \
	ui/core/rootwindow.h ui/core/rootwindow.cpp \
	ui/core/label.h ui/core/label.cpp \
	ui/map.h ui/map.cpp \
	ui/progress.h ui/progress.cpp \
	ui/navigator.h ui/navigator.cpp \
	main.cpp

uvnav_CXXFLAGS = @PCRE_CFLAGS@ @X_CFLAGS@ @SDL_CFLAGS@ @CXXFLAGS_WARNINGS@
uvnav_LDADD = @PCRE_LIBS@ @SDL_LIBS@ @X_LIBS@

MAINTAINERCLEANFILES = rev.h .rev-str Makefile.in

#
# rev.h Hack -- Subversion Revision im Code einbetten.
#
# Bei jedem Build wird als erstes .build-rev.h aufgerufen.
# Das .build-rev.h Target ueberprueft die Revision des Source-Trees,
# und aktualisiert rev.h nur falls noetig.
#
# rev.h:     Fertiger Header. Wird nur veraendert falls effektiv noetig,
#            damit wir nicht unnoetige Rebuilds ausloesen.
# .rev:      Zuletzt gesehene echte Revision (d.h. nicht "exported").
# .rev-str:  Zuletzt fuer die Generierung von rev.h benutzter String.
#
# Falls vorhanden wird mit svnversion die aktuelle Revision ermittelt.
# Falls svnversion nicht vorhanden ist, wird "exported" verwendet.
#
# Falls eine echte Revision gefunden wurde, wird rev.h mit dieser
# aktualisiert, falls sie sich von .rev unterscheidet.
#
# Falls keine echte Revision gefunden wurde, wird rev.h mit
# "exported from " und der letzten gesehenen Revision (.rev)
# aktualisiert, falls sich dieser String von .rev-str unterscheidet.
#
# Falls rev.h aktualisiert wurde, wird der benutzte String in .rev-str
# abgelegt.
#
# Falls eine echte Revision gefunden und benutzt wurde, wird diese in
# .rev abgelegt.
#

# Damit das sauber funktioniert, muessen im top-level Makefile.am
# die sowohl regenerierten als auch distributeten Files von distcleancheck
# ausgenommen werden:
#distcleancheck_listfiles = \
#    find . -type f -print | grep -v 'rev\.h' | grep -v '\.rev'

BUILT_SOURCES = .build-rev.h
EXTRA_DIST = .rev .rev-str

rev.h:

.rev: .build-rev.h

.rev-str: .build-rev.h

.build-rev.h:
	@echo Updating rev.h... ; \
	echo -n "pwd=" ; pwd ; \
	touch .rev .rev-str ; \
	oldrev=`cat .rev 2>/dev/null` || true ; \
	oldstr=`cat .rev-str 2>/dev/null` || true ; \
	echo "oldrev=$${oldrev} oldstr=$${oldstr}" ; \
	if [ "x$(SVNVERSION)" != "x" -a -x "$(SVNVERSION)" ]; then \
		newrev=`$(SVNVERSION) .` ; \
		echo "Output of svnversion: $${newrev}" ; \
	else \
		newrev="exported" ; \
		echo "No svnversion available, falling back to: exported" ; \
	fi ; \
	if [ "x$${newrev}" = "xexported" ]; then \
		if [ "x$${oldrev}" = "x" ]; then \
			newstr="exported" ; \
		else \
			newstr="exported from r$${oldrev}" ; \
		fi ; \
	else \
		newstr="r$${newrev}" ; \
		echo "$${newrev}" > .rev ; \
	fi ; \
	echo "Revision is: $${newstr}" ; \
	if [ "x$${newstr}" != "x$${oldstr}" ]; then \
		echo "/*" > rev.h ; \
		echo " * Diese Datei wird von make automatisch generiert." >> rev.h ; \
		echo " * Veraenderungen koennen in src/Makefile.am gemacht werden." >> rev.h ; \
		echo " */" >> rev.h ; \
		echo "#ifdef REVISION" >> rev.h ; \
		echo "#undef REVISION" >> rev.h ; \
		echo "#endif" >> rev.h ; \
		echo "#define REVISION \"$${newstr}\"" >> rev.h ; \
		echo "rev.h brought up to date" ; \
		echo "$${newstr}" > .rev-str ; \
	else \
		echo "rev.h is up to date - not changed" ; \
	fi
