CREATE TYPE "EPC_CLNT_OBJECT" AUTHID DEFINER under std_object (
  interface_name varchar2(32) /*epc.interface_name_subtype*/  /* the key */
  /* Protocol information */
, protocol integer
  /* SOAP related information */
, namespace varchar2(128) /*epc.namespace_subtype*/
, inline_namespace varchar2(128) /*epc.namespace_subtype*/
  /* Connection information: TCP/IP, HTTP, database pipe */
, connection_method integer

  /* Fields of type utl_tcp.connection */
, tcp_remote_host varchar2(255)
, tcp_remote_port integer
, tcp_local_host  varchar2(255)
, tcp_local_port  integer
, tcp_charset     varchar2(30)
, tcp_newline     varchar2(2)
, tcp_tx_timeout  integer
, tcp_private_sd  integer

  /* Fields of type epc_clnt.http_connection_rectype */
, http_url varchar2(4000) /* http://... */
, http_method varchar2(10)
, http_version varchar2(10)

  /* Fields used for dbms_pipe */
, request_pipe varchar2(128) /*epc.pipe_name_subtype*/
, send_timeout integer
, recv_timeout integer
, max_pipe_size integer -- defaults to 8192
, private integer -- default to 1 (true)

, constructor function epc_clnt_object
  ( self in out nocopy epc_clnt_object
  , p_interface_name in varchar2
  )
  return self as result

, overriding member function name(self in epc_clnt_object) return varchar2

  -- every sub type must add its attributes (in capital letters)
, overriding member procedure serialize(self in epc_clnt_object, p_json_object in out nocopy json_object_t)

) final;
/

