/*
REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.8  2005/01/03 13:06:31  gpaulissen
REMARK  Release 4.4.0
REMARK
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

subtype interface_name_subtype is varchar2(32);
subtype namespace_subtype is varchar2(128);
subtype method_name_subtype is varchar2(512);
subtype parameter_name_subtype is varchar2(32);
subtype data_type_subtype is pls_integer;
subtype parameter_mode_subtype is pls_integer;
subtype pipe_name_subtype is varchar2(128);

/* Start of backwards compatibility for subtypes. See epc.pls (package 4.0.0). */
subtype datatype_t is data_type_subtype;
subtype parameter_mode_t is parameter_mode_subtype;
subtype pipe_name_t is pipe_name_subtype;
/* End of backwards compatibility for subtypes. */

subtype int_subtype is integer;
subtype long_subtype is integer;
subtype float_subtype is float;
subtype double_subtype is double precision;
subtype string_subtype is varchar2(32767);
subtype xml_subtype is varchar2(32767);
subtype date_subtype is date; /* ISO date format, e.g. 19980717T14:08:55 */

/* numbers for each data_type function: see idl_defs.h */
data_type_string constant data_type_subtype := 1;
data_type_int    constant data_type_subtype := 2;
data_type_long   constant data_type_subtype := 3;
data_type_float  constant data_type_subtype := 4;
data_type_double constant data_type_subtype := 5;
data_type_xml    constant data_type_subtype := 7;
data_type_date   constant data_type_subtype := 8;

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
  Exceptions are raised using raise_application_error(error_number, ...),
  which expects error numbers in the range -20000 uptill -20999.
  The EPC uses -20100 uptill -20106.
  
  We have to map exceptions to an error number using the pragma lines,
  otherwise the exceptions starting with e_ (new naming convention) and
  without the e_ (old naming convention) would be distinct. So for backwards
  compatibility we must map them.

  Proof: run the following PL/SQL block:

  declare
    e_something exception;
  --  pragma exception_init(e_something, -20000);
    something exception;
  --  pragma exception_init(something, -20000);
  begin
    raise e_something;
  exception
    when something
    then
      dbms_output.put_line('something');
      null;     
  end;
  
  This does not catch e_something. 

  When you uncomment the pragma lines, the exception is catched.
*/
e_illegal_null_value exception;
c_illegal_null_value constant pls_integer := -20100;
pragma exception_init(e_illegal_null_value, -20100);

e_wrong_protocol     exception;
c_wrong_protocol     constant pls_integer := -20101;
pragma exception_init(e_wrong_protocol, -20101);

e_comm_error         exception;
c_comm_error         constant pls_integer := -20102;
pragma exception_init(e_comm_error, -20102);

e_msg_timed_out      exception;
c_msg_timed_out      constant pls_integer := -20103;
pragma exception_init(e_msg_timed_out, -20103);

e_msg_too_big        exception;
c_msg_too_big        constant pls_integer := -20104;
pragma exception_init(e_msg_too_big, -20104);

e_msg_interrupted    exception;
c_msg_interrupted    constant pls_integer := -20105;
pragma exception_init(e_msg_interrupted, -20105);

e_parse_error        exception;
c_parse_error        constant pls_integer := -20106;
pragma exception_init(e_parse_error, -20106);

/* Start of backwards compatibility for exceptions. See epc.pls (package 4.0.0). */
illegal_null_value exception;
pragma exception_init(illegal_null_value, -20100);

wrong_protocol     exception;
pragma exception_init(wrong_protocol, -20101);

comm_error         exception;
pragma exception_init(comm_error, -20102);

msg_timed_out      exception;
pragma exception_init(msg_timed_out, -20103);

msg_too_big        exception;
pragma exception_init(msg_too_big, -20104);

msg_interrupted    exception;
pragma exception_init(msg_interrupted, -20105);
/* End of backwards compatibility for exceptions. */

/*ORA-06558 is raised if the message buffer overflows (currently 4096 bytes)*/

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

show errors
