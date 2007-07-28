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

GENERATED_FILES      = currentTime.c currentTime.h currentTime.c currentTime.pkb \
                       currentTime.pke currentTime.pks currentTime.pls
GENERATED_FILES     += mirror.c mirror.h mirror.c mirror.pkb \
                       mirror.pke mirror.pks mirror.pls

MOSTLYCLEANFILES     = $(GENERATED_FILES)

CLEANFILES           = currentTime.pls_run
CLEANFILES          += mirror.pls_run

EXTRA_DIST           = 

noinst_DATA          = currentTime.pls_run
noinst_DATA         += mirror.pls_run

IDL = $(top_builddir)/src/prog/idl$(EXEEXT)

$(GENERATED_FILES): $(IDL)

$(IDL):
	cd $(top_builddir)/src/prog && make all

@SUFFIXES@: .idl .pls_run .pls

.idl.pls:
	$(IDL) $<

.pls.pls_run:
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$?
	@touch $@


