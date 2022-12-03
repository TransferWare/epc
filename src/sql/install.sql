whenever oserror exit failure
whenever sqlerror exit failure
set define off sqlblanklines on
ALTER SESSION SET PLSQL_WARNINGS = 'ENABLE:ALL';

@@0001.TYPE_SPEC.EPC_CLNT_OBJECT.sql
sho err type EPC_CLNT_OBJECT
@@R__0002.PACKAGE_SPEC.EPC.sql
sho err package epc
@@R__0003.PACKAGE_SPEC.EPC_SRVR.sql
sho err package epc_srvr
@@R__0004.PACKAGE_SPEC.EPC_CLNT.sql
sho err package epc_clnt
@@R__0005.TYPE_BODY.EPC_CLNT_OBJECT.sql
sho err type body epc_clnt_object
@@R__0006.PACKAGE_BODY.EPC_SRVR.sql
sho err package body epc_srvr
@@R__0007.PACKAGE_BODY.EPC_CLNT.sql
sho err package body epc_clnt
@@R__0008.PACKAGE_BODY.EPC.sql
sho err package body epc

