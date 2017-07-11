#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

void pan8(int frames,		// duration of *sound
		int startFrame,		// start time
		double **array2D,	// output array
		int startchan,	// initial & final channels for pan
		int endchan, 
		double *sound)	// input sound
{
	double *p1, *p2 ;
	double Ainc = 1.0 / (frames - 1.0) ;// amplitude increment
	double A1 = 1.0 ;	// initial & final amplitudes
	double A2 = 0.0 ;
	double boost ;

	p1 = array2D[startchan%8] ;		// point to correct channels
	p2 = array2D[endchan%8] ;

	p1 += startFrame ;		// increment to correct start time
	p2 += startFrame ; 

	while (frames--) {
		boost = 1.0 / sqrt((A1 * A1) + (A2 * A2)) ;

		*p1++ += (A1 * boost) * *sound ;
		*p2++ += (A2 * boost) * *sound++ ;

		A1 -= Ainc ;
		A2 += Ainc ;
	}
	
	return ;
}

double *fm(double C, double M, double pd, double dur, double dB, int SR)
{
	double cInc, mInc, A, cPhase, mPhase ;
	int i, samples ;
	double *array ;

	double twopi = 2 * M_PI ;

	cInc = (twopi * C ) / SR ;
	mInc = (twopi * M ) / SR ;

	samples = dur * SR ;

	array = (double *)calloc(samples, sizeof(double)) ;

	A = pow(10.0, dB/20)/2 ;

	cPhase = mPhase = 0 ;

	for ( i = 0 ; i < samples ; i++ ){

	array[i] = A * sin(cPhase + pd * sin(mPhase)) ;


	cPhase += cInc ;
	mPhase += mInc ;

	if (cPhase >= twopi) cPhase -= twopi ;
	if (mPhase >= twopi) mPhase -= twopi ;
	}

	return array ;
}

double **makeCycles(int *pathArray, int nSteps, double nCycles,
		 double *monoInput, int totalFrames) 
{
	int nFrames, nTrans, chA, chB, i, start ;
	double **array2D ;
	double totalDur ;

	nTrans = nSteps * nCycles ;	// total number of transitions
	nFrames = totalFrames / nTrans ;	// duraiton of each transition
	totalDur = totalFrames / SAMPLING_RATE ;	// total duration

	array2D = channel8Allocate(totalDur) ;

	// loop to generate each transition
	for ( i = 0, start = 0 ; i < nTrans ; i++ ) {

		chA = pathArray[i%nSteps] ;		// initial & final channels
		chB = pathArray[(i+1)%nSteps] ;

		// pan between those two channels
		pan8(nFrames, start, array2D, chA, chB, monoInput) ;

		start += nFrames ;	// increment start index

	}

	return array2D ;
}

int main(void)
{
	double C, M, pd, dur, dB, *tone ;
	double **array2D, *array1D ;
	int SR ;
	int totalFrames ;
	int pathArray[] = {0, 1, 2, 3, 4, 5, 6, 7} ;
	int nSteps = 8 ;
	double nCycles = 1 ; 

	C = 200 ;		// variables for FM tone
	M = 280 ;
	pd = 5 ;
	dur = 60 ;
	dB = 138 ;

	SR = SAMPLING_RATE ;

	totalFrames = dur * SAMPLING_RATE ;
	
	tone = fm(C, M, pd, dur, dB, SR) ;

	array2D = 
		makeCycles(pathArray, nSteps, nCycles, tone, totalFrames) ; 

	free(tone) ;

	array1D = channel8Interleave(array2D, dur) ;

	channel8Free(array2D) ;

	saveSoundfile("cycletest.wav", array1D, dur) ;

	free(array1D) ;

	return 0 ;
}


