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
		epc.empty_pipe( r_pipe.name, i_timeout );
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
