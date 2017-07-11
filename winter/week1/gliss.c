#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cm.h>

void addGliss(double f1, double f2, double A, 
		double *tone, int samples, double startingPhase)
{
	const double twopi = 2 * M_PI ;
	double logRatio, scl, time, timeInc, phase, phaseInc ;
	/*
	 * This is the equation being used:
	 *
	 * out[n] = f1 + (f2-f1) * (1-exp(time*log(f2/f1)))/(1-exp(log(f2/f1)))
	 *
	 * where :
	 *	'time' represents 'percentage' between 0 and 1
	 *	f1, f2 are the initial and final frequencies in radians
	 *	out[n] is the output at sample number 'n'
	 *
	 * this equation can be simplified:
	 *
	 * 1. Replace log(f2/f1) with the variable 'logRatio'
	 *
	 *		logRatio = log(f2/f1)
	 *		out[n] = f1 + (f2-f1) * (1-exp(time*logRatio)) / (1-exp(logRatio))
	 *
	 * 2. This can be rearranged to :
	 *
	 *		out[n] = f1 + (f2-f1) / (1-exp(logRatio)) * (1-exp(time*logRatio)
	 *
	 *	because multiplication is associative.
	 *
	 *	3. Now the middle component 
	 *
	 *		(f2-f1) / (1-exp(logRatio))
	 *
	 *	can be a constant:
	 *
	 *		scale = (f2-f1) / (1-exp(logRatio)) 
	 *
	 *	and the equation can be written as:
	 *
	 *		out[n] = f1 + scale * (1-exp(time*logRatio))
	 *
	 */
	f1 = ( twopi * f1 ) / SAMPLING_RATE ;	// convert hertz to radians
	f2 = ( twopi * f2 ) / SAMPLING_RATE ;

	logRatio = log(f2/f1) ;
	scl = ( f2 - f1 ) / (1-exp(logRatio)) ;
	timeInc = 1.0 / (samples - 1) ;

	phase = f1 + startingPhase ;
	for ( time = 0 ; samples-- ; time += timeInc ) {
		if ( phase >= twopi ) 
			phase -= twopi ;
		*tone++ += A * sin(phase) ;
		phaseInc = f1 + scl * ( 1 - exp(time * logRatio) ) ;
		phase = phase + phaseInc ; 
	}
	return ;
}
double *SawtoothGliss(double f1, double f2, double dB, 
		double dur, int nPartials)
{
	double F1, F2, A, *tone, phase ;
	int samples, n ;
	const double nyquist = SAMPLING_RATE / 2 ;
	static int counter ;

	if ( nPartials * f1 >= nyquist )
		nPartials = (int)(nyquist/f1) ;

	if ( nPartials * f2 >= nyquist )
		nPartials = (int)(nyquist/f2) ;

	fprintf(stderr,
	"\tSawGliss[%03d]: %4.0f->%4.0f Hz  %3.0f dB  %3d partials  %5.2f sec\n",
			counter++, f1, f2, dB, nPartials, dur) ;

	samples = dur * SAMPLING_RATE ;
	tone = (double *)calloc(samples, sizeof(double)) ;
	phase = 0 ;		// set phase to zero for a sine wave

	for ( n = 1 ; n <= nPartials ; n++ ) {
		F1 = f1 * n ;
		F2 = f2 * n ;
		A = 1.0 / n ;
		addGliss(F1, F2, A, tone, samples, phase) ;
	}

	scale(tone, samples, dB) ;

	return tone ;
}

double *SquareGliss(double f1, double f2, double dB, 
		double dur, int nPartials)
{
	double F1, F2, A, *tone, phase ;
	int i, samples, n, maxPartials ;
	const double nyquist = SAMPLING_RATE / 2 ;
	static int counter ;

	// check to make sure nPartials doesn't exceed nyquist
	maxPartials = ( (nyquist/f1) - 1 ) / 2 ;	// check for f1
	if ( nPartials > maxPartials )
		nPartials = maxPartials ;

	maxPartials = ( (nyquist/f2) - 1 ) / 2 ;	// check for f2
	if ( nPartials > maxPartials )
		nPartials = maxPartials ;

	fprintf(stderr,
	"\tSquGliss[%03d]: %4.0f->%4.0f Hz  %3.0f dB  %3d partials  %5.2f sec\n",
			counter++, f1, f2, dB, nPartials, dur) ;

	samples = dur * SAMPLING_RATE ;
	tone = (double *)calloc(samples, sizeof(double)) ;
	phase = 0 ;		// set phase to zero for a sine wave

	for ( i = 0 ; i < nPartials ; i++ ) {
		n = 2 * i + 1 ;		// odd partials only
		F1 = f1 * n ;		// start and end frequences for this partial
		F2 = f2 * n ;
		A = 1.0 / n ;		// amplitude for this partial
		addGliss(F1, F2, A, tone, samples, phase) ;
	}

	scale(tone, samples, dB) ;

	return tone ;
}

double *TriangleGliss(double f1, double f2, double dB, 
		double dur, int nPartials)
{
	double F1, F2, A, *tone, phase ;
	int i, samples, n, maxPartials ;
	const double nyquist = SAMPLING_RATE / 2 ;
	static int counter ;

	// check to make sure nPartials doesn't exceed nyquist
	maxPartials = ( (nyquist/f1) - 1 ) / 2 ;	// check nPartials for f1
	if ( nPartials > maxPartials )
		nPartials = maxPartials ;

	maxPartials = ( (nyquist/f2) - 1 ) / 2 ;	// check nPartials for f2
	if ( nPartials > maxPartials )
		nPartials = maxPartials ;

	fprintf(stderr,
	"\tTriGliss[%03d]: %4.0f->%4.0f Hz  %3.0f dB  %3d partials  %5.2f sec\n",
			counter++, f1, f2, dB, nPartials, dur) ;

	samples = dur * SAMPLING_RATE ;
	tone = (double *)calloc(samples, sizeof(double)) ;
	phase = M_PI / 2 ;	// set phase to pi/2 for a cosine wave

	for ( i = 0 ; i < nPartials ; i++ ) {
		n = 2 * i + 1 ;		// odd partials only
		F1 = f1 * n ;		// start and end frequencies for this partial
		F2 = f2 * n ;
		A = 1.0 / (n*n) ;	// amplitude for this partial
		addGliss(F1, F2, A, tone, samples, phase) ;
	}

	scale(tone, samples, dB) ;

	return tone ;
}

double *SineGliss(double f1, double f2, double dB, double dur)
{
	int samples ;
	double *tone, phase, A ;
	static int counter ;
	fprintf(stderr,
		"\tSineGliss[%03d]: %4.0f->%4.0f Hz  %3.0f dB  %5.2f sec\n",
			counter++, f1, f2, dB, dur) ;
	samples = dur * SAMPLING_RATE ;
	tone = (double *)calloc(samples, sizeof(double)) ;

	A = 1.0 ;
	phase = 0 ;		// set phase to zero for a sine wave
	addGliss(f1, f2, A, tone, samples, phase) ;
	scale(tone, samples, dB);
	return tone ;
}
 
int main(void)
{
	double f2 = 440, f1 = 110;
	double dB = 144 ;
	double dur = 10 ;
	double *tone ;
	int nPartials = 1000 ;

	tone = SawtoothGliss(f1, f2, dB, dur, nPartials) ;

	playbuffer(tone, dur) ;
	saveSoundfile("gliss.wav", tone, dur) ;

	free(tone);

	return 0 ;
}
