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

include ../../lint.am

# Flex does not produce lint quality code
idl_l.ln: idl_l.c
	$(LINT_c) +posixlib -temptrans -nullassign -incondefs -noeffect -evalorder -exitarg -exportlocal $<

# Bison does not produce lint quality code
idl_y.ln: idl_y.c
	$(LINT_c) -noeffect -branchstate -usereleased -exportlocal $<

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS     = $(top_builddir)/src/lib/ansi2knr

AM_CPPFLAGS	     = -I$(top_srcdir)/src/lib -I$(includedir)

bin_SCRIPTS          = epcdaemon epcdaemon.cmd epcdaemon.pl
bin_PROGRAMS         = idl
idl_SOURCES          = idl_l.l idl_prs.c idl_y.y idl_kwrd.h idl_prs.h yy.h yywrap.c
#idl_LDADD            = $(top_builddir)/replace/libreplace.la $(libdir)/libdbug.la
idl_LDADD            = $(top_builddir)/replace/libreplace.la

AM_YFLAGS            = -d

EXTRA_DIST           = epcdaemon epcdaemon.cmd epcdaemon.pl.in

idl_l.c: idl_y.h
