#!/bin/sh
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

#
# autogen.sh - (Re)Generiert die ganze Autotools-Infrastruktur.
#
# Die wichtigsten Zusammenhänge:
#
#   configure.ac +
#   acinclude.m4 ----[aclocal]---> aclocal.m4
#
#   configure.ac --[autoheader]--> config.h.in
#
#   aclocal.m4   +
#   configure.ac ---[autoconf]---> configure
#
#   configure.ac +
#   Makefile.am  ---[automake]---> Makefile.in
#
#   config.h.in  ---[configure]--> config.h
#   Makefile.in  ---[configure]--> Makefile
#
# automake --add-missing generiert ausserdem eine Reihe von Helper-Scripts
# und weitere Ueberbleibsel:
# config.guess, config.sub, depcomp, install-sh, missing, mkinstalldirs,
# autom4te.cache/
#
# configure generiert ebenso einige Logfiles, Helper-Scripts und andere
# Ueberbleibsel:
# config.log, config.status, stamp-h1
#

am="1.8"
ac="2.59"

am_suffix=`echo "$am" | sed 's/[^0-9]//g'`
ac_suffix=`echo "$ac" | sed 's/[^0-9]//g'`

# Updating / creating ChangeLog
aux/svn2log.sh

# Gentoo fix
which emerge 2>&1 >/dev/null
if [ $? -eq 0 ]; then
	echo "Using Gentoo WANT_AUTOCONF=$ac / WANT_AUTOMAKE=$am hack."
	WANT_AUTOCONF="$ac"
	export WANT_AUTOCONF
	WANT_AUTOMAKE="$am"
	export WANT_AUTOMAKE
fi

# Multiplatform autotools invocation
sysver=`uname -s`
echo -n "Running autotools toolchain"
case "$sysver" in
*BSD)
	echo ", BSD style..."
	make=gmake
	aclocal$am_suffix &&
	autoheader$ac_suffix &&
	autoconf$ac_suffix &&
	automake$am_suffix --gnu -Wportability --add-missing
	;;
*)
	echo ", generic style..."
	make=make
	aclocal &&
	autoheader &&
	autoconf &&
	automake --gnu -Wportability --add-missing
	;;
esac

if [ $? -eq 0 ]; then
	echo "Done. To build and install now, run:"
	echo -e "\t./configure"
	echo -e "\t$make"
	echo -e "\t$make install"
else
	echo " *** An error occured!"
	echo " *** Before sending in a bug report, check that you have appropriate"
	echo " *** versions of both Autoconf ($ac) and Automake ($am) available."
fi
