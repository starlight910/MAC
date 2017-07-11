#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>
#include <time.h>

void mixSound(double *out, double *in, int start, int samples)
{
	out += start ;
	while(samples--)
	{
		*out = *out + *in ;
		out++ ;
		in++ ;
	}
	return ;
}



double *fmIShift(double C, double M, double dur, double dB)
{
	double cInc, mInc, cphase, mphase, A ;
	double *out, *po ;
	int samples ;
	double I, I2, iShift ;
	double twopi = 2 * M_PI ;

	cInc = ( twopi * C ) / SAMPLING_RATE ;
	mInc = ( twopi * M ) / SAMPLING_RATE ;
	cphase = mphase = 0 ;

	A = pow(10.0, dB/20.0) / 2 ;

	samples = dur * SAMPLING_RATE ;

	po = out = (double *)calloc(samples, sizeof(double)) ;

	I = 2 ;				// initial value of I
	I2 = 7 ;			// final value of I
	iShift = (I2 - I) / ( samples - 1 ) ;	// amount of increment

	while ( samples-- ) {

		*po++ = A * sin(cphase + I * sin(mphase)) ;

		I += iShift ;	// change the value of I

		cphase += cInc ;
		mphase += mInc ;

		if ( cphase >= twopi )
			cphase -= twopi ;

		if ( mphase >= twopi )
			mphase -= twopi ;
	}

	return out ;
}

/*
 * stereoMix: add a stereo input to a stereo output at the proper
 * location
 */

void stereoMix(double *stereoOut, int startFrame, int nFrames, double *stereoIn)
{
	int nchan = 2 ;

	stereoOut += startFrame * nchan ;	// set output to correct time

	while ( nFrames-- ) {				// add input to output
		*stereoOut++ += *stereoIn++ ;
		*stereoOut++ += *stereoIn++ ;
	}

	return ;
}

/*
 * monoToStereoPan: put a mono sound into a stereo location
 */

double *monoToStereoPan(double *monoIn, int nFrames, double pan)
{
	double L, R, boost ;
	double *stereoOut, *S ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan > 1.0 || pan < 0.0 ) {	// make sure values are within range
		printf("\tmonoToStereoPan[%03d]: bad value for pan: %f\n", 
				counter, pan);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}

	L = pan ;
	R = L - 1 ;
	boost = 1.0 / sqrt( L*L + R*R ) ;
	L *= boost ;
	R *= boost ;

	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	while ( nFrames-- ) {
		*S++ = L * *monoIn ;
		*S++ = R * *monoIn++ ;
	}

	return stereoOut ;
}

/*
 * monoToStereoPanShift: put a mono sound in a stereo output and shift
 * it from location pan1 to location pan2
 */

double *monoToStereoPanShift(double *monoIn, int nFrames, double pan1, 
		double pan2)
{
	double L, R, boost, pan, panInc ;
	double *S, *stereoOut ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan1 > 1.0 || pan1 < 0.0 || pan2 > 1.0 || pan2 < 0.0 ) {
		printf("\tmonoToStereoPanShift[%03d]: bad value for pan: %f %f\n", 
				counter, pan1, pan2);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}

	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	panInc = (pan2 - pan1) / ( nFrames - 1 ) ;
	pan = pan1 ;

	while ( nFrames-- ) {

		L = pan ;
		R = L - 1 ;
		boost = 1.0 / sqrt( L*L + R*R ) ;
		L *= boost ;
		R *= boost ;

		*S++ = L * *monoIn ;
		*S++ = R * *monoIn++ ;

		pan += panInc ;
	}

	return stereoOut ;
}

/*
 * makeFList: create a list of frequencies following the list of
 * given intervals
 */

double *makeFList(int nTones, double F)
{
	// list of intervals in cents
	double clist[] = {75, -125, 50, 175, -33, -66, -100, 150, -250, -350, 200} ;
	double K = 1200 / log10(2.0) ;
	double *flist ;
	int i ;

	flist = (double *)calloc(nTones, sizeof(double)) ;

	flist[0] = F ;		// first frequency

	for ( i = 1 ; i < nTones ; i++ )	// subsequent frequencies
		flist[i] = flist[i-1] * pow(10.0, clist[i%11]/K) ;

	return flist ;
}

/*
 * makePanList: create a list of random pan locations
 */
double *makePanList(int nTones)
{
	double panList[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double *p, *plist ;
	int index ;

	plist = (double *)calloc(nTones, sizeof(double)) ;

	for ( p = plist ; p < plist + nTones ; p++ ) 
	{
		index = irand() % 11 ;
		*p = panList[ index ] ;
	}

	return plist ;
}

/*double *makePanList2(int nTones)
{
	double panList[] = {0.0, 0.5, 0.7, 1.0} ;
	double *p, *plist ;
	int index ;

	plist = (double *)calloc(nTones, sizeof(double)) ;

	for ( p = plist ; p < plist + nTones ; p++ )
	{
		index = irand() % 11 ;
		*p = panList[ index] ;
	}

	return plist ;
}
*/

/*
 * makeEnvelope: envelope to apply to the tones
 */

void makeEnvelope(double *data, double dur)
{
	
	double T[] = {0.0, 0.1, 0.25, 0.75, 1.0} ;
	double A[] = {0.0, 1.0, 0.75, 0.5, 0.0} ;
	int N = 5 ;

	envelope(data, dur, T, A, N) ;

	return ;
}

double *secA(double *totalDur)
{
	// list of durations
	double durList[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5} ;

	int nDurs = 40 ;

	// list of ratios
	double ratios[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
					   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					   1, 1, 1, 1, 1, 1, 1, 1, 1, 1} ;
	int nRatios = 40 ;

	double F = 55 ;		// fundamental frequency
	double dB = 144 ;
	int nNotes = 40 ;		// total number of notes

	double *flist, *panList ;

	int i, nchan = 2 ;
	int startFrame, nFrames ;
	double C, M, dur, pan ;
	double *stereoOut, *mono, *stereo ;

	printf("\tCreating part A") ;

	flist = makeFList(nNotes, F) ;	// create frequency list
	panList = makePanList(nNotes) ;	// create pan list

	for ( i = 0 ; i < nDurs ; i++ )	// reduce all durations to 1/4
		durList[i] *= 0.25 ;

	*totalDur = 0 ;					// calculate total duration
	for ( i = 0 ; i < nNotes ; i++ )
		*totalDur += durList[i%nDurs] ;

	stereoOut = (double *)calloc(*totalDur*SAMPLING_RATE*nchan, sizeof(double));

	startFrame = 0 ;

	for ( i = 0 ; i < nNotes ; i++ ) {		// make the sequence of notes
		printf(".") ;

		C = flist[i] ;						// carrier
		M = C * ratios[i%nRatios] ;			// modulator = carrier * ratio
		dur = durList[i%nDurs] ;			// duration
		pan = panList[i] ;					// pan location

		mono = fmIShift(C, M, dur, dB) ;	// create FM tone
		makeEnvelope(mono, dur) ;			// apply envelope

		nFrames = dur * SAMPLING_RATE ;

		if ( i == 0 || pan == panList[i-1] )	// pan the tone
			stereo = monoToStereoPan(mono, nFrames, pan) ;
		else
			stereo = monoToStereoPanShift(mono, nFrames, panList[i-1], pan) ;

		// add to output 
		stereoMix(stereoOut, startFrame, nFrames, stereo) ;

		startFrame += nFrames ;	// increment start time

		free(mono) ;
		free(stereo) ;
	}

	free(flist) ;
	free(panList) ;

	printf("Done\n") ;

	return stereoOut ;
}

double *secB(double *totalDur)
{
	double durList[] = {0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 
						0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3,
						0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3} ;	// durations
	int nDurs = 30 ;
	double durList2[30] ;

	// ratios
	double ratios[] = {1.00, 1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 
		     		   1.10, 1.11, 1.12, 1.13, 1.14, 1.15, 1.16, 1.17, 1.18, 1.19,
					   1.20, 1.21, 1.22, 1.23, 1.24, 1.25, 1.26, 1.27, 1.28, 1.29} ;
	int nRatios = 30 ;

	double F = 164.81 ;		// fundamental
	double dB = 144 ;
	int nNotes = 30 ;		// total number of tones

	double *flist, *panList ;
	double *flist2, *panList2 ;

	int i, j, nchan = 2 ;
	int startFrame, nFrames ;
	double C, M, dur, pan ;
	double *stereoOut, *mono, *stereo ;

	printf("\tCreating part B") ;

	for ( i = 0, j = nDurs - 1  ; i < nDurs ; i++, j-- ) {
		durList[i] *= 0.5 ;				// reduce durations by 1/2
		durList2[j] = durList[i] ;		// reverse copy durList
	}

	flist = makeFList(nNotes, F) ;		// create frequencies
	panList = makePanList(nNotes) ;		// create pan locations

	panList2 = (double *)calloc(nNotes, sizeof(double)) ;
	flist2 = (double *)calloc(nNotes, sizeof(double)) ;

	for ( i = 0, j = nNotes - 1 ; i < nNotes ; i++, j-- ) {
		panList2[i] = panList[j] ;		// reverse copy panlist
		flist2[i] = flist[j] ;			// reverse copy frequency list
	}

	*totalDur = 0 ;						// calculate total duration
	for ( i = 0 ; i < nNotes ; i++ )
		*totalDur += durList[i%nDurs] ;

	stereoOut = (double *)calloc(*totalDur*SAMPLING_RATE*nchan, sizeof(double));

	startFrame = 0 ;

	for ( i = 0 ; i < nNotes ; i++ ) {	// create the sequence

		printf(".") ;

		C = flist[i] ;					// carrier
		M = C * ratios[i%nRatios] ;		// modulator = carrier * ratio
		dur = durList[i%nDurs] ;		// duration
		pan = panList[i] ;				// pan location

		mono = fmIShift(C, M, dur, dB) ;	// create FM tone
		makeEnvelope(mono, dur) ;			// apply envelope

		nFrames = dur * SAMPLING_RATE ;

		if ( i == 0 || pan == panList[i-1] )	// pan the tone
			stereo = monoToStereoPan(mono, nFrames, pan) ;
		else
			stereo = monoToStereoPanShift(mono, nFrames, panList[i-1], pan) ;

		// add to output
		stereoMix(stereoOut, startFrame, nFrames, stereo) ;

		free(mono) ;
		free(stereo) ;

		// create the second tone

		M = flist[i] ;					// reverse the carrier and modulator
		C = M * ratios[i%nRatios] ;
		dur = durList2[i%nDurs] ;
		pan = panList2[i] ;

		mono = fmIShift(C, M, dur, dB) ;	// create the tone
		makeEnvelope(mono, dur) ;
		nFrames = dur * SAMPLING_RATE ;

		if ( i == 0 || pan == panList2[i-1] )		// pan the tone
			stereo = monoToStereoPan(mono, nFrames, pan) ;
		else
			stereo = monoToStereoPanShift(mono, nFrames, panList[i-1], pan) ;

		// add to output mix
		stereoMix(stereoOut, startFrame, nFrames, stereo) ;

		free(mono) ;
		free(stereo) ;


		startFrame += nFrames ;

	}

	free(flist) ;
	free(panList) ;

	printf("Done\n") ;

	return stereoOut ;
}

double *secC(double *totalDur)
{
	// durations
	double durList[] = {0.75, 3.25, 1.0, 0.625, 0.375, 0.25,
							0.25, 0.375, 0.625, 1.0, 3.25, 0.75} ;
	int nDurs = 12 ;

	// ratios
	double ratios[] = {1./2., 3./8, 1./2, 1./3, 4./9, 1./3, 1./4, 3./8, 1./4,
		1./5, 2./5, 1./5, 1./6, 1./7, 2./7, 1./7, 1./8, 3./8, 1./8, 1./9, 
		2./9, 1./9} ;
	int nRatios = 22 ;

	double F = 660 ;		// fundamental
	double dB = 144 ;
	int nNotes = 20 ;		// number of tones
	double *flist, *panList ;

	int i, nchan = 2 ;
	int startFrame, nFrames ;
	double C, M, dur, pan ;
	double *stereoOut, *mono, *stereo ;

	printf("\tCreating part C") ;

	for ( i = 0 ; i < nDurs ; i++ )		// reduce durations by 1/2
		durList[i] *= 0.5 ;

	flist = makeFList(nNotes, F) ;		// create frequency list
	panList = makePanList(nNotes) ;		// create pan list

	*totalDur = 0 ;						// calculate total duration
	for ( i = 0 ; i < nNotes ; i++ )
		*totalDur += durList[i%nDurs] ;

	stereoOut = (double *)calloc(*totalDur*SAMPLING_RATE*nchan, sizeof(double));

	startFrame = 0 ;

	for ( i = 0 ; i < nNotes ; i++ ) {	// create the sequence

		printf(".") ;

		C = flist[i] ;					// carrier
		M = C * ratios[i%nRatios] ;		// modulator = carrier * ratio
		dur = durList[i%nDurs] ;		// duration
		pan = panList[i] ;				// pan location

		mono = fmIShift(C, M, dur, dB) ;	// create tone
		makeEnvelope(mono, dur) ;			// apply envelope

		nFrames = dur * SAMPLING_RATE ;

		if ( i == 0 || pan == panList[i-1] )	// pan the tone
			stereo = monoToStereoPan(mono, nFrames, pan) ;
		else
			stereo = monoToStereoPanShift(mono, nFrames, panList[i-1], pan) ;

		// add to output
		stereoMix(stereoOut, startFrame, nFrames, stereo) ;

		startFrame += nFrames ;

		free(mono) ;
		free(stereo) ;
	}

	free(flist) ;
	free(panList) ;

	printf("Done\n") ;

	return stereoOut ;
}

int main(void)
{
	double *partA, *partB, *partC, *final ;
	double durA, durB, durC, totalDur ;
	int nFramesA, nFramesB, nFramesC, startFrame ;
	int nchan = 2 ;

	setChannelCount(2) ;

	partA = secA(&durA) ;	// create the three sequences
	partB = secB(&durB) ;
	partC = secC(&durC) ;

	// set the total duration
	totalDur = durA + durA + durB + durA + durA + durC + durA ;

	final = (double *)calloc(totalDur * SAMPLING_RATE * nchan, sizeof(double));

	nFramesA = durA * SAMPLING_RATE ;	// sequence durations in samples
	nFramesB = durB * SAMPLING_RATE ;
	nFramesC = durC * SAMPLING_RATE ;

	startFrame = 0 ;

	// 1st part
	stereoMix(final, startFrame, nFramesA, partA) ;
	startFrame += nFramesA ;

	stereoMix(final, startFrame, nFramesA, partA) ;
	startFrame += nFramesA ;

    stereoMix(final, startFrame, nFramesA, partA) ;
    startFrame += nFramesA ;

    stereoMix(final, startFrame, nFramesA, partA) ;
    startFrame += nFramesA ;
	printf("\tCopting 1st part\n") ;


	//2nd part
	printf("\tCopying 2nd part\n") ;
    stereoMix(final, startFrame, nFramesB, partB) ; 
    startFrame += nFramesB ;



	//printf("\tCopying 3rd part\n") ;
	stereoMix(final, startFrame, nFramesA, partA) ;		// add 1st part
	startFrame += nFramesA ;

	printf("\tCopying 3rd part\n") ;
	stereoMix(final, startFrame, nFramesA, partA) ;     // add 1st part
	startFrame += nFramesA ;

	printf("\tCopying 4th part\n") ;
	stereoMix(final, startFrame, nFramesC, partC) ;		// add 3rd part
	startFrame += nFramesC ;

	printf("\tCopying 5th part\n") ;
	stereoMix(final, startFrame, nFramesA, partA) ;		// add 1st part again

	

	saveSoundfile("project1.wav", final, totalDur) ;

	free(partA) ;
	free(partB) ;
	free(partC) ;
	free(final) ;

	return 0 ;
}

