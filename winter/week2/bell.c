#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

void mixSound(double *out, int start, int samples, double *in)
{
	out = out + start ;
	
	while(samples--)
	{
		*out = *out + *in ;
		out++ ;
		in++ ;
	}
	return ;
}
 
double *fm(double c, double m, double pd, double dur,  int SR)
{
     double cPhase ;
     double mPhase ;
     double cPhaseInc ;
     double mPhaseInc ;
     double A
     double *array ;
	 double *p ;
     double twopi ;
     int samples ;
     int i ;

     twopi = 2*M_PI ;
     cPhaseInc = (twopi * c) / SR ;
     mPhaseInc = (twopi * m) / SR ;
     samples = dur * SR ;
     A = pow(10.0, dB/20) / 2 ;

     array = (double*)calloc(samples, sizeof(double)) ;
	 cPhase = mPhase = 0 ;
	
	 p = array ;

     for(i=0; i<samples; i++)
     {
     *p++ = A = sin(cPhase + pd * sin(mPhase)) ;

     cPhase += cPhaseInc ;
     mPhase += mPhaseInc ;
     if(cPhase >= twopi) cPhase -= twopi ;
     if(mPhase >= twopi) mPhase -= twopi ;
     }

	 return array ;
}

double *Bell(double cBell)
{
		
	double dB = 144 ;
	double pd = 1 ;
	double dur = 15 ;
	double N = 6 ;
	double T[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0} ;
	double A[] = {1.0, 0.3, 0.2, 0.1, 0.0, 0.0} ;
    double *out ;
	double *tone ;

	double mBell;
	mBell = cBell * 1.4 ;
      
	int samples;
	samples = dur * SAMPLING_RATE;

	out = (double *)calloc(samples, sizeof(double));
    out = fm(cBell, mBell, pd, dur, SAMPLING_RATE);
	envelope(out, dur, T, A, N);

	printf("bell end %f\n", N);
	return out;
}

int main(void)
{

     double *sound ;
	 double *tone ;
	 double cBell = 220;
	 double dur = 15 ;

	 printf("main %f\n", dur);
	 sound = Bell(tone) ;
	 sound = Bell(cBell) ;

     saveSoundfile("bell.wav", sound, dur) ;

     return 0 ;
}


