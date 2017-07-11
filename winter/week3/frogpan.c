#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include<cm.h>

//copies a mono input into one stereo output location
void stereoPan(double *stereoOut, int startSample, int inputSamples, double *monoIn, double pan)
{
	double Left, Right, boost ;

	Left = pan ;
	Right = 1- pan ;

	boost = 1.0 / sqrt(Left*Left + Right*Right) ;  //equal power boost

	Left *= boost ;    //boost both Left and Right
	Right *= boost ;

	stereoOut +=startSample ;
	while (inputSamples--)
	{
		*stereoOut++ = Left * *monoIn ;   //place the tone at location
		*stereoOut++ = Right * *monoIn++ ;
	}
	return ;
}

//copies a stereo input to a stereo output
void stereoCopy(double *stereoOut, double *stereoIn, double dur, double skipDur)
{
	int nchan = 2 ;
	int skipFrames = skipDur * SAMPLING_RATE ;
	int nFrames = dur *SAMPLING_RATE ;

	stereoOut += ( skipFrames * nchan ) ;
	while(nFrames--)
	{
		*stereoOut++ += *stereoIn++ ; //left channel
		*stereoOut++ += *stereoIn++ ; //Right channel
	}

	return ;
}

//shift a list of frequencies by 'cents'
void shift(double *flist, int N, double cents)
{
	double K = 1200 / log10(2.0) ;
	double multiple = pow(10.0, cents/K) ; //change from log to linear

	while (N--)
	{
		*flist = *flist * multiple ;
		flist++ ;
	}

	return ;
}

double *setFlist(int *N)  //returns an ascending major scale
{
	double fundamental = 220 ;
	double majorScaleIntervals[] = {200, 200, 100, 200, 200, 200, 100} ;

	double K = 1200 / log10(2.0) ;
	double F, *flist, *f ;
	int i ;

	*N = 24 ; //number of tones in sequence

	flist = (double *)calloc(*N, sizeof(double)) ;

	F = fundamental ;

	// create a major scale
	for ( f = flist, i = 0 ; f < flist + *N ; f++, i++)
	{
		*f = F ;
		F = F * pow(10.0, majorScaleIntervals[i%7]/K) ;
		if ( F >= 2 * fundamental)  //when octave is reached
			F = fundamental ;       // go back to the fundamental
	}

	return flist ;
}

double *setPanList(int N)    //returns a list of pan location between 0-1
{
	double *panList, *p ;
	double pan, panInc = -0.1 ;  //distance between pan locations
	double precision = 0.1 ;

	panList = (double *)calloc(N, sizeof(double)) ;
	pan = 1 ;  // initial pan position

	//creat a zig=zag pattern, moving from 0-1, 1-0, 0-1, etc
	for ( p = panList ; p < panList + N ; p++ )
	{
		pan = ((int)(pan/precision + 0.5) * precision) ;
		*p = pan ;      //set location
		pan += panInc ; //move the pan

		if (pan == 1.0 || pan == 0.0)  //if limit is reached
			panInc = -panInc ;         //reverse direction
	}
	return panList ;
}

// create a list of tones separated by 'silenceDur'
// and placed at 'panList' locations in the stereo field
// returns the list, and the totalDuration
double *makeSequence(double *clist, double *panList, int N, double toneDur, double silenceDur, double *totalDur)
{
	int samples, silenceSamples, start, i ;
	double *stereoOut, *oneTone ;
	double carrier, pan ;
	int nchan = 2 ;
	double dB = 144 ;

	*totalDur = N * (toneDur + silenceDur) ;
	samples = toneDur * SAMPLING_RATE ;
	silenceSamples = silenceDur * SAMPLING_RATE ;

	stereoOut = (double *)calloc(*totalDur * nchan * SAMPLING_RATE, sizeof(double)) ;

	for (i = 0, start = 0 ; i < N ; i++)
	{
		carrier = *clist++ ;
		oneTone = bassoon(carrier, dB, toneDur) ;     //create sound

		pan = *panList++ ;
		stereoPan(stereoOut, start, samples, oneTone, pan) ;

		// increment the start index, skipping over the silence
		start += (samples + silenceSamples) * nchan ;

		free(oneTone) ;
	}

	return stereoOut ;
}

// copy the three voices into a canon, with 'gapDuration' between them
double *makeCanon(double *v1, double *v2, double *v3, double sequenceDur, double gapDuration, double *totalDur)
{
	double *stereo ;
	int nchan = 2 ;

	// calculate total duration of the final canon, allocate memory
	*totalDur = sequenceDur + 2 * gapDuration ;
	stereo = (double *)calloc(*totalDur*nchan*SAMPLING_RATE, sizeof(double)) ;

	// copy 1st voice with delay of 0
	stereoCopy(stereo, v1, sequenceDur, 0.0) ;
	// copy 2nd voice with delay of 'gapDuration'
	stereoCopy(stereo, v2, sequenceDur, gapDuration) ;
	// copy 3rd voice with delay of '2*gapDuration'
	stereoCopy(stereo, v3, sequenceDur, 2*gapDuration) ;

	return stereo ;
}

int main(void)
{
	double toneDur = 0.25 ;           // duration of each tone
	double silenceDur = 3 * toneDur ;   // duration of each silence

	double sequenceDur, totalDur ;
	double *v1, *v2, *v3, *canon ;    //arrays for voices, and output

	double *flist ;      //list of frequenices
	double *panList ;    // list of pan locations
	int nTones, nSkip ;
	double gapDuration ;

	setRandSeed(time(0));  // initialize random number generator

	flist = setFlist(&nTones) ;
	panList = setPanList(nTones) ;

	// make the first voice
	v1 = makeSequence(flist, panList, nTones, toneDur, silenceDur, &sequenceDur) ;
 
	shift(flist,nTones, 1200) ;    //shift pitches, make 2nd voice
	v2 = makeSequence(flist, panList, nTones, toneDur, silenceDur, &sequenceDur) ;

	shift(flist, nTones, -2400) ;  //shift pitches, make 3rd voice
	v3 = makeSequence(flist, panList, nTones, toneDur, silenceDur, &sequenceDur) ;

	// Now create the canon
	nSkip = 5 ;
	gapDuration = nSkip * silenceDur ;
	canon = makeCanon(v1, v2, v3, sequenceDur, gapDuration, &totalDur) ;

	// set the output number of channels
	setChannelCount(2) ;

	playbuffer(canon, totalDur) ;

	saveSoundfile("melody.wav", canon, totalDur) ;
	
	return 0 ;
}
