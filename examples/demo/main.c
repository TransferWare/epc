#include <stdlib.h>
#include <epc.h>
#include <demo.h>

void do_system_call( char *cmd )
{
        system( cmd );
}

int main( int argc, char **argv )
{
        return epc_main( argc, argv, &ifc_demo );
}
