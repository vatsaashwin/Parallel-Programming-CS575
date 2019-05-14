#include "simd.p4.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE		100000
#endif

#ifndef NUMTRIES
#define NUMTRIES	100
#endif

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];


// July 13, 2017: mjb -- changed rbx register to r8
// The convention is that rbx needs to be saved by the callee (i.e., pushed and popped),
// 	but rcx, rdx, r8, and r9 do not
// This fixed the bug that showed up in cs 475/575 project #5 in SQ 2017

float Ranf( float low, float high )
{
    float r = (float) rand();         // 0 - RAND_MAX
    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

void SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	__asm
	(
		".att_syntax\n\t"
		"movq    -24(%rbp), %r8\n\t"		// a
		"movq    -32(%rbp), %rcx\n\t"		// b
		"movq    -40(%rbp), %rdx\n\t"		// c
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"movups	%xmm0, (%rdx)\n\t"	// store the result
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
			"addq $16, %rdx\n\t"
		);
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}



float SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	__asm
	(
		".att_syntax\n\t"
		"movq    -40(%rbp), %r8\n\t"		// a
		"movq    -48(%rbp), %rcx\n\t"		// b
		"leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
		"movups	 (%rdx), %xmm2\n\t"		// 4 copies of 0. in xmm2
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"addps	%xmm0, %xmm2\n\t"	// do the add
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
		);
	}

	__asm
	(
		".att_syntax\n\t"
		"movups	 %xmm2, (%rdx)\n\t"	// copy the sums back to sum[ ]
	);

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}

void NonSimdMul( float *a, float *b,   float *c,   int len )
{
	for( int i = 0; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}

float NonSimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	for( int i = 0; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0];
}

// array size from 1K to 'big'
// Test the number of trials
// Use the peak value for the performance you record.
// you don't need to worry about a 

int main( int argc, char *argv[ ] )
{
	#ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
        return 1;
	#endif

    for( int i =0; i < ARRAYSIZE; i++ )
    {
        A[i] = Ranf(-100.0f, 100.0f);
        B[i] = Ranf(-100.0f, 100.0f);
        C[i] = Ranf(-100.0f, 100.0f);
    }

    double maxPerformanceSimdMul = 0.;
    double maxPerformanceNonSimdMul = 0.;
    double maxPerformanceSimdMulSum = 0.;
    double maxPerformanceNonSimdMulSum= 0.;

	for( int t = 0; t < NUMTRIES; t++ )
    {

		//  SimdMul
		double time0 = omp_get_wtime( );
		SimdMul(A, B, C, ARRAYSIZE);
		double time1 = omp_get_wtime( );
		double p = (double)ARRAYSIZE/(time1-time0)/1000000.;
		if( p > maxPerformanceSimdMul )
			maxPerformanceSimdMul = p;


		//  SimdMulSum
		time0 = omp_get_wtime( );
		float s = SimdMulSum(A, B, ARRAYSIZE);
		time1 = omp_get_wtime( );
		p = (double)ARRAYSIZE/(time1-time0)/1000000.;
		if( p > maxPerformanceSimdMulSum )
		{   
			maxPerformanceSimdMulSum = p;
			printf("sum - simd = %.4f\n", s);
		}        
            

		//  NonSimdMul
		time0 = omp_get_wtime( );
		NonSimdMul(A, B, C, ARRAYSIZE);
		time1 = omp_get_wtime( );
		p = (double)ARRAYSIZE/(time1-time0)/1000000.;
		if( p > maxPerformanceNonSimdMul )
			maxPerformanceNonSimdMul = p;
            

		//  NonSimdMulSum
		time0 = omp_get_wtime( );
		s = NonSimdMulSum(A, B, ARRAYSIZE);
		time1 = omp_get_wtime( );
		p = (double)ARRAYSIZE/(time1-time0)/1000000.;
		if( p > maxPerformanceNonSimdMulSum )
		{
			maxPerformanceNonSimdMulSum = p;
			printf("sum - simd = %.4f\n", s);
		}
 
    }

    printf( "  Max Performance for SIMD Multiplication ( Peak Performance ) = %10.2lf MegaMults/Sec\n", maxPerformanceSimdMul );
    printf( "  Max Performance for Non-SIMD Multiplication ( Peak Performance ) = %10.2lf MegaMults/Sec\n", maxPerformanceNonSimdMul );
    printf( "  Max Performance for SIMD Multiplication-Sum ( Peak Performance ) = %10.2lf MegaMults/Sec\n", maxPerformanceSimdMulSum );
    printf( "  Max Performance for Non-SIMD Multiplication-Sum ( Peak Performance ) = %10.2lf MegaMults/Sec\n", maxPerformanceNonSimdMulSum );
    printf( "  Speedup for SimdMul = %10.2f \n", maxPerformanceSimdMul / maxPerformanceNonSimdMul );
    printf( "  Speedup for SimdMulSum = %10.2f\n", maxPerformanceSimdMulSum / maxPerformanceNonSimdMulSum );
        
    return 0;


}









