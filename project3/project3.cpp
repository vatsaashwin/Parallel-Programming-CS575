#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

// Requirements
// You are creating a month-by-month simulation of a grain-growing operation. The amount the grain grows is affected by the temperature, amount of precipitation, and the number of "graindeer" around to eat it. The number of graindeer depends on the amount of grain available to eat.
// The "state" of the system consists of the following global variables:

int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumDragons; //number of dragons in the current population
// Your basic time step will be one month. Interesting parameters that you need are:

const float GRAIN_GROWS_PER_MONTH =		8.0;
const int   DEER_GROWS_PER_MONTH =                20;
const int   DRAGON_GROWS_PER_MONTH =                1;


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
// Units of grain growth are inches. 
// Units of temperature are degrees Fahrenheit (°F). 
// Units of precipitation are inches.

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int
Ranf( unsigned int *seedp, int ilow, int ihigh )
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


// In addition to this, you must add in some other phenomenon that directly or indirectly controls the growth of the 
// grain and/or the graindeer population. Your choice of this is up to you.
// You are free to tweak the constants to make everything turn out "more interesting".
// The GrainGrowth and GrainDeer threads will each compute the next grain height and the next number of deer based 
// on the current set of global state variables. They will compute these into local, temporary, variables. 
// They both then will hit the DoneComputing barrier.

void Grain()
{
     compute into tmp variables
    // float tempFactor = exp(-pow((NowTemp - MIDTEMP) / 10., 2));
    // float precipFactor = exp(-pow((NowPrecip - MIDPRECIP) / 10., 2));
    float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
	float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

    float tempNowHeight  = NowHeight;
    // tempNowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    // tempNowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH + NowNumPest * PEST_GROWS_PER_MONTH;

    // You then use tempFactor and precipFactor like this:
	tempNowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
	tempNowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

    if(tempNowHeight <= 0.)
        tempNowHeight = 0.;

#pragma omp barrier
     // copy into global variables
    NowHeight = tempNowHeight;
#pragma omp barrier
#pragma omp barrier
}

void GrainDeer()
{
    float grainFactor = 1 - exp(   -SQR(  ( NowHeight - 0.0 ) / 10.  )   );
    //  compute into tmp variables
    int tempNowNumDeer = NowNumDeer;
    tempNowNumDeer += grainFactor * DEER_GROWS_PER_MONTH;

    if (tempNowNumDeer > ONE_DRAGON_EATS_PER_MONTH * NowNumDragons && tempNowNumDeer > NowHeight / ONE_DEER_EATS_PER_MONTH)
        tempNowNumDeer -= ONE_DRAGON_EATS_PER_MONTH * NowNumDragons;
    else
        tempNowNumDeer = tempNowNumDeer * 0.75;
#pragma omp barrier
    //  copy into global variables
    NowNumDeer = tempNowNumDeer;
#pragma omp barrier
#pragma omp barrier

}

void Dracarys()
{
    int tempNumDragons = NowNumDragons;
    float deerFactor = 1 - exp(   -SQR(  ( NowNumDeer - 30 ) / 10.  )   );
    
    if (deerFactor > 0.5 && NowNumDeer > 30)
        tempNumDragons += DRAGON_GROWS_PER_MONTH;
    if (tempNumDragons > NowNumDeer / ONE_DRAGON_EATS_PER_MONTH / 6.)
        tempNumDragons = tempNumDragons * 0.5;
    
#pragma omp barrier
    NowNumDragons = tempNumDragons;
#pragma omp barrier
#pragma omp barrier
}


void Watcher()
{

	#pragma omp barrier
    #pragma omp barrier

    printf("%d\t%.2f\t%.2f\t%.2f\t%d\t%d\n", NowMonth+1, NowTemp, NowPrecip, NowHeight, NowNumDeer, NowNumDragons);
    
    //  update month and year
    NowMonth++;
    NowYear = 2019 + NowMonth/12;

    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
	float temp = AVG_TEMP - AMP_TEMP * cos( ang );

	unsigned int seed = 0;
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		NowPrecip = 0.;

	#pragma omp barrier

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
	NowNumDeer = 1;
	NowNumDragons = 1;
	NowHeight =  1.;
	seed 	   = 0;

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






