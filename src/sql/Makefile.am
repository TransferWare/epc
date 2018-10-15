## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002-2008 G.J. Paulissen
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

CLEANFILES       = *.???_run pod*.tmp

sql_scripts      = epc.pks epc.pkb
sql_scripts     += epc_srvr.pks epc_srvr.pkb
sql_scripts     += std_object.typ std_object.tyb
sql_scripts     += std_objects.tab
sql_scripts     += std_object_mgr.pks std_object_mgr.pkb
sql_scripts     += epc_clnt_object.typ epc_clnt_object.tyb
sql_scripts     += epc_clnt.pks epc_clnt.pkb
sql_scripts     += epc_install.sql epc_uninstall.sql epc_verify.sql

noinst_DATA      = $(sql_scripts)

# Use order of installation: 
# 1) object specifications
# 2) tables
# 3) package specifications
# 4) bodies

noinst_DATA     += epc_install.sql_run

EXTRA_DIST       = $(sql_scripts)

uninstall: epc_uninstall.sql_run clean

epc_install.sql_run: epc_install.sql
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$<
	@touch $@

epc_uninstall.sql_run: epc_uninstall.sql
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$< 

