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

if COND_TOOLS
  MAYBE_TOOLS = tools
endif
SUBDIRS = util share lib src $(MAYBE_TOOLS)

EXTRA_DIST = \
    autogen.sh \
    docs

dist-hook:
	rm -rf `find $(distdir)/docs -name .svn`

# gehoert zu rev.h Hack, siehe src/Makefile.am
distcleancheck_listfiles = \
    find . -type f -print | grep -v '\.rev' | grep -v 'rev\.h'

MAINTAINERCLEANFILES = \
    Makefile.in configure configure.lineno aclocal.m4 config.h.in \
    config.h.in~ ChangeLog INSTALL config.guess config.sub depcomp \
    install-sh missing mkinstalldirs compile \[config.h\].in

ChangeLog: FORCE
	@if [ -d .svn -a -x util/svn2log.sh ]; then \
		touch ChangeLog ; \
		util/svn2log.sh ; \
	fi

FORCE:

