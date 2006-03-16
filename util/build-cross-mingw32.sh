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
# build-cross-mingw32.sh - Kompiliert mit dem MinGW32 Cross Compiler.
# Muss aus dem Top-Level-Verzeichnis heraus aufgerufen werden.
#

target=mingw32
#build=i386-unknown-freebsd5.2.1
prefix="/usr/local/$target"

test -d util || exit 1

sysver=`uname -s`
case "$sysver" in
*BSD)
	make=gmake
	;;
*)
	make=make
	;;
esac

# Maintainer-Clean
if [ -e Makefile ]; then
	find . -name Makefile | xargs touch
	$make maintainer-clean
fi

export PATH="$prefix/bin:$PATH"
export SDL_CONFIG="$prefix/bin/$target-sdl-config"
if [ "x$build" != "x" ]; then
	build_opt="--build=$build"
fi

./autogen.sh &&
./configure --target=$target --host=$target $build_opt "$@" &&
$make &&
$target-strip */*.exe &&
echo "Win32 executable(s) ready:" &&
ls -las */*.exe
