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

MAINTAINERCLEANFILES = Makefile.in

qotd_sources         = qotd.pkb qotd.pks qotd.pls
aws_sources          = aws.pkb aws.pks aws.pls

BUILT_SOURCES        = $(qotd_sources) $(aws_sources)

MOSTLYCLEANFILES     = $(BUILT_SOURCES)

CLEANFILES           = *.pls_run *.idl_run *.c *.h *.pc *.pke

# Just the IDL files and the SQL*Plus scripts
EXTRA_DIST           = qotd.idl aws.idl $(BUILT_SOURCES)

noinst_DATA          = qotd.pls_run aws.pls_run

IDL                  = $(top_builddir)/src/prog/idl$(EXEEXT)

@SUFFIXES@           = .pls_run .pls .idl_run .idl

.idl.idl_run:
	$(IDL) $<
	@touch $@

.pls.pls_run:
# Before running the compiler first compile the package
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$<
	@touch $@

$(qotd_sources): qotd.idl_run

$(aws_sources): aws.idl_run

qotd.idl_run aws.idl_run: $(IDL)

$(IDL):
	cd $(top_builddir)/src/prog && $(MAKE) all
