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

MOSTLYCLEANFILES     = demo.h demo.pc demo.pkb demo.pke demo.pks demo.pls

AUTOMAKE_OPTIONS     = ../../src/lib/ansi2knr

INCLUDES	     = -I$(top_builddir) -I$(top_srcdir) -I../lib

bin_PROGRAMS         = demo
demo_SOURCES         = demo.c demo.h
demo_LDADD           = -ldbug

#AM_CPPFLAGS          = -I../src/lib

../../src/prog/idl:
	cd `dirname $@` && make

demo.pc: demo.idl ../../src/prog/idl
	../../src/prog/idl $<

@SUFFIXES@: .c .pc

.pc.c:
	@PROC@ @PROCFLAGS@ iname=$< oname=$@

EXTRA_DIST           = Imakefile demo.pks demo.pkb demo.pke demo.pls demo.pc demo.idl

