// Array multiplication: C = A * B:

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		16		// number of threads per block
#endif

#ifndef SIZE
#define SIZE			16*1024	// array size
#endif

#ifndef TOLERANCE
#define TOLERANCE		0.00001f	// tolerance to relative error
#endif

// ranges for the random numbers:
const float XCMIN =	 0.0;
const float XCMAX =	 2.0;
const float YCMIN =	 0.0;
const float YCMAX =	 2.0;
const float RMIN  =	 0.5;
const float RMAX  =	 2.0;

// monte carlo (CUDA Kernel) on the device
__global__  void MonteCarlo( float *xcs, float *ycs, float *rs, int *numHits )
{
    __shared__ int prods[BLOCKSIZE];

	unsigned int numItems = blockDim.x;
	unsigned int tnum = threadIdx.x;
	unsigned int wgNum = blockIdx.x;
	unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;

    float xc = xcs[gid];
    float yc = ycs[gid];
    float  r =  rs[gid];

    // solve for the intersection using the quadratic formula:
    float a = 2.;
    float b = -2.*( xc + yc );
    float c = xc*xc + yc*yc - r*r;
    float d = b*b - 4.*a*c;

    if( d < 0. )
    {
        prods[tnum] = 0;
    }
    else
    {
        // hits the circle:
        d = sqrt( d );
        float t1 = (-b + d ) / ( 2.*a );	
        float t2 = (-b - d ) / ( 2.*a );	
        float tmin = t1 < t2 ? t1 : t2;		

        if( tmin < 0. )
        {
            prods[tnum] = 0;
        }
        else
        {

            float xcir = tmin;
            float ycir = tmin;


            float nx = xcir - xc;
            float ny = ycir - yc;
            float n = sqrt( nx*nx + ny*ny );
            nx /= n;	
            ny /= n;	


            float inx = xcir - 0.;
            float iny = ycir - 0.;
            float in = sqrt( inx*inx + iny*iny );
            inx /= in;	// unit vector
            iny /= in;	// unit vector

            // get the outgoing (bounced) vector:
            float dot = inx*nx + iny*ny;
            float outx = inx - 2.*nx*dot;	
            float outy = iny - 2.*ny*dot;	

            // find out if it hits the infinite plate:
            float t = ( 0. - ycir ) / outy;

            if( t < 0. )
            {
                prods[tnum] = 0;
            }
            else
            {
                prods[tnum] = 1;
            }
        }
    }

	for (int offset = 1; offset < numItems; offset *= 2)
	{
		int mask = 2 * offset - 1;
		__syncthreads();
		if ((tnum & mask) == 0)
		{
			prods[tnum] += prods[tnum + offset];
		}
	}

	__syncthreads();
	if (tnum == 0)
		numHits[wgNum] = prods[0];
}

// helper functions
float Ranf( float low, float high )
{
        float r = (float) rand();               // 0 - RAND_MAX
        float t = r  /  (float) RAND_MAX;       // 0. - 1.

        return   low  +  t * ( high - low );
}

int Ranf( int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = ceil( (float)ihigh );

        return (int) Ranf(low,high);
}

// main program:

int
main( int argc, char* argv[ ] )
{
	int dev = findCudaDevice(argc, (const char **)argv);

	// allocate host memory:
    
	float * hxcs = new float [ SIZE ];
	float * hycs = new float [ SIZE ];
	float * hrs = new float [ SIZE ];
    int * hnumHits = new int [ SIZE/BLOCKSIZE ];
    
    // fill the random-value arrays:
    for( int n = 0; n < SIZE; n++ )
    {
        hxcs[n] = Ranf( XCMIN, XCMAX );
        hycs[n] = Ranf( YCMIN, YCMAX );
        hrs[n] = Ranf(  RMIN,  RMAX );
    }
    
	// allocate device memory:

    float *dxcs, *dycs, *drs;
    int *dnumHits;
    
	dim3 dimsxcs( SIZE, 1, 1 );
	dim3 dimsycs( SIZE, 1, 1 );
	dim3 dimsrs( SIZE, 1, 1 );
    dim3 dimsnumHits( SIZE, 1, 1);
    

	cudaError_t status;

	status = cudaMalloc( reinterpret_cast<void **>(&dxcs), SIZE*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dycs), SIZE*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&drs), SIZE*sizeof(float) );
		checkCudaErrors( status );
    status = cudaMalloc( reinterpret_cast<void **>(&dnumHits), (SIZE/BLOCKSIZE)*sizeof(int) );
		checkCudaErrors( status );


	// copy host memory to the device:

	status = cudaMemcpy( dxcs, hxcs, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dycs, hycs, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
    status = cudaMemcpy( drs, hrs, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( SIZE / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	status = cudaEventCreate( &start );
		checkCudaErrors( status );
	status = cudaEventCreate( &stop );
		checkCudaErrors( status );

	// record the start event:

	status = cudaEventRecord( start, NULL );
		checkCudaErrors( status );

	// execute the kernel:

    MonteCarlo<<< grid, threads >>>( dxcs, dycs, drs, dnumHits );

	// record the stop event:

	status = cudaEventRecord( stop, NULL );
		checkCudaErrors( status );

	// wait for the stop event to complete:

	status = cudaEventSynchronize( stop );
		checkCudaErrors( status );

	float msecTotal = 0.0f;
	status = cudaEventElapsedTime( &msecTotal, start, stop );
		checkCudaErrors( status );

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double trialsPerSecond = (float)SIZE / secondsTotal;
	double megaTrialsPerSecond = trialsPerSecond / 1000000.;
	fprintf( stderr, "SIZE = %10d, MegaTrials/Second = %10.2lf\n", SIZE, megaTrialsPerSecond );

	// copy result from the device to the host:

	status = cudaMemcpy( hnumHits, dnumHits, (SIZE/BLOCKSIZE)*sizeof(int), cudaMemcpyDeviceToHost );
		checkCudaErrors( status );
    
    int sum = 0.;
	for(int i = 0; i < SIZE/BLOCKSIZE; i++ )
	{
		sum += hnumHits[i];
	}
    float prob = (float)(sum) / (float)(SIZE);
    fprintf( stderr, "\nNumHit=%10d\n", sum );
	fprintf( stderr, "\nProbability=%8.4lf\n", prob );
    
	// clean up memory:
    delete [ ] hxcs;
    delete [ ] hycs;
    delete [ ] hrs;
    delete [ ] hnumHits;

    status = cudaFree( dxcs );
		checkCudaErrors( status );
	status = cudaFree( dycs );
		checkCudaErrors( status );
	status = cudaFree( drs );
		checkCudaErrors( status );
    status = cudaFree( dnumHits );
		checkCudaErrors( status );

	return 0;
}