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
# Stellt eine binaere Distribution zusammen.
# Muss aus dem Top-Level-Verzeichnis heraus aufgerufen werden.
#

if [ -e configure.ac ]; then
	pkg=`grep AC_INIT configure.ac|sed 's/^AC_INIT(\[.*\], \[\(.*\)\], \[.*\], \[\(.*\)\])$/\2-\1/'`
else
	echo "Must be run from top level source directory."
	exit 1
fi

target="${1:-win32}"
dist="$pkg-$target"

txts='AUTHORS COPYING NEWS README THANKS TODO'

case "$target" in
win32)
	bin='uvnav.exe'
	rt='*.dll'
	;;
*)
	bin='uvnav'
	rt=''
	;;
esac

echo "Packaging $dist"

rm -rf $dist $dist.zip
mkdir -p $dist

mkdir -p $dist/fonts
cp share/fonts/*.ttf $dist/fonts/ || exit 1

mkdir -p $dist/images
cp share/images/*.png $dist/images/ || exit 1

mkdir -p $dist/docs
cp docs/*.txt $dist/docs/ || exit 1

cp src/$bin $dist/ || exit 1

if [ "x$rt" != "x" ]; then
	cp runtime/$target/$rt $dist/ || exit 1
fi

for txt in $txts; do
	cp $txt $dist/ || exit 1
	if [ "$target" = "win32" ]; then
		mv $dist/$txt $dist/docs/`echo $txt.txt | tr '[:upper:]' '[:lower:]'`
	fi
done

if [ "$target" = "win32" ]; then
	for f in `find $dist/ -name '*.txt'`; do
		chmod u+w $f
		recode latin1..windows-1252/CR-LF $f
	done
fi

zip -9 -r $dist.zip $dist/ || exit 1
rm -rf $dist/

echo "Package ready:"

ls -lash $dist.zip

