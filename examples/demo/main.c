#include <stdlib.h>
#include <epc.h>
#include <demo.h>
#include <errno.h>

int do_system_call( char *cmd )
{
  return system( cmd );
}

int main( int argc, char **argv )
{
  return epc__main( argc, argv, &ifc_demo );
}
