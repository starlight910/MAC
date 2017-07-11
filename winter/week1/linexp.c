#include <stdio.h>     // printf()
#include <stdlib.h>    // calloc() , free()
#include <time.h>      // time()
#include <math.h>      // log
#include <cm.h>        // 



int main(void)
{
	double *lin ;
	double *expo ;
	double logR ;

	double f1 = 220 ;     // Freq 1 Start
	double f2 = 880 ;     // Freq 2 End
	double dB = 144 ;     // desiBell
	double dur = 10 ;     // Time
	double *tone ;        // TriangleGLiss tone
	int nPartials = 100;  // Number ofPartials
	double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
//	double A[] = {1,1,1,1,1,1,1,1,1,1,1} ;
	int N = 11 ;          // Number of steps

	logR = 1 ;
	lin = linear(f1, f2, N) ;
	expo = exponential(f1, f2, N, logR) ;

	printf("Linear value\n") ;
	show(lin, N) ;
	printf("Exponential value\n") ;
	show(expo, N) ;

	tone = TriangleGliss(f1, f2, dB, dur, nPartials) ;
	envelope(tone, dur, T, lin, N) ;
	saveSoundfile("linear.wav", tone, dur);
	free (tone) ;

	tone = TriangleGliss(f1, f2, dB, dur, nPartials) ;
	envelope(tone, dur, T, expo, N) ;
	saveSoundfile("Expotnential.wav", tone, dur);
	free (tone) ;

	return 0;
	
}

void addGliss(double f1, double f2, double A, double *tone, int samples, double startingPhase)
{
	const double twopi = 2 * M_PI ;
	double logRatio, scl, time, timeInc, phase, phaseInc ;
	
	f1 = ( twopi * f1 ) / SAMPLING_RATE ;   // convert hertz to radians
	f2 = ( twopi * f2 ) / SAMPLING_RATE ;
	
	 logRatio = log(f2/f1) ;
     scl = ( f2 - f1 ) / (1-exp(logRatio)) ;
     timeInc = 1.0 / (samples - 1) ;

     phase = f1 + startingPhase ;
     for ( time = 0 ; samples-- ; time += timeInc ) 
	 {
		 if ( phase >= twopi )
		      phase -= twopi ;
         *tone++ += A * sin(phase) ;
		 phaseInc = f1 + scl * ( 1 - exp(time * logRatio) ) ;
		 phase = phase + phaseInc ;
     }
     return ;
}


double *TriangleGliss(double f1, double f2, double dB, double dur, int nPartials)
{
    double F1, F2, A, *tone, phase ;
    int i, samples, n, maxPartials ;
    const double nyquist = SAMPLING_RATE / 2 ;
    static int counter ;

    // check to make sure nPartials doesn't exceed nyquist
    maxPartials = ( (nyquist/f1) - 1 ) / 2 ;    // check nPartials for f1
    if ( nPartials > maxPartials )
    nPartials = maxPartials ;

    maxPartials = ( (nyquist/f2) - 1 ) / 2 ;    // check nPartials for f2
    if ( nPartials > maxPartials )
	nPartials = maxPartials ;
	
	fprintf(stderr, "\tTriGliss[%03d]: %4.0f->%4.0f Hz  %3.0f dB  %3d partials  %5.2f sec\n", counter++, f1, f2, dB, nPartials, dur) ;

    samples = dur * SAMPLING_RATE ;
    tone = (double *)calloc(samples, sizeof(double)) ;
    phase = M_PI / 2 ;  // set phase to pi/2 for a cosine wave

    for ( i = 0 ; i < nPartials ; i++ )
	{
			n = 2 * i + 1 ;     // odd partials only
            F1 = f1 * n ;       // start and end frequencies for this partial
	        F2 = f2 * n ;
	        A = 1.0 / (n*n) ;   // amplitude for this partial
			addGliss(F1, F2, A, tone, samples, phase) ;
	}
	scale(tone, samples, dB) ;
	
	return tone ;
}


//linear tone
double *linear(double f1, double f2, int nSteps)
{	
	double *list ;
	double *p ;
	double range ;   
	double linc ;  //linear increase

	list = (double *)calloc(nSteps, sizeof(double)) ;
	range = f2-f1 ;
	linc = range/nSteps ;

	for(p = list; p < list + nSteps; p++)
	{
		*p = f1 ;
		f1 += linc ;
	}

	return list ;
}


// Exponential
double *exponential(double f1, double f2, int nSteps, double logRatio)
{
	double *list ;
	double *p ;
	double scale ;
	double einc ;  //expo increase

	list = (double *)calloc(nSteps, sizeof(double)) ;
	scale = (f2 - f1) / (1 - exp(logRatio)) ;
	einc = 1/nSteps ;

	for(p = list; p < list + nSteps; p++)
	{
		*p = f1 + scale * (1 - exp(t * logRatio)) ;
	}

	return list ;
}

void show(double *list, int len)
{
	double *p ;

	for(p = list; p < list + len; p++)
	{
		printf("%f\n",*p ) ;
	}
	return ;
}
