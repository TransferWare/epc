REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.2  2004/04/21 11:16:55  gpaulissen
REMARK  .
REMARK
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

type epc_request is record (
  interface_name epc.interface_name_subtype,
  method_name epc.method_name_subtype, 
  oneway pls_integer,
  body varchar2(32767)  
);

type epc_info_rectype is record (
  interface_name epc.interface_name_subtype,
  connection_method connection_method_subtype,
  request_pipe epc.pipe_name_subtype,
  tcp_connection utl_tcp.connection,
  req epc_request,
  send_timeout pls_integer default 10,
  recv_timeout pls_integer default 10
);

type epc_info_tabtype is table of epc_info_rectype index by binary_integer;

epc_info_tab epc_info_tabtype;

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
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_UTL_TCP;
  epc_info_tab(p_epc_key).tcp_connection := p_connection;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection out utl_tcp.connection
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_UTL_TCP
  then
    p_connection := epc_info_tab(p_epc_key).tcp_connection;
  else
    raise no_data_found;
  end if;
end get_connection_info;

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_DBMS_PIPE;
  epc_info_tab(p_epc_key).request_pipe := p_pipe_name;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
    p_pipe_name := epc_info_tab(p_epc_key).request_pipe;
  else
    raise no_data_found;
  end if;
end get_connection_info;

procedure set_request_send_timeout
(
  p_epc_key in epc_key_subtype
, p_request_send_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).send_timeout := p_request_send_timeout;
end set_request_send_timeout;

procedure set_response_recv_timeout
(
  p_epc_key in epc_key_subtype
, p_response_recv_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).recv_timeout := p_response_recv_timeout;
end set_response_recv_timeout;

procedure set_request_header
(
  p_epc_key in epc_key_subtype
, p_interface_name in epc.interface_name_subtype
, p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).req.interface_name := p_interface_name;
  epc_info_tab(p_epc_key).req.method_name := p_method_name;
  epc_info_tab(p_epc_key).req.oneway := p_oneway;
end set_request_header;

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
)
is
begin
  if p_data_type = epc.data_type_string
  then
    epc_info_tab(p_epc_key).req.body :=
      epc_info_tab(p_epc_key).req.body 
      ||'<'||p_name||' xsi:type="string">'||p_value||'</'||p_name||'>';
  else
    raise value_error;
  end if;
end set_request_parameter;

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
)
is
  l_data_type varchar2(10);
begin
  if p_data_type in ( epc.data_type_int, epc.data_type_long )
  then
    l_data_type := 'integer';
  elsif p_data_type = epc.data_type_float
  then
    l_data_type := 'float';
  elsif p_data_type = epc.data_type_double
  then
    l_data_type := 'double';
  else
    raise value_error;
  end if;

  epc_info_tab(p_epc_key).req.body :=
    epc_info_tab(p_epc_key).req.body 
    ||'<'||p_name||' xsi:type="'||l_data_type||'">'||to_char(p_value)||'</'||p_name||'>';
end set_request_parameter;

procedure set_request_trailer
(
  p_epc_key in epc_key_subtype
)
is
begin
  null;
end set_request_trailer;

procedure send_request
( 
  p_epc_key in epc_key_subtype
)
is
  l_msg varchar2(32767);
begin
  l_msg :=
'SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsi="http://www.w3.org/1999/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/1999/XMLSchema"'
||'<SOAP-ENV:Body><'
||epc_info_tab(p_epc_key).req.method_name
||' '
||epc_info_tab(p_epc_key).req.interface_name
||' SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">'
||epc_info_tab(p_epc_key).req.body
||'</'
||epc_info_tab(p_epc_key).req.method_name
||'></SOAP-ENV:Body></SOAP-ENV:Envelope>';
end send_request;

procedure recv_response
( 
  p_epc_key in epc_key_subtype
)
is
begin
  null;
end recv_response;

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
)
is
begin
  null;
end get_response_parameter;

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
)
is
begin
  null;
end get_response_parameter;

end epc_clnt;
/
