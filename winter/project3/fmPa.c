#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cm.h>

double *fmParallel(double carrier, double mod1, double I1, double mod2, double I2, double dur, double dB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.1, 0.5, 0.8, 1.0} ;
	double P[] = {0.0, 1.0, 0.75, 0.50, 0.0} ;
	int N = 5;

	double T1[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double P1[] = {1.0, 0.8, 0.6, 0.4, 1.0, 0.6, 0.4, 1.0, 0.4, 0.2, 0.0} ;
	int N1 = 11;

	double T2[] = {0.0, 0.1, 0.5, 1.0} ;
	double P2[] = {0.0, 1.0, 0.6, 0.0} ;
	int N2 = 4;
	double *env1, *env2, *e1, *e2 ;

	cInc = (twopi * carrier) / SAMPLING_RATE ;
	m1Inc = (twopi * mod1) / SAMPLING_RATE ;
	m2Inc = (twopi * mod2) / SAMPLING_RATE ;

	C = M1 = M2 = 0 ;

	amp = pow(10.0, dB/20.0)/2 ;

	samples = dur * SAMPLING_RATE ;

	sound = (double *)calloc(samples, sizeof(double)) ;

	env1 = (double *)calloc(samples, sizeof(double)) ;
	env2 = (double *)calloc(samples, sizeof(double)) ;

	linearEnv(env1, samples, T1, P1, N1) ;
	linearEnv(env2, samples, T2, P2, N2) ;

	rescale(env1, samples, 0.0, I1) ;
	rescale(env2, samples, 0.0, I2) ;

	e1 = env1 ;
	e2 = env2 ;

	for ( s = sound ; s < sound + samples ; s++ ) 
			{
				*s = amp * sin(C + *e1++ * sin(M1) + *e2++ * sin(M2)) ;
				C += cInc ;
				M1 += m1Inc ;
				M2 += m2Inc ;

				if( C >= twopi ) C -= twopi ;
				if( M1 >= twopi ) M1 -= twopi ;
				if( M2 >= twopi ) M2 -= twopi ;
			}
		
	free(env1) ;
	free(env2) ;
	envelope(sound, dur, T, P, N) ;

	return sound ;
}
