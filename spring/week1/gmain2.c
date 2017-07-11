#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cm.h"

void doit(int channelSeq[], double timeSeq[], int nPoints,
        double **channel8, double *data, double dur)
{
    int initialFrame = 0 ;
    int totalFrames = dur * SAMPLING_RATE ;

    shuffleI(channelSeq, nPoints) ;

    uniqueRandomListD(timeSeq, nPoints) ;
    rescale(timeSeq, nPoints, 0.0, 1.0);
    bubbleSortD(timeSeq, nPoints, 0) ;

    channel8PanSequence(
        channelSeq, timeSeq, nPoints,
        initialFrame, totalFrames,
        channel8, data) ;

    return ;
}

int main(void)
{
	GRAIN g ;
	double *data, *data2, *data3, *data4 ;
	double dur = 20 ; 
	double K = 1200 / log(2.0) ;
	double cents = 300 ;
    double **channel8 ;
    int channelSeq[] = {0, 5, 2, 6, 3, 7, 4, 1} ;
    double timeSeq[] = {0.0, 0.14, 0.28, 0.42, 0.57, 0.71, 0.85, 1.0} ;
    int nPoints = 8 ;
    int i ;

	g.density = 1000 ;	

	g.maxD = 300 ;	// in milliseconds
	g.minD = 250 ;

	g.F0 = 440 ;
	g.F1 = g.F0 * exp(cents/K) ;	// 300 cents
	g.DB0 = 120 ;
	g.DB1 = 138 ;

	g.type = gSINE ;
	g.seed = 0 ;

	data = grainCloud(g, dur) ;

    setChannelCount(1) ;

	saveSoundfile("grain.wav", data, dur) ;

	// playbuffer(data, dur) ;

	free(data) ;

	return 0 ;
}
