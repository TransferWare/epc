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

if PLDOC
generated_pldoc_html_dir = sql
else
generated_pldoc_html_dir = 
endif

if DOXYGEN
generated_doxygen_html_dir = c
else
generated_doxygen_html_dir =
endif

generated_html_dirs = $(generated_pldoc_html_dir) $(generated_doxygen_html_dir)

EXTRA_DIST           = epcman.html epcman_files/ EPC-optimize.html EPC-optimize_files/ $(generated_html_dirs)

CLEANFILES           = sql/* c/* *.tmp

SCHEMA               = `echo $${USERID:?} | cut -d '/' -f 1`

clean-local:
	-rm -fr sql c

FORCE = force

force: ;

html: $(generated_html_dirs)

# Create HTML documentation in the source tree
sql: $(FORCE)
	@$(PLDOC) -d $@ -doctitle EPC -user "$(SCHEMA)" $(srcdir)/../src/sql/R__*PACKAGE_SPEC*.sql

# Create HTML documentation in the source tree
c: $(FORCE)
	@cat Doxyfile | ($(DOXYGEN) -)
	@-rm -f *.tmp
