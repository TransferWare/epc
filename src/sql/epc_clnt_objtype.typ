create or replace type epc_clnt_objtype under epc_objtype (
  info_tab epc_clnt_info_tabtype

, constructor function epc_clnt_objtype
  return self as result

) final
/

show errors
