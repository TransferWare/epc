/*
REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.7  2005/01/03 12:26:44  gpaulissen
REMARK  Release 4.4.0
REMARK
REMARK  Revision 1.6  2004/12/28 12:18:11  gpaulissen
REMARK  Test on Amazon
REMARK
REMARK  Revision 1.5  2004/12/16 18:43:08  gpaulissen
REMARK  generated HTML added
REMARK
REMARK  Revision 1.4  2004/12/16 17:50:31  gpaulissen
REMARK  REMARK blocks commented for PLDoc
REMARK
REMARK  Revision 1.3  2004/04/21 11:16:54  gpaulissen
REMARK  .
REMARK
REMARK  Revision 1.2  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK
*/

create or replace package epc is
/**
--
-- This package is used to implement RPC like functionality on Oracle.
-- Messages are sent by the client to a server. The transport mechanisms
-- supported are database pipes (package DBMS_PIPE), HTTP (package UTL_HTTP) 
-- and TCP/IP (package UTL_TCP).
--
-- @headcom
*/

/* 
|| SUBTYPES
*/

/* see $EPC_HOME/src/lib/idl_defs.h */
subtype interface_name_subtype is varchar2(32);
subtype namespace_subtype is varchar2(128);
subtype method_name_subtype is varchar2(512);
subtype parameter_name_subtype is varchar2(32);
subtype data_type_subtype is pls_integer;
subtype parameter_mode_subtype is pls_integer;
subtype pipe_name_subtype is varchar2(128);

subtype int_subtype is integer;
subtype long_subtype is integer;
subtype float_subtype is float;
subtype double_subtype is double precision;
subtype string_subtype is varchar2(32767);
subtype xml_subtype is varchar2(32767);

/* 
|| CONSTANTS
*/

"xmlns:SOAP-ENV" constant varchar2(1000) := 
  'xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"';

SOAP_HEADER_START constant varchar2(1000) :=
  '<?xml version="1.0" encoding="UTF-8"?>'
  ||'<SOAP-ENV:Envelope'
  ||' '
  ||'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
  ||' '
  ||'xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"'
  ||' '
  ||"xmlns:SOAP-ENV"
  ||' '
  ||'xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
  ||' '
  ||'SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"'
  ||'>'
  ||'<SOAP-ENV:Body>';

SOAP_HEADER_END constant varchar2(1000) := 
  '</SOAP-ENV:Body></SOAP-ENV:Envelope>';

/* 
|| EXCEPTIONS
*/

e_illegal_null_value exception;
e_wrong_protocol     exception;
e_comm_error         exception;
e_msg_timed_out      exception;
e_msg_too_big        exception;
e_msg_interrupted    exception;

/*ORA-06558 is raised if the message buffer overflows (currently 4096 bytes)*/

/**
-- Get the xml data type constant.
*/
function data_type_xml
return data_type_subtype;

/**
-- Get the string data type constant.
*/
function data_type_string
return data_type_subtype;

/**
-- Get the int data type constant.
*/
function data_type_int
return data_type_subtype;

/**
-- Get the long data type constant.
*/
function data_type_long
return data_type_subtype;

/**
-- Get the float data type constant.
*/
function data_type_float
return data_type_subtype;

/**
-- Get the double data type constant.
*/
function data_type_double
return data_type_subtype;

/**
-- Print XML data.
--
-- Pretty prints an XML message using DBMS_OUTPUT.
-- 
-- @param p_msg  XML message.
*/
procedure print
(
  p_msg in varchar2
);

end epc;
/
