CREATE OR REPLACE TYPE BODY "EPC_CLNT_OBJECT" 
is

constructor function epc_clnt_object
( p_interface_name in varchar2
)
return self as result
is
  l_object_name constant std_objects.object_name%type := 'EPC_CLNT' || '.' || p_interface_name;
  l_std_object std_object;
begin
  begin
    std_object_mgr.get_std_object(l_object_name, l_std_object);
    self := treat(l_std_object as epc_clnt_object);
    self.dirty := 0;
  exception
    when no_data_found
    then
      self.dirty := 1; -- a new object is dirty
      self.interface_name := p_interface_name;
      self.protocol := epc_clnt."NATIVE";
      self.namespace := p_interface_name;
      self.inline_namespace := 'ns1';
      self.connection_method := epc_clnt.CONNECTION_METHOD_DBMS_PIPE;
      self.request_pipe := 'epc_request_pipe';
      self.http_method := 'POST';
      self.http_version := utl_http.http_version_1_1;
      self.send_timeout := 60; /* GJP 2018-08-21 10 => 60 */
      self.recv_timeout := 60; /* GJP 2018-08-21 10 => 60 */
  end;

  -- essential
  return;
end;

overriding
member function name(self in epc_clnt_object)
return varchar2
is
begin
  return 'EPC_CLNT' || '.' || interface_name;
end name;

overriding
member procedure print(self in epc_clnt_object)
is
begin
  (self as std_object).print; -- Generalized invocation 
  dbms_output.put_line
  ( utl_lms.format_message
    ( '%s.%s.%s; interface: %s; protocol: %s; namespace: %s'
    , $$PLSQL_UNIT_OWNER
    , $$PLSQL_UNIT
    , 'PRINT'
    , interface_name
    , to_char(protocol)
    , namespace
    )
  );
  dbms_output.put_line
  ( utl_lms.format_message
    ( '%s.%s.%s; inline namespace: %s; connection method: %s'
    , $$PLSQL_UNIT_OWNER
    , $$PLSQL_UNIT
    , 'PRINT'
    , inline_namespace
    , to_char(connection_method)
    )
  );

  case connection_method
    when epc_clnt.CONNECTION_METHOD_UTL_TCP
    then
      dbms_output.put_line
      ( utl_lms.format_message
        ( '%s.%s.%s; remote host/port (%s/%s); local host/port (%s/%s); timeout: %s'
        , $$PLSQL_UNIT_OWNER
        , $$PLSQL_UNIT
        , 'PRINT'
        , tcp_remote_host
        , to_char(tcp_remote_port)
        , tcp_local_host
        , to_char(tcp_local_port)
        --, tcp_charset     varchar2(30)
        --, tcp_newline     varchar2(2)
        , to_char(tcp_tx_timeout)
        --, tcp_private_sd  integer
        )
      );

    when epc_clnt.CONNECTION_METHOD_UTL_HTTP
    then
      dbms_output.put_line
      ( utl_lms.format_message
        ( '%s.%s.%s; http url: %s; method: %s; version: %s'
        , $$PLSQL_UNIT_OWNER
        , $$PLSQL_UNIT
        , 'PRINT'
        , http_url
        , http_method
        , http_version
        )
      );
  
    when epc_clnt.CONNECTION_METHOD_DBMS_PIPE
    then
      dbms_output.put_line
      ( utl_lms.format_message
        ( '%s.%s.%s; request pipe: %s; send timeout: %s; recv timeout: %s'
        , $$PLSQL_UNIT_OWNER
        , $$PLSQL_UNIT
        , 'PRINT'
        , request_pipe
        , to_char(send_timeout)
        , to_char(recv_timeout)
        )
      );
  end case;
end print;

end;
/

