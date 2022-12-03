CREATE OR REPLACE TYPE BODY "EPC_CLNT_OBJECT" 
is

constructor function epc_clnt_object
( self in out nocopy epc_clnt_object
, p_interface_name in varchar2
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
      self.send_timeout := 0; /* GJP 2018-08-21 10 => 60 */ /* GJP 2022-12-03 60 => 0 */
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
member procedure serialize(self in epc_clnt_object, p_json_object in out nocopy json_object_t)
is
begin
  -- every sub type must first start with (self as <super type>).serialize(p_json_object)
  (self as std_object).serialize(p_json_object);

  p_json_object.put('INTERFACE_NAME', interface_name);
  p_json_object.put('PROTOCOL', protocol);
  p_json_object.put('NAMESPACE', namespace);
  p_json_object.put('CONNECTION_METHOD', connection_method);
  p_json_object.put('TCP_REMOTE_HOST', tcp_remote_host);
  p_json_object.put('TCP_REMOTE_PORT', tcp_remote_port);
  p_json_object.put('TCP_LOCAL_HOST ', tcp_local_host );
  p_json_object.put('TCP_LOCAL_PORT ', tcp_local_port );
  p_json_object.put('TCP_CHARSET    ', tcp_charset    );
  p_json_object.put('TCP_NEWLINE    ', tcp_newline    );
  p_json_object.put('TCP_TX_TIMEOUT ', tcp_tx_timeout );
  p_json_object.put('TCP_PRIVATE_SD ', tcp_private_sd );
  p_json_object.put('HTTP_URL', http_url);
  p_json_object.put('HTTP_METHOD', http_method);
  p_json_object.put('HTTP_VERSION', http_version);
  p_json_object.put('REQUEST_PIPE', request_pipe);
  p_json_object.put('SEND_TIMEOUT', send_timeout);
  p_json_object.put('RECV_TIMEOUT', recv_timeout);
end serialize;

end;
/

