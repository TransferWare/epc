REMARK $Id$

rem WHENEVER SQLERROR EXIT FAILURE

SET TERMOUT OFF

/*
  The following documentation uses the Perl pod format. A html file
  can be constructed by: 

        pod2html --infile=empty_pipes.prc --outfile=empty_pipes.html

*/

/* 

=pod

=head1 NAME

empty_pipes - Stored procedure to display and empty user defined pipes (created via Oracle package DBMS_PIPE).

=head1 SYNOPSIS

=cut

*/

SET TERMOUT ON

=pod

CREATE OR REPLACE
PROCEDURE
empty_pipes( i_pipe_name_wildcard IN VARCHAR2 := '%', i_timeout IN INTEGER := 0 )

-- =cut

IS
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

SET TERMOUT OFF

/*

=pod

=head1 DESCRIPTION

The I<EMPTY_PIPES> procedure allows you to empty user defined pipes (not
starting with 'ORA$') and display its contents using the Oracle I<DBMS_OUTPUT>
package.

The input parameters for this procedure are:

=over 4

=item i_pipe_name_wildcard

The pipe name to empty. Wildcards are allowed. The default is to empty and display all user pipes.

=item i_timeout

The time in seconds to wait for a message in I<DBMS_PIPE.RECEIVE_MESSAGE>. The default timeout of 0 means that this procedure will not wait for messages in a pipe.

=back

=head1 NOTES

The I<empty_pipes> procedure uses the Oracle package I<DBMS_PIPE> and the dictionary view I<V$DB_PIPES>, which is notmally a synonym for I<V_$DB_PIPES>. Access to these objects is necessary for this procedure to compile correctly.

    SQL> connect sys
    SQL> grant execute on dbms_pipe to &&account;
    SQL> REMARK Use fixed view name not the synonym name.
    SQL> grant select on v_$db_pipes to &&account; 
    SQL> connect &&account
    SQL> @ empty_pipes.prc

=head1 EXAMPLES

    SQL> set serveroutput on size 1000000
    SQL> exec empty_pipes;

=head1 AUTHOR

Gert-Jan Paulissen, E<lt>gpaulissen@transfer-solutions.comE<gt>.

=head1 BUGS

=head1 SEE ALSO

=head1 COPYRIGHT

All rights reserved by Transfer Solutions b.v.

=cut

*/
