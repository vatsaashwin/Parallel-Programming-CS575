#include "simd.p7.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

#define SIZE 32768
float Array[2*SIZE];
float  Sums[1*SIZE];


int main(int argc, char const *argv[])
{
	
	FILE *fp = fopen( "signal.txt", "r" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open file 'signal.txt'\n" );
	}

	int Size;
	fscanf( fp, "%d", &Size );
	Size = SIZE;

	for( int i = 0; i < Size; i++ )
	{
		fscanf( fp, "%f", &Array[i] );
		Array[i+Size] = Array[i];		// duplicate the array
	}
	fclose( fp );

	int shift;
	

	double time0=omp_get_wtime();
	for( shift = 0; shift < Size; shift++ )
	{
		float sum = 0.;
		Sums[shift] = SimdMulSum( &Array[0], &Array[0+shift], Size );
		//printf("%f\n",Sums[shift]);
	}
	double time1=omp_get_wtime();
	
	
	for( shift = 0; shift < 512; shift++ )
	{
	printf("%f\n",Sums[shift] );
	}
	printf("Performance:%lf\n", (double)(Size*Size)/(time1-time0)/1000000.);
return 0;
}
