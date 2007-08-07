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
ACLOCAL			= aclocal -I $(ac_aux_dir)

dist: lint

lint:
	cd $(top_builddir)/src/lib && make lint
	cd $(top_builddir)/src/prog && make lint

generated_html_dirs     = doc/sql doc/c

doc: $(generated_html_dirs)

docdir			= $(top_builddir)/utils

doc:
	@echo Making $@ in $(docdir)
	@cd $(docdir) && make $@

FORCE = force

force: ;

# Create HTML documentation in the source tree
doc/sql: $(FORCE)
	@cd $(top_srcdir) && mkdir -p $@
	@cd $(top_srcdir) && $(PLDOC) -d $@ -doctitle EPC src/sql/*.pks

# Create HTML documentation in the source tree
doc/c: $(FORCE)
	@cd $(top_srcdir) && $(DOXYGEN)

EXTRA_DIST              = VERSION $(generated_html_dirs)

dist-hook:
	chmod +x $(top_srcdir)/config/install-sh