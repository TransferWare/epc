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

MAINTAINERCLEANFILES = Makefile.in

GENERATED_FILES      = epctest.c epctest.h epctest.pc epctest.pkb epctest.pke epctest.pks epctest.pls
GENERATED_FILES     += str.c str.h str.pc str.pkb str.pke str.pks str.pls

MOSTLYCLEANFILES     = $(GENERATED_FILES)

AUTOMAKE_OPTIONS     = $(top_builddir)/src/lib/ansi2knr

AM_CPPFLAGS	     = -I$(top_srcdir)/src/lib

check_PROGRAMS       = epctest
epctest_SOURCES      = epctest.idl str.idl proc.c main.c
epctest_LDADD        = $(top_builddir)/src/lib/libepc.la

IDL = $(top_builddir)/src/prog/idl$(EXEEXT)

@SUFFIXES@: .c .idl

.idl.c:
	$(IDL) $<
# Before running PRO*C first compile the package
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$(@:.c=.pls)
	$(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$(@:.c=.pc) ONAME=$@

EXTRA_DIST           = Imakefile epctest.ref epctest.sql str.sql

TESTS = test1

EPCTEST = ./epctest
DIFF = diff

test1: $(top_srcdir)/test/epctest.sql
	@$(EPCTEST) -d -u$(USERID) 1>epctest.log &
	@sleep 20
	@echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$< 1
	@pid=`ps | grep epctest | grep -v grep |  awk '{ print $$1; }'`; kill "$$pid"
	@$(DIFF) epctest.lis $(top_srcdir)/test/epctest.ref
	@echo : > $@