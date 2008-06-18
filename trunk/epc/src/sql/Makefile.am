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

noinst_DATA      = epc.pks epc.pkb epc.pks_run epc.pkb_run
noinst_DATA     += epc_srvr.pks epc_srvr.pkb epc_srvr.pks_run epc_srvr.pkb_run
noinst_DATA     += std_object.typ std_object.typ_run
noinst_DATA     += std_objects.tab std_objects.tab_run
noinst_DATA     += std_object_mgr.pks std_object_mgr.pkb \
                   std_object_mgr.pks_run std_object_mgr.pkb_run
noinst_DATA     += epc_clnt_object.typ epc_clnt_object.tyb \
                   epc_clnt_object.typ_run epc_clnt_object.tyb_run
noinst_DATA     += epc_clnt.pks epc_clnt.pkb epc_clnt.pks_run epc_clnt.pkb_run

CLEANFILES       = *.???_run pod*.tmp

EXTRA_DIST       = epc.pks epc.pkb
EXTRA_DIST      += epc_srvr.pks epc_srvr.pkb
EXTRA_DIST      += std_object.typ
EXTRA_DIST      += std_objects.tab
EXTRA_DIST      += std_object_mgr.pks std_object_mgr.pkb
EXTRA_DIST      += epc_clnt_object.typ epc_clnt_object.tyb
EXTRA_DIST      += epc_clnt.pks epc_clnt.pkb

EXTRA_DIST      += login.sql

@SUFFIXES@: .typ .tyb .tab .pks .pkb .typ_run .tyb_run .tab_run .pks_run .pkb_run

# &lt and &gt are used in epc_clnt.pks
# To prevent asking for the defines, the login.sql sets define off,
# but since login.sql may not be the build directory SQLPATH must be set 
# to the source directory.

.typ.typ_run:
	echo exit SQL.SQLCODE | ( SQLPATH=$(srcdir) && export SQLPATH && $(SQLPLUS) $(USERID) @$? )
	@touch $@

.tyb.tyb_run:
	echo exit SQL.SQLCODE | ( SQLPATH=$(srcdir) && export SQLPATH && $(SQLPLUS) $(USERID) @$? )
	@touch $@

.tab.tab_run:
	echo exit SQL.SQLCODE | ( SQLPATH=$(srcdir) && export SQLPATH && $(SQLPLUS) $(USERID) @$? )
	@touch $@

.pks.pks_run:
	echo exit SQL.SQLCODE | ( SQLPATH=$(srcdir) && export SQLPATH && $(SQLPLUS) $(USERID) @$? )
	@touch $@

.pkb.pkb_run:
	echo exit SQL.SQLCODE | ( SQLPATH=$(srcdir) && export SQLPATH && $(SQLPLUS) $(USERID) @$? )
	@touch $@

