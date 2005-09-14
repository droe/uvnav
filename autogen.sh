#!/bin/sh
# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004-2005 Daniel Roethlisberger <roe@chronator.ch>
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

am="1.9"
ac="2.59"

bsd_autotools_prefix="/usr/local/gnu-autotools"

am_suffix=`echo "$am" | sed 's/[^0-9]//g'`
ac_suffix=`echo "$ac" | sed 's/[^0-9]//g'`

# Updating / creating ChangeLog
util/svn2log.sh

# Multiplatform autotools invocation
AM_WARN="-Wportability"

sysver=`uname -s`
echo -n "Running autotools toolchain, "
case "$sysver" in
*BSD)
	echo -n "BSD"
	if ! which -s autoconf ; then
		if [ -d "$bsd_autotools_prefix" ]; then
			PATH=$bsd_autotools_prefix/bin:$PATH
			export PATH
		else
			echo
			echo " *** An error occured!" >&2
			echo " *** Could not find autoconf in PATH or at $bsd_autotools_prefix!" >&2
			echo " *** Make sure you install the devel/gnu-* ports." >&2
			exit 1
		fi
	fi
	make=gmake
	;;
Linux)
	echo -n "Linux"
	make=make
	# Gentoo
	if which -s emerge ; then
		WANT_AUTOCONF="$ac"
		export WANT_AUTOCONF
		WANT_AUTOMAKE="$am"
		export WANT_AUTOMAKE
	fi
	;;
*)
	echo -n "generic non-GNU Unix"
	make=gmake
	;;
esac
echo " style... ($sysver)"
aclocal &&
autoheader &&
autoconf &&
automake --gnu $AM_WARN --add-missing

if [ $? -eq 0 ]; then
	echo "Done. To configure, build and install now, run:"
	echo -e "\t./configure"
	echo -e "\t$make"
	echo -e "\t$make install"
else
	echo " *** An error occured!" >&2
	echo " *** Before sending in a bug report, check that you have appropriate" >&2
	echo " *** versions of both autoconf ($ac) and automake ($am) available." >&2
fi
