#include <stdio.h>
#include <string.h>

void proc01 ( char *i_par1, char *io_par2, char *o_par3 )
{
	printf( "proc01\ni_par1: %s\nio_par2: %s\n", i_par1, io_par2 );
	strcpy( io_par2, "hjfgeljaujfd" );
	strcpy( o_par3, "dfgfhahfmkghjfvsbvfhjfgeljaujfd" );
	printf( "io_par2: %s\no_par3: %s\n\n", io_par2, o_par3 );
}

int proc02 ( int *io_par1, int *o_par2, int i_par3 )
{
	printf( "proc02\nio_par1: %d\ni_par3: %d\n", *io_par1, i_par3 );
	*io_par1 = 100;
	*o_par2 = 199;
	printf( "io_par1: %d\no_par2: %d\n\n", *io_par1, *o_par2 );
	return -1;
}

double proc03 ( double *o_par1, double i_par2, double *io_par3 )
{
	printf( "proc03\ni_par2: %lf\nio_par3: %lf\n", i_par2, *io_par3 );
	*o_par1 = 100.9998;
	*io_par3 = 13478.54754;
	printf( "o_par1: %lf\no_par3: %lf\n\n", *o_par1, *io_par3 );
	return 3473.686;
}

float proc04 ( float i_par1, float *io_par2, float *o_par3 )
{
	printf( "proc04\ni_par1: %f; io_par2: %f\n", i_par1, *io_par2 );
	*io_par2 = 33.99;
	*o_par3 = -1.88;
	printf( "proc04\nio_par2: %f; o_par3: %f\n", *io_par2, *o_par3 );
	return 33.45;
}
