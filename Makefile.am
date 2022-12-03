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

AUTOMAKE_OPTIONS 	   = 
SUBDIRS 		         = doc replace src test utils

# GJP 2018-08-20  Trick to only distribute it and not build (see also configure.ac)
if EXAMPLES

SUBDIRS 	          += examples

endif

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure config-h.in \
                       stamp-h.in $(AUX_DIST)
DISTCLEANFILES       = $(distdir).tar.gz
EXTRA_DIST           = VERSION lint.am .mvn README.md CHANGELOG.md darwin-remove-rpath.sh pom.xml .mvn/maven.config ln_oracle_tools.sh

# Gert-Jan Paulissen, 15-01-2014
# libtoolize: AC_CONFIG_MACRO_DIR([config]) conflicts with ACLOCAL_AMFLAGS=-I config
ACLOCAL_AMFLAGS      = -I config

MVN                  = mvn

lintdirs	           = $(top_builddir)/src/lib $(top_builddir)/src/prog

check: check-version

check-version:
	@echo "Testing that package version is the same as the Maven revision"
	chmod +x $(top_srcdir)/ln_oracle_tools.sh && $(top_srcdir)/ln_oracle_tools.sh $(top_srcdir)
	@mvn_version=`echo '$${revision}' | $(MVN) -f $(top_srcdir)/pom.xml -N help:evaluate | grep -v '[INFO]'`; set -x; test "$(PACKAGE_VERSION)" = "$$mvn_version"

dist: lint html

lint:
	@echo Making $@ in $(lintdirs)
	@for d in $(lintdirs); do (cd $$d && make $@); done

dist-hook:
	chmod +x $(top_srcdir)/config/install-sh

distuninstallcheck:
         @:
