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

sql_scripts      = R__09.PACKAGE_SPEC.EPC.sql R__14.PACKAGE_BODY.EPC.sql
sql_scripts     += R__09.PACKAGE_SPEC.EPC_SRVR.sql R__14.PACKAGE_BODY.EPC_SRVR.sql
sql_scripts     += 02.TYPE_SPEC.STD_OBJECT.sql R__15.TYPE_BODY.STD_OBJECT.sql
sql_scripts     += 06.TABLE.STD_OBJECTS.sql
sql_scripts     += R__09.PACKAGE_SPEC.STD_OBJECT_MGR.sql R__14.PACKAGE_BODY.STD_OBJECT_MGR.sql
sql_scripts     += 02.TYPE_SPEC.EPC_CLNT_OBJECT.sql R__15.TYPE_BODY.EPC_CLNT_OBJECT.sql
sql_scripts     += R__09.PACKAGE_SPEC.EPC_CLNT.sql R__14.PACKAGE_BODY.EPC_CLNT.sql
sql_scripts     += install.sql uninstall.sql

noinst_DATA      = $(sql_scripts)

# Use order of installation: 
# 1) object specifications
# 2) tables
# 3) package specifications
# 4) bodies

noinst_DATA     += install.sql_run

EXTRA_DIST       = $(sql_scripts)

uninstall: uninstall.sql_run clean

install.sql_run: install.sql
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$<
	@touch $@

uninstall.sql_run: uninstall.sql
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$< 

