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

double mixStereo(double *outStereo, int startFrame, int nFrames, double *inStereo)
{
	int nchan = 2 ;

	outStereo += startFrame * nchan ;

	while ( nFrames-- ) 
	{
		*outStereo++ += *inStereo++ ;
		*outStereo++ += *inStereo++ ;
	}
		
	return ;
}

double *monoToStereoPan(double *monoIn, int nFrames, double pan)
{
	double L, R, boost ;
	double *stereoOut, *S ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan > 1.0 || pan < 0.0 ) 
	{	// make sure values are within range
		printf("\tmonoToStereoPan[%03d]: bad value for pan: %f\n", counter, pan);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}

	L = pan ;
	R = L - 1 ;
	boost = 1.0 / sqrt( L*L + R*R ) ;
	L *= boost ;
	R *= boost ;

	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	while ( nFrames-- ) 
	{
		*S++ = L * *monoIn ;
		*S++ = R * *monoIn++ ;
	}

	return stereoOut ;
}

/*
 *  * monoToStereoPanShift: put a mono sound in a stereo
 *  output and shift
 *   * it from location pan1 to location pan2
 *    */

double *monoToStereoPanShift(double *monoIn, int nFrames, double pan1, double pan2)
{
	double L, R, boost, pan, panInc ;
	double *S, *stereoOut ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan1 > 1.0 || pan1 < 0.0 || pan2 > 1.0 || pan2 < 0.0 ) 
	{
		printf("\tmonoToStereoPanShift[%03d]: bad value for pan: %f %f\n", counter, pan1, pan2);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}

	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	panInc = (pan2 - pan1) / ( nFrames - 1 ) ;
	pan = pan1 ;

	while ( nFrames-- ) 
	{
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


//double *Pan1(double *mono, int frames)
//{
//	double *stereo, *S, *M ;
//	double Left, Right, pan

//double *Pan2(double *







double *secA(double *totalDur)
{
	double t = 0.5 ;
	double F = 55;
	double dB = 144 ;
	int nNotes = 150 ;             // 0.5 * 300  = 150sec  = 2min 30sec 
	
	double *flist, *panList ;

	int i, nchan = 2 ;
	int startFrame, nFrames ;
	double C, M ,dur, pan ;
	double *stereoOut, *mono, *stereo ;
	
	printf("\tCreating part A") ;

	flist = makeFlist(nNotes, F) ;
	panList = makePanList(nNotes) ;
	
	*totalDur = 0 ;
	for(i = 0 ; i < nNotes ; i++)
		*totalDur += t ;

	stereoOut = (double *)calloc(*totalDur*SAMPLING_RATE*nchan, sizeof(double));

	startFrame = 0 ;

	for ( i = 0 ; i < nNotes ; i++ ) 
	{		// make the sequence of notes
		printf(".") ;

		C = flist[i] ;						// carrier
		M = 10 ;			// modulator = carrier * ratio
		dur = t ;							// duration
		pan = panList[i] ;					// pan location

		mono = fmIShift(C, M, dur, dB) ;	// create FM tone
		makeEnvelope(mono, dur) ;			// apply envelope

		nFrames = dur * SAMPLING_RATE ;

		if ( i == 0 || pan == panList[i-1] )	// pan the tone
			stereo = monoToStereoPan(mono, nFrames, pan) ;
			else
			stereo = monoToStereoPanShift(mono, nFrames, panList[i-1], pan) ;

			// add to output 
			stereoMix(stereoOut, startFrame, nFrames, stereo);
		
			startFrame += nFrames ;  // increment start time
			
			free(mono) ;
			free(stereo) ;
			}
		
			free(flist) ;
			free(panList) ;

			printf("Done\n") ;

			return stereoOut ;
}

double *secB(double totalDur)
{







int main(void)
{
	double *partA, *final ;
	double durA, totalDur ;
	int nFramesA, startFrame ;
	int nchan = 2 ;

	setChannelCount(2) ;

	partA = secA(&durA) ;	// create the three sequences
	
	totalDur = durA ;

	final = (double *)calloc(totalDur * SAMPLING_RATE * nchan, sizeof(double));

	nFramesA = durA * SAMPLING_RATE ;	// sequence durations in samples
	//nFramesB = durB * SAMPLING_RATE ;
	//nFramesC = durC * SAMPLING_RATE ;

	startFrame = 0 ;

	stereoMix(final, startFrame, nFramesA, partA) ;

	saveSoundfile("project1.wav", final, totalDur) ;

	free(partA) ;

	return 0 ;
}





