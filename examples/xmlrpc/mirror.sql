REM See also http://www.xmlrpc.com/currentTime

set serveroutput on size 1000000 format truncated
declare
  http_connection epc_clnt.http_connection_subtype;
  l_xml epc.xml_subtype;
begin
  dbug.activate('DBMS_OUTPUT');
  dbug.enter('mirror.sql');
  http_connection.url := 'http://www.mirrorproject.com/xmlrpc/';
  epc_clnt.set_connection_info('mirror', http_connection);
  epc_clnt.set_protocol('mirror', epc_clnt."XMLRPC");
  l_xml := mirror.Random;
  dbug.print('info', 'xml: %s', l_xml);
  dbug.leave;
end;
/
