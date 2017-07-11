#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <time.h>
#include <cm.h>

double *equalPowerPan(double *mono, int frames) 
{
	double *stereo, *S, *M ;
	double Left, Right, boost, panIncrement ;
	int nchan = 2 ;

	// output array
	stereo = (double *)calloc(frames * nchan, sizeof(double)) ;

	Left = 1 ;
	panIncrement = 1.0 / ( frames - 1 ) ;

	S = stereo ;
	M = mono ;

	while ( frames-- ) {
		Right = 1.0 - Left ;
		boost = 1.0 / sqrt(Left * Left + Right * Right) ;
		*S++ = (Left * boost) * *M ;
		*S++ = (Right * boost) * *M++ ;
		Left -= panIncrement ;
	}

	return stereo ;
}

void dotFM(double *fm, int samples, int i, int nDots)
{
	double carrier, modulator, cInc, mInc, cPhase, mPhase, amp, *pfm ;
	int I, ratio ;
	static double twopi, K, cShift ;
	static double db, dbShift, dur ;
	double T[] = {0.0, 0.01, 0.25, 0.9, 1.0} ;
	double A[] = {0.0, 1.0, 0.75, 0.5, 0.0} ;
	int N = 5 ;

	if ( i == 0 ) {					// first time only
		twopi = 2 * M_PI ;

		K = 1200 / log(2.0) ;		// to translate cents to hertz

		dur = samples / SAMPLING_RATE ;	// duration of each dot in seconds

		cShift = 100 / nDots ;		// shift by 100 cents over nDots
		dbShift = 12.0 / nDots ;	// shift by 12 dB over nDots

		db = 144 - 12 ;				// initial amplitude

	}

	carrier = 440 / exp((i*cShift)/K) ;	// freq descends by 'cShift'
	amp = pow(10.0, db/20.0) / 2 ;
	db += dbShift ;						// amp ascends by 'dbShift'


	ratio = (4 - 1) * frand() + 1 ;		// ratio of carrier to modulator
	modulator = carrier * ratio ;
	I = (4 - 1) * frand() + 1 ;			// value of 'I'

	cInc = ( twopi * carrier ) / SAMPLING_RATE ;	// hertz to radians
	mInc = ( twopi * modulator ) / SAMPLING_RATE ;
	cPhase = mPhase = 0 ;

	memset(fm, 0, samples * sizeof(double)) ;	// set memory to zero

	for ( pfm = fm ; pfm < fm + samples ; pfm++ ) {		// create sound
		*pfm = amp * sin(cPhase + I * sin(mPhase)) ;
		cPhase += cInc ;
		mPhase += mInc ;
		if ( cPhase >= twopi ) cPhase -= twopi ;
		if ( mPhase >= twopi ) mPhase -= twopi ;
	}

	envelope(fm, dur, T, A, N) ;		// apply envelope

	return ;
}

double *makeDotsFM(int *totalSamples)	// build a sequence of 'dots'
{
	double *oneDot, *allDots, *pa, *pd ;
	int oneDotSamples, silenceSamples, i ;
	int nDots = 30 ;
	double dur = 0.25 ;

	oneDotSamples = dur * SAMPLING_RATE ;

	oneDot = (double *)calloc(oneDotSamples, sizeof(double)) ;
	silenceSamples = 2 * oneDotSamples ;
	*totalSamples = nDots * (oneDotSamples + silenceSamples) ;

	allDots = (double *)calloc(*totalSamples, sizeof(double)) ;

	pa = allDots ;
	for ( i = 0 ; i < nDots ; i++ ) {
		dotFM(oneDot, oneDotSamples, i, nDots) ;
		for ( pd = oneDot ; pd < oneDot + oneDotSamples ; pd++ )
			*pa++ = *pd ;
		pa += silenceSamples ;
	}

	free(oneDot) ;

	return allDots ;
}

int main(void)
{
	double duration ;
	double *mono, *stereo ;
	int samples ;

	setRandSeed( time(0) ) ;	// initialize random number generator

	mono = makeDotsFM(&samples) ;	// build a single channel sequence

	duration = samples / SAMPLING_RATE ;	// duration in seconds

	setChannelCount(2) ;		// make sure output is set to two channels

	stereo = equalPowerPan(mono, samples) ;	// create the pan

	free(mono) ;

	saveSoundfile("pan.wav", stereo, duration) ;

	free(stereo) ;

	return 0 ;
}

