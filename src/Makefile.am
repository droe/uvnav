# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004 Daniel Roethlisberger <roe@chronator.ch>
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

uvnav_SOURCES = rev.h global.h stl.h debug.h sysdep.h sysdep.cpp \
	conf.h conf.cpp images.h images.cpp font.h font.cpp draw.h draw.cpp \
	progress.h progress.cpp welt.h welt.cpp parser_txt.h parser_txt.cpp \
	map.h map.cpp gui.h gui.cpp navigator.h navigator.cpp main.cpp

uvnav_CXXFLAGS = @X_CFLAGS@ @SDL_CFLAGS@ @PCREPP_CFLAGS@
uvnav_LDFLAGS = @X_LIBS@ @SDL_LIBS@ @PCREPP_LIBS@

MAINTAINERCLEANFILES = rev.h .rev .rev-str Makefile.in

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
	touch .rev .rev-str ; \
	oldrev=`cat .rev 2>/dev/null` || true ; \
	oldstr=`cat .rev-str 2>/dev/null` || true ; \
	if [ "x$(SVNVERSION)" != "x" -a -x "$(SVNVERSION)" ]; then \
		newrev=`$(SVNVERSION) .` ; \
		echo "Output of svnversion: $${newrev}" ; \
	else \
		newrev="exported" ; \
		echo "No svnversion available, falling back to: exported" ; \
	fi ; \
	if [ "x$${newrev}" != "x$${oldrev}" ]; then \
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
		fi ; \
	else \
		echo "rev.h is up to date - not changed" ; \
	fi
