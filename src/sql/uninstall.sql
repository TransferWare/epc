--$NO_KEYWORD_EXPANSION$
REMARK
REMARK  $Header$
REMARK

whenever sqlerror continue

drop package epc_clnt;
drop package epc_srvr;
drop package epc;
drop package std_object_mgr;
drop table std_objects purge;
drop type epc_clnt_object;
drop type std_object;

whenever sqlerror exit failure

-- set sql.sqlcode to 0
set termout off

select count(*) from dual
/
