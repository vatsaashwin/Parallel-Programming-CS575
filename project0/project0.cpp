//Submission by Aashwin Vats                             

#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT             4  //change to 1 while using single thread
#define ARRAYSIZE 6000    
#define NUMTRIES        12    

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main( )
{
#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
    return 1;
#endif

    omp_set_num_threads( NUMT );
    fprintf( stderr, "Using %d threads\n", NUMT );

    double maxMegaMults = 0.;

    for( int t = 0; t < NUMTRIES; t++ )
    {
      double time0 = omp_get_wtime( );

#pragma omp parallel for
        for( int i = 0; i < ARRAYSIZE; i++ )
        {
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime( );
        double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
        if( megaMults > maxMegaMults )
            maxMegaMults = megaMults;
        double execTime = time1 - time0;
        printf("Exec time = %f Secs\n", execTime);
    }



    printf( "Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
    // note: %lf stands for "long float", which is how printf prints a "double"
    //        %d stands for "decimal integer", not "double"

    return 0;
}



