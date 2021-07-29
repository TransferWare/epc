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

EXTRA_DIST           = epcman.doc EPC-optimize.doc epcdistr.rtf epcpf.xls problems&changes.xls $(generated_html_dirs)

CLEANFILES           = sql/* c/* *.tmp

clean-local:
	-rm -fr sql c

FORCE = force

force: ;

doc: $(generated_html_dirs)

# Create HTML documentation in the source tree
sql: $(FORCE)
	@cd $(srcdir); if test -d $@; then chmod +w $@; rm -fr $@; fi; mkdir -p $@
	@$(PLDOC) -d $@ -doctitle EPC -user $(SCHEMA) $(srcdir)/../src/sql/R__*PACKAGE_SPEC*.sql

# Create HTML documentation in the source tree
c: $(FORCE)
	@cd $(srcdir); if test -d $@; then chmod +w $@; rm -fr $@; fi; mkdir -p $@
	@-rm -f *.tmp
# The doc srcdir must be writeable because DOXYGEN creates temporary files there.
	@cat Doxyfile | (test -f Doxyfile && cd $(srcdir) && chmod +w . && $(DOXYGEN) -)
	@-rm -f *.tmp

