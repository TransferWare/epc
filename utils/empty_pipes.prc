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
/*POD

=cut

POD*/
    IS
	PROCEDURE empty_pipe
	(
		i_pipe_name IN VARCHAR2
	,	i_timeout IN INTEGER
	)
	IS
		/* Named constants for the different codes of pipe types */
		date_type CONSTANT INTEGER := 12;
		string_type CONSTANT INTEGER := 9;
		number_type CONSTANT INTEGER := 6;

		/* Variables to hold the items coming out of the pipe */
		my_date DATE;
		my_string VARCHAR2(2000);
		my_number NUMBER;

		/* Variables to hold message type */
		msg_type INTEGER;
		msg_nr INTEGER := 0;
	BEGIN
		dbms_output.put_line( 'Emptying pipe ' || i_pipe_name );
		LOOP
			/* Read a message from the named pipe. */
			IF DBMS_PIPE.RECEIVE_MESSAGE (i_pipe_name, i_timeout) = 0
			THEN
				/* 
				|| If successful, determine the datatype of the first item
				|| and then call UNPACK_MESSAGE with the right kind of 
				|| variable to match that datatype.
				*/
				msg_nr := msg_nr + 1;
				BEGIN
					dbms_output.put_line('Message: ' || to_char(msg_nr));
				EXCEPTION
					WHEN	OTHERS
					THEN	NULL;
				END;
	
				LOOP
				BEGIN
					msg_type := DBMS_PIPE.NEXT_ITEM_TYPE;
					IF msg_type = date_type
					THEN
						DBMS_PIPE.UNPACK_MESSAGE(my_date);
						dbms_output.put_line('Date  : ' || to_char(my_date, 'YYYYMMDDHH24MISS'));
					ELSIF msg_type = string_type
					THEN
						DBMS_PIPE.UNPACK_MESSAGE(my_string);
						dbms_output.put_line('String: ' || my_string);
					ELSIF msg_type = number_type
					THEN
						DBMS_PIPE.UNPACK_MESSAGE(my_number);
						dbms_output.put_line('Number: ' || to_char(my_number));
					ELSE
						EXIT;
					END IF;
				EXCEPTION
					WHEN	OTHERS
					THEN	NULL;
				END;
				END LOOP;
			ELSE
				EXIT;
			END IF;
		END LOOP;
	END empty_pipe;
BEGIN
	FOR r_pipe IN
	(
		SELECT	name
		FROM	v$db_pipes
		WHERE	name LIKE upper(i_pipe_name_wildcard)
		AND	name NOT LIKE 'ORA$%' /* no Oracle pipes */
	)
	LOOP
	BEGIN
		empty_pipe( r_pipe.name, i_timeout );
	EXCEPTION
		WHEN OTHERS
		THEN
			dbms_output.put_line( substr(SQLERRM, 1, 255) );
	END;		     
	END LOOP;
END;
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
