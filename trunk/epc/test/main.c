#include <stdlib.h>
#include <epc.h>
#include "epctest.h"
#include "str.h"

int main( int argc, char **argv )
{
  return epc__list_main( argc, argv, &ifc_str, &ifc_epctest, NULL );
}
