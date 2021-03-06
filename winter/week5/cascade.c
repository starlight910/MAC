#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

double *cascadeFM(double carrier, 
		double mod1, double I1, 
		double mod2, double I2, 
		double dur, double dB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.1, 0.25, 0.75, 1.0} ;
	double A[] = {0.0, 1.0, 0.75, 0.50, 0.0} ;
	int N = 5 ;
	static int counter ;

	printf("\tcascadeFM[%03d]: C: %g  M1: %g  M2: %g  I1: %g  I2: %g\n",
			counter++, carrier, mod1, mod2, I1, I2) ;

	cInc = ( twopi * carrier ) / SAMPLING_RATE ;
	m1Inc = ( twopi * mod1 ) / SAMPLING_RATE ;
	m2Inc = ( twopi * mod2 ) / SAMPLING_RATE ;

	C = M1 = M2 = 0 ;

	amp = pow(10.0, dB/20.0) / 2 ;

	samples = dur * SAMPLING_RATE ;

	sound = (double *)calloc(samples, sizeof(double)) ;

	for ( s = sound  ; s < sound + samples ; s++ ) {
		
		*s = amp * sin(C + I1 * sin(M1 + I2 * sin(M2))) ;

		C += cInc ;
		M1 += m1Inc ;
		M2 += m2Inc ;

		if ( C >= twopi ) C -= twopi ;
		if ( M1 >= twopi ) M1 -= twopi ;
		if ( M2 >= twopi ) M2 -= twopi ;
	}

	// apply an envelope
	envelope(sound, dur, T, A, N) ;

	return sound ;
}

int main(void)
{
	double *sound ;
	double f = 440 ;
	double dur = 10 ;

	printf("main %f\n", dur) ;
	sound = cascadeFM(f) ;

	saveSoundfile("cascade.wav", sound, dur) ;

	return 0 ;
}
