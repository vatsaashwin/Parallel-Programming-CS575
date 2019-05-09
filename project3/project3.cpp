#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumDragons; //number of dragons in the current population
// Your basic time step will be one month. Interesting parameters that you need are:

const float GRAIN_GROWS_PER_MONTH =		8.0;
const float ONE_DRAGON_EATS_PER_MONTH =     1.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

float Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

// I like squaring things with another function:
float
SQR( float x )
{
        return x*x;
}

unsigned int seed = 0;  // a thread-private variable
float x = Ranf( &seed, -1.f, 1.f );


void Grain()
{
    float NowHeight = ::NowHeight;

    while(NowYear < 2025)
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        float tempFactor = exp(-SQR((NowTemp - MIDTEMP)/ 10.));
        float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP)/ 10.));

        NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        NowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if(NowHeight < 0)
            NowHeight = 0.;

        #pragma omp barrier

        ::NowHeight = NowHeight;

        #pragma omp barrier

        #pragma omp barrier
    }
}

void GrainDeer()
{
    float NowNumDeer = ::NowNumDeer;

    while(NowYear < 2025)
    {
        if(NowNumDeer > NowHeight)
            NowNumDeer -= 1;
        else if(NowNumDeer < NowHeight)
                NowNumDeer += 1;
        
        NowNumDeer -= (float)NowNumDragons * ONE_DRAGON_EATS_PER_MONTH;

        if(NowNumDeer < 0)
            NowNumDeer = 0;

        #pragma omp barrier

        ::NowNumDeer = NowNumDeer;

        #pragma omp barrier

        #pragma omp barrier
    }
}

void Dracarys()
{
 	while(NowYear < 2025)
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        if(NowNumDragons > NowNumDeer)
            NowNumDragons = 0;
        else if(NowNumDragons < NowNumDeer && NowNumDeer - NowNumDragons > 4)
                NowNumDragons += 1;

        #pragma omp barrier

        ::NowNumDragons = NowNumDragons;
		#pragma omp barrier
		#pragma omp barrier
    }
}


void Watcher()
{
    int numMonth = 0;
    while(NowYear < 2025)
    {
        #pragma omp barrier
        #pragma omp barrier
        
        NowMonth++;
        numMonth++;

        if(NowMonth % 12 == 0)  // Increment Year after 12 months
        {
            NowMonth = 0;
            NowYear++;
        }

        // Calculate Temperature and Precipitation
        float ang = (30.*(float)NowMonth + 15.) * (M_PI/ 180.);
        float temp = AVG_TEMP - AMP_TEMP * cos(ang);
        
        unsigned int seed = 0;
        NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
        NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
        
        if(NowPrecip < 0.)
            NowPrecip = 0.;

        printf("%d\t%.2f\t%.2f\t%.2f\t%d\t%d\n", numMonth, NowPrecip, NowTemp, NowHeight, NowNumDeer, NowNumDragons);
    
        // DonePrinting barrier:
        #pragma omp barrier
    }
}


int main( int argc, char *argv[])
{

	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif

	//Set the number of threads
	omp_set_num_threads(4);

	// starting date and time:
	NowMonth =    0;
	NowYear  = 2019;

	// starting state (feel free to change this if you want):
	NowNumDeer = 10;
	NowHeight =  10.;
	NowNumDragons = 1;

	//The temperature and precipation function of a particular month
	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	unsigned int seed = 0;
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );

	if( NowPrecip < 0. ){
		NowPrecip = 0.;
	}
	while( NowYear < 2025 )
	{
	
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				GrainDeer();
			}
	
			#pragma omp section
			{
				Grain();
			}

			#pragma omp section
			{
				Watcher();
			}
			#pragma omp section
			{
				Dracarys( );	// your own
			}
   	
		}
	}

	return 0;
}






