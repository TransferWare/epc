set serveroutput on size unlimited format trunc
set define on

declare
  i_pipe_name_wildcard varchar2(128) := '&&pipe_name';
  i_timeout integer := to_number('&&timeout');

  procedure empty_pipe
  (
    i_pipe_name in varchar2
  , i_timeout in pls_integer
  )
  is
    /* variables to hold the items coming out of the pipe */
    my_date date;
    my_string varchar2(32767);
    my_number number;

    item_type pls_integer;
    msg_nr pls_integer := 0;
  begin
    dbms_output.put_line( 'emptying pipe ' || i_pipe_name );

    <<receive_message_loop>>
    loop
      /* read a message from the named pipe. */
      if dbms_pipe.receive_message (i_pipe_name, i_timeout) = 0
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
          item_type := dbms_pipe.next_item_type;
          /* See METALINK for item types 1 and 2 */
          if item_type = 12
          then
            dbms_pipe.unpack_message(my_date);
            dbms_output.put_line('date  : ' || to_char(my_date, 'YYYYMMDDHH24MISS'));
          elsif item_type in (1, 9)
          then
            dbms_pipe.unpack_message(my_string);
            dbms_output.put_line(substr('string: ' || my_string, 1, 255));
          elsif item_type in (2, 6)
          then
            dbms_pipe.unpack_message(my_number);
            dbms_output.put_line('number: ' || to_char(my_number));
          else
            exit unpack_message_loop;
          end if;
        exception
          when others
          then null;
        end;
        end loop unpack_message_loop;
      else
        exit receive_message_loop;
      end if;
    end loop receive_message_loop;
  end empty_pipe;

begin
  for r_pipe in
  (
    select  name
    from    v$db_pipes
    where   name like upper(i_pipe_name_wildcard)
    and     name not like 'ORA$%' /* no Oracle pipes */
  )
  loop
    begin
      empty_pipe( r_pipe.name, i_timeout );
      if dbms_pipe.remove_pipe( r_pipe.name ) = 0
      then
        null;
      end if;
    exception
      when others
      then
        dbms_output.put_line( substr(sqlerrm, 1, 255) );
    end;                 
  end loop;
end;
/
