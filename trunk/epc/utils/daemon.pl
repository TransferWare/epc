use warnings;
use strict;

use Carp;
use File::Spec;
use POSIX qw(setsid ENOENT);

# prototypes
sub main ();
sub daemonize ();

main();

sub main () {
    daemonize();

    while (1) {
	defined(my $pid = fork)   or croak("Can't fork: $!");
	if ($pid != 0) {
	    wait(); # wait for the child to die
	} else {
	    exec("@ARGV") or croak("Can't exec: $!");
	}
    }
}

sub daemonize () {
    defined(my $pid = fork)   or croak("Can't fork: $!");
    exit if $pid;

    setsid                    or croak("Can't start a new session: $!");
    open STDERR, '>&STDOUT'   or croak("Can't dup stdout: $!");
}
