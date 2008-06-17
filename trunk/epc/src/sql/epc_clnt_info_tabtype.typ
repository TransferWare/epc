REMARK At most 10 pieces of client info active

create or replace type epc_clnt_info_tabtype as varying array(10) of epc_clnt_info_objtype
/

show errors
