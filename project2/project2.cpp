#include <omp.h>
#include <stdio.h>
#include <math.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT        1 //change this value to run for 1, 2, 4, 6, 8
#endif

#define XMIN     0.
#define XMAX     3.
#define YMIN     0.
#define YMAX     3.

#define TOPZ00  0.
#define TOPZ10  1.
#define TOPZ20  0.
#define TOPZ30  0.

#define TOPZ01   1.
#define TOPZ11  12.
#define TOPZ21   1.
#define TOPZ31   0.

#define TOPZ02  0.
#define TOPZ12  1.
#define TOPZ22  0.
#define TOPZ32  4.

#define TOPZ03  3.
#define TOPZ13  2.
#define TOPZ23  3.
#define TOPZ33  3.

#define BOTZ00  0.
#define BOTZ10  -3.
#define BOTZ20  0.
#define BOTZ30  0.

#define BOTZ01  -2.
#define BOTZ11  10.
#define BOTZ21  -2.
#define BOTZ31  0.

#define BOTZ02  0.
#define BOTZ12  -5.
#define BOTZ22  0.
#define BOTZ32  -6.

#define BOTZ03  -3.
#define BOTZ13   2.
#define BOTZ23  -8.
#define BOTZ33  -3.


float Height( int iu, int iv, int number_nodes )    // iu,iv = 0 .. NUMNODES-1
{
    float u = (float)iu / (float)(number_nodes-1);
    float v = (float)iv / (float)(number_nodes-1);

    // the basis functions:

    float bu0 = (1.-u) * (1.-u) * (1.-u);
    float bu1 = 3. * u * (1.-u) * (1.-u);
    float bu2 = 3. * u * u * (1.-u);
    float bu3 = u * u * u;

    float bv0 = (1.-v) * (1.-v) * (1.-v);
    float bv1 = 3. * v * (1.-v) * (1.-v);
    float bv2 = 3. * v * v * (1.-v);
    float bv3 = v * v * v;

    // finally, we get to compute something:


    float top =       bu0 * ( bv0*TOPZ00 + bv1*TOPZ01 + bv2*TOPZ02 + bv3*TOPZ03 )
    + bu1 * ( bv0*TOPZ10 + bv1*TOPZ11 + bv2*TOPZ12 + bv3*TOPZ13 )
    + bu2 * ( bv0*TOPZ20 + bv1*TOPZ21 + bv2*TOPZ22 + bv3*TOPZ23 )
    + bu3 * ( bv0*TOPZ30 + bv1*TOPZ31 + bv2*TOPZ32 + bv3*TOPZ33 );

    float bot =       bu0 * ( bv0*BOTZ00 + bv1*BOTZ01 + bv2*BOTZ02 + bv3*BOTZ03 )
    + bu1 * ( bv0*BOTZ10 + bv1*BOTZ11 + bv2*BOTZ12 + bv3*BOTZ13 )
    + bu2 * ( bv0*BOTZ20 + bv1*BOTZ21 + bv2*BOTZ22 + bv3*BOTZ23 )
    + bu3 * ( bv0*BOTZ30 + bv1*BOTZ31 + bv2*BOTZ32 + bv3*BOTZ33 );

    return top - bot;    // if the bottom surface sticks out above the top surface
    // then that contribution to the overall volume is negative
}


int main( int argc, char *argv[ ] )
{
    #ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here.\n" );
        return 1;
    #endif

    omp_set_num_threads( NUMT );
    fprintf( stderr, "Using %d threads\n", NUMT );


    //int numnodes[] = { 500, 1000, 2500, 5000, 10000, 15000, 25000, 35000};

    for (int number_nodes=500 ; number_nodes <= 40000; number_nodes += 5000)
    {
        // the area of a single full-sized tile:
        float fullTileArea = (  ( ( XMAX - XMIN )/(float)(number_nodes-1) )  *
                              ( ( YMAX - YMIN )/(float)(number_nodes-1) )  );

        // sum up the weighted heights into the variable "volume"
        // using an OpenMP for loop and a reduction:
        double sum = 0.;
        double sumMegaMults = 0.;
        double time0 = omp_get_wtime( );
        double end = number_nodes-1;

        #pragma omp parallel for default(none), shared(fullTileArea, number_nodes, end), reduction(+:sum)
        for( int i = 0; i <= number_nodes*number_nodes; i++ )
        {
            int iu = i % number_nodes;
            int iv = i / number_nodes;
            double w = 1.;
            if ((iu == 0 && iv == 0) || (iu == 0 && iv == end) || (iu == end && iv == 0) || (iu == end && iv == end))
                w = 0.25;
            else if ((iu == 0 && iv!=end && iv !=0) || (iu == end && iv!=end && iv != 0) || (iv == 0 && iu!=end && iu != 0) || (iv == end && iu!=end && iu != 0))
                w = 0.5;
            sum += Height(iu, iv, number_nodes) * w * fullTileArea;
         }

        double time1 = omp_get_wtime( );
        double time = time1 - time0;
        double megaMults = (double)(number_nodes*number_nodes)/(time)/1000000.;
        sumMegaMults += megaMults;
        printf("Number Nodes = %d \n", number_nodes);
        printf("Result = %10.2lf \n", sum);
        printf("Performance = %10.2lf megaMults/Sec\n", sumMegaMults);
    }
}


