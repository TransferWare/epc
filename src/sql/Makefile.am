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
noinst_DATA     += epc_clnt.pks epc_clnt.pkb epc_clnt.pks_run epc_clnt.pkb_run
noinst_DATA     += epc_srvr.pks epc_srvr.pkb epc_srvr.pks_run epc_srvr.pkb_run

CLEANFILES       = *.pk?_run pod*.tmp

EXTRA_DIST       = epc.pks epc.pkb
EXTRA_DIST      += epc_clnt.pks epc_clnt.pkb
EXTRA_DIST      += epc_srvr.pks epc_srvr.pkb

@SUFFIXES@: .pks .pkb .pks_run .pkb_run

.pks.pks_run:
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$?
	@touch $@

.pkb.pkb_run:
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$?
	@touch $@


