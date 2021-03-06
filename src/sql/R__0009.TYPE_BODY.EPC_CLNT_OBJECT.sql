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

end;
/

