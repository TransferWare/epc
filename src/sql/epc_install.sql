--$NO_KEYWORD_EXPANSION$
REMARK
REMARK  $HeadURL$
REMARK

whenever oserror exit failure

set termout on

REMARK Set define off for epc_clnt.pks.
REMARK Do not define it in epc_clnt.pks because PLDoc does not like that.

set define off

prompt @@std_object.typ
@@std_object.typ
prompt @@epc_clnt_object.typ
@@epc_clnt_object.typ
prompt @@std_objects.tab
@@std_objects.tab
prompt @@std_object_mgr.pks
@@std_object_mgr.pks
prompt @@epc.pks
@@epc.pks
prompt @@epc_srvr.pks
@@epc_srvr.pks
prompt @@epc_clnt.pks
@@epc_clnt.pks
prompt @@std_object.tyb
@@std_object.tyb
prompt @@epc_clnt_object.tyb
@@epc_clnt_object.tyb
prompt @@std_object_mgr.pkb
@@std_object_mgr.pkb
prompt @@epc.pkb
@@epc.pkb
prompt @@epc_srvr.pkb
@@epc_srvr.pkb
prompt @@epc_clnt.pkb
@@epc_clnt.pkb
