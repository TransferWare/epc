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
SUBDIRS 		= doc src examples test utils
MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure config-h.in \
			stamp-h.in $(AUX_DIST)
ACLOCAL			= aclocal -I $(ac_aux_dir)

dist: doc

generated_html_dirs     = doc/sql doc/c

doc: $(generated_html_dirs)

docdir			= $(top_builddir)/utils

doc:
	@echo Making $@ in $(docdir)
	@cd $(docdir) && make $@

FORCE = force

force: ;

PLDOC = pldoc.bat

doc/sql: $(FORCE)
	@mkdir -p $@
	@$(PLDOC) -d $@ -doctitle EPC $(top_srcdir)/src/sql/*.pks

DOXYGEN = doxygen

doc/c: $(top_srcdir)/Doxyfile
	@$(DOXYGEN) $<

EXTRA_DIST              = VERSION $(generated_html_dirs)
