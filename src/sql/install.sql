REMARK Try to call Flyway script beforeEachMigrate.sql (add its directory to SQLPATH) so that PLSQL_CCFlags can be set.
REMARK But no harm done if it is not there.

whenever oserror continue
whenever sqlerror continue
@@beforeEachMigrate.sql

whenever oserror exit failure
whenever sqlerror exit failure
set define off sqlblanklines on
ALTER SESSION SET PLSQL_WARNINGS = 'ENABLE:ALL';

prompt @@0001.TYPE_SPEC.EPC_CLNT_OBJECT.sql
@@0001.TYPE_SPEC.EPC_CLNT_OBJECT.sql
show errors TYPE "EPC_CLNT_OBJECT"
prompt @@R__0002.PACKAGE_SPEC.EPC.sql
@@R__0002.PACKAGE_SPEC.EPC.sql
show errors PACKAGE "EPC"
prompt @@R__0003.PACKAGE_SPEC.EPC_SRVR.sql
@@R__0003.PACKAGE_SPEC.EPC_SRVR.sql
show errors PACKAGE "EPC_SRVR"
prompt @@R__0004.PACKAGE_SPEC.EPC_CLNT.sql
@@R__0004.PACKAGE_SPEC.EPC_CLNT.sql
show errors PACKAGE "EPC_CLNT"
prompt @@R__0005.TYPE_BODY.EPC_CLNT_OBJECT.sql
@@R__0005.TYPE_BODY.EPC_CLNT_OBJECT.sql
show errors TYPE BODY "EPC_CLNT_OBJECT"
prompt @@R__0006.PACKAGE_BODY.EPC_SRVR.sql
@@R__0006.PACKAGE_BODY.EPC_SRVR.sql
show errors PACKAGE BODY "EPC_SRVR"
prompt @@R__0007.PACKAGE_BODY.EPC_CLNT.sql
@@R__0007.PACKAGE_BODY.EPC_CLNT.sql
show errors PACKAGE BODY "EPC_CLNT"
