REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.1  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK

create or replace package body epc_clnt as

subtype connection_method_subtype is pls_integer;

CONNECTION_METHOD_DBMS_PIPE constant connection_method_subtype := 1;
CONNECTION_METHOD_UTL_TCP constant connection_method_subtype := 2;

type epc_info_rectype is record (
  interface_name epc.interface_name_subtype,
  connection_method connection_method_subtype,
  epc.pipe_name_subtype request_pipe,
  utl_tcp.connection tcp_connection,
  xmldom.document doc
);

epc_info_tab epc_info_tabtype;

type epc_info_tabtype is table of epc_info_rectype index by binary_integer;

function register( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype
is
  l_idx epc_key_subtype;
begin
  begin
    l_idx := get_epc_key( p_interface_name );
  exception
    when no_data_found
    then
      l_idx := epc_info_tab.count+1;
      epc_info_tab(l_idx).interface_name := p_interface_name;
      epc_info_tab(l_idx).doc := xmldom.newDOMDocument;
  end;
  return l_idx;
end register;

function get_epc_key( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype
is
begin
  if epc_info_tab.count > 0 
  then
    for l_idx in epc_info_tab.first .. epc_info_tab.last
    loop
      if epc_info_tab(l_idx).interface_name = p_interface_name
      then
        return l_idx;
      end if;
    end loop;
  end if;
  raise no_data_found;
end get_epc_key;

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection in utl_tcp.connection
);

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection out utl_tcp.connection
);

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
);

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
);

procedure set_request_send_timeout
(
  p_epc_key in epc_key_subtype
, p_request_send_timeout in pls_integer
);

procedure set_response_recv_timeout
(
  p_epc_key in epc_key_subtype
, p_response_recv_timeout in pls_integer
);

procedure set_request_header
(
  p_epc_key in epc_key_subtype
, p_interface_name in epc.interface_name_subtype
, p_routine_name in epc.routine_name_subtype
, p_oneway in pls_integer
);

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
);

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
);

procedure set_request_trailer
(
  p_epc_key in epc_key_subtype
);

procedure send_request
( 
  p_epc_key in epc_key_subtype
);

procedure recv_response
( 
  p_epc_key in epc_key_subtype
);

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_t
, p_data_type in epc.data_type_subtype
, p_value out varchar2
);

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_t
, p_data_type in epc.data_type_subtype
, p_value out number
);

end epc_clnt;
/
