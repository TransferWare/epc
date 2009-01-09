## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 G.J. Paulissen
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

AUTOMAKE_OPTIONS 	= 
SUBDIRS 		= doc replace src examples test utils
MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure config-h.in \
			stamp-h.in $(AUX_DIST)
#ACLOCAL			= aclocal -I $(ac_aux_dir)
EXTRA_DIST              = VERSION lint.am
ACLOCAL_AMFLAGS           = -I m4

lintdirs		= $(top_builddir)/src/lib $(top_builddir)/src/prog
docdirs			= $(top_builddir)/doc $(top_builddir)/utils

FORCE                   = force

force: ;

dist: lint doc

lint:
	@echo Making $@ in $(lintdirs)
	@for d in $(lintdirs); do (cd $$d && make $@); done

doc: $(FORCE)
	@echo Making $@ in $(docdirs)
	@for d in $(docdirs); do (cd $$d && make $@); done

dist-hook:
	chmod +x $(top_srcdir)/config/install-sh

distuninstallcheck:
         @:
