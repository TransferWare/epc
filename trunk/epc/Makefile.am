## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 G.J. Paulissen
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

AUTOMAKE_OPTIONS 	= 
SUBDIRS 		= config src examples test utils
MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure config-h.in \
			stamp-h.in $(AUX_DIST)
ACLOCAL			= aclocal -I $(ac_aux_dir)

docdir			= $(top_builddir)/utils

html:
	@echo Making $@ in $(docdir)
	@cd $(docdir) && make $@

dist-hook:
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done

EXTRA_DIST              = bootstrap Imakefile MANIFEST MANIFEST.SKIP Makefile.PL README.html README.imake \
			cmp/EPC.cmp cmp/EPC_CORE.cmp cmp/EPC_DEV.cmp \
			cmp/EPC_DOC.cmp cmp/EPC_RUN.cmp doc/epcman.doc \
			doc/EPC-optimize.doc doc/epcman.doc doc/epcman.html 
