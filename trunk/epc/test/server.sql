rem $Header$
rem
rem Parameters:
rem 1 - true (empty pipe) or false (act as server)
rem

set serveroutput on size 1000000
set verify off

declare
  subtype pipe_name_t is varchar2(128);

  v_request_pipe constant pipe_name_t := 'EPC_REQUEST_PIPE';
  v_result_pipe pipe_name_t;
  v_send_wait_time pls_integer := 10;
  v_receive_wait_time pls_integer := 10;

  retval binary_integer;
  v_interface_name varchar2(1000);
  v_routine_name varchar2(1000);

  comm_error exception;

  function empty_pipe
  (
    i_pipe_name in pipe_name_t
  , i_timeout in pls_integer
  )
  return number
  is
    /* named constants for the different codes of pipe types */
    date_type constant pls_integer := 12;
    string_type constant pls_integer := 9;
    number_type constant pls_integer := 6;

    /* variables to hold the items coming out of the pipe */
    my_date date;
    my_string varchar2(32767);
    my_number number(38);

    /* variables to hold message type */
    msg_type pls_integer;
    msg_nr pls_integer := 0;

    retval number;
  begin
    dbms_output.put_line( 'emptying pipe ' || i_pipe_name );

    <<receive_message_loop>>
    loop
      /* read a message from the named pipe. */
      retval := dbms_pipe.receive_message(i_pipe_name, i_timeout);
      if retval = 0
      then
        /* 
        || if successful, determine the datatype of the first item
        || and then call unpack_message with the right kind of 
        || variable to match that datatype.
        */
        msg_nr := msg_nr + 1;
        begin
          dbms_output.put_line('message: ' || to_char(msg_nr));
        exception
          when    others
          then    null;
        end;

        <<unpack_message_loop>>
        loop
        begin
          msg_type := dbms_pipe.next_item_type;
          if msg_type = date_type
          then
            dbms_pipe.unpack_message(my_date);
            dbms_output.put_line('date  : ' || to_char(my_date, 'YYYYMMDDHH24MISS'));
          elsif msg_type = string_type
          then
            dbms_pipe.unpack_message(my_string);
            dbms_output.put_line('string: ' || my_string);
          elsif msg_type = number_type
          then
            dbms_pipe.unpack_message(my_number);
            dbms_output.put_line('number: ' || to_char(my_number));
          elsif msg_type <> 0
          then
            dbms_output.put_line('unknown next item type: ' || to_char(msg_type));
            /* exit unpack_message_loop; */
          else
            exit unpack_message_loop;
          end if;
        exception
          when    others
          then    null;
        end;
        end loop unpack_message_loop;
      else
        exit receive_message_loop;
      end if;
    end loop receive_message_loop;
    return retval;
  end empty_pipe;

  procedure proc01
  is
    i_par1 epc.string_subtype;
    io_par2 epc.string_subtype;
    o_par3 epc.string_subtype;
    v_result epc.string_subtype;
  begin
--/*DBUG*/    dbms_output.put_line( 'proc01' );

    dbms_pipe.unpack_message(i_par1);
    dbms_pipe.unpack_message(io_par2);

    io_par2 := 'hjfgeljaujfd';
    o_par3 := 'dfgfhahfmkghjfvsbvfhjfgeljaujfd';
    v_result := 'abcd';

    dbms_pipe.pack_message(io_par2);
    dbms_pipe.pack_message(o_par3);
    dbms_pipe.pack_message(v_result);
  end proc01;

  procedure proc02
  is
    io_par1 epc.int_subtype;
    o_par2 epc.int_subtype;
    i_par3 epc.int_subtype;
    v_result epc.int_subtype;
  begin
--/*DBUG*/    dbms_output.put_line( 'proc02' );

    dbms_pipe.unpack_message(io_par1);
    dbms_pipe.unpack_message(i_par3);

    io_par1 := 100;
    o_par2 := 199;
    v_result := -1;

    dbms_pipe.pack_message(io_par1);
    dbms_pipe.pack_message(o_par2);
    dbms_pipe.pack_message(v_result);
  end proc02;

  procedure proc03
  is
    o_par1 epc.double_subtype;
    i_par2 epc.double_subtype;
    io_par3 epc.double_subtype;
    v_result epc.double_subtype;
  begin
--/*DBUG*/    dbms_output.put_line( 'proc03' );

    dbms_pipe.unpack_message(i_par2);
    dbms_pipe.unpack_message(io_par3);

    o_par1 := 100.9998;
    io_par3 := 13478.54754;
    v_result := 3473.686;

    dbms_pipe.pack_message(o_par1);
    dbms_pipe.pack_message(io_par3);
    dbms_pipe.pack_message(v_result);
  end proc03;

  procedure proc04
  is
    i_par1 epc.float_subtype;
    io_par2 epc.float_subtype;
    o_par3 epc.float_subtype;
    v_result epc.float_subtype;
  begin
--/*DBUG*/    dbms_output.put_line( 'proc04' );

    dbms_pipe.unpack_message(i_par1);
    dbms_pipe.unpack_message(io_par2);

    io_par2 := 33.99;
    o_par3 := -1.88;
    v_result := 33.45;

    dbms_pipe.pack_message(io_par2);
    dbms_pipe.pack_message(o_par3);
    dbms_pipe.pack_message(v_result);
  end proc04;

  procedure nothing1
  is
  begin
--/*DBUG*/    dbms_output.put_line( 'nothing1' );

    null;
  end nothing1;

  procedure nothing2
  is
  begin
--/*DBUG*/    dbms_output.put_line( 'nothing2' );
    null;
  end nothing2;

  procedure request_get_header
  (
    o_interface out varchar2
  , o_routine_name out varchar2
  , o_result_pipe out pipe_name_t
  )
  is
  begin
    dbms_pipe.unpack_message( o_interface );
    dbms_pipe.unpack_message( o_routine_name );
    dbms_pipe.unpack_message( o_result_pipe );

--/*DBUG*/    dbms_output.put_line( 'interface: '||o_interface );
--/*DBUG*/    dbms_output.put_line( 'routine: '||o_routine_name );
--/*DBUG*/    dbms_output.put_line( 'result pipe: '||o_result_pipe );

  end request_get_header;

begin
  if &1
  then
    retval := empty_pipe( v_request_pipe, 0 );
  else
    loop
      retval := dbms_pipe.receive_message( v_request_pipe, v_receive_wait_time );

      if retval != 0
      then
        dbms_output.put_line( 'Error receiving message from ' || 
                              v_request_pipe || '; ' ||
                              'retval: ' || to_char(retval) );
        raise comm_error;
      end if;

      request_get_header(v_interface_name, v_routine_name, v_result_pipe);

      begin
        if v_routine_name like '%proc01%'
        then
          proc01;
        elsif v_routine_name like '%proc02%'
        then
          proc02;
        elsif v_routine_name like '%proc03%'
        then
          proc03;
        elsif v_routine_name like '%proc04%'
        then
          proc04;
        elsif v_routine_name like '%nothing1%'
        then
          nothing1;
        elsif v_routine_name like '%nothing2%'
        then
          nothing2;
          v_routine_name := null;
        else
          v_routine_name := null;      
        end if;
      exception
        when others
        then
          dbms_output.put_line( 'Routine ' || v_routine_name );
          raise;
      end; 

--      v_routine_name := null;

      if v_routine_name is not null and
         v_result_pipe is not null and
         v_result_pipe != 'N/A'
      then
        retval := dbms_pipe.send_message( v_result_pipe, 1 );

--/*DBUG*/        dbms_output.put_line( 'send message for ' ||
--/*DBUG*/                              v_result_pipe || ': ' ||
--/*DBUG*/                              to_char(retval) );

        if retval != 0
        then
          dbms_output.put_line( 'Error sending message through ' || 
                                v_result_pipe || '; ' ||
                                'retval: ' || to_char(retval) );
          raise comm_error;
        end if;
      end if;
    end loop;
  end if;
exception
  when others
  then
    dbms_output.put_line( substr(sqlerrm, 1, 255) );
    retval := empty_pipe( v_request_pipe, 0 );    
    retval := empty_pipe( v_result_pipe, 0 );    
    raise;
end;
/
