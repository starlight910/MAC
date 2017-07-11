#include <stdio.h>

double **makeCycles(int *pathArray, int nSteps, double nCycles,
		double *totalDur, double *monoInput, int totalFrames) 
{

int nFrames, nTrans, chA, chB, i ;

nFrames = totalFrames / (nSteps * nCycle) ;

nTrans = nSteps * nCycles ; 

for ( i = 0 ; i < nTrans ; i++ ) {
	chA = pathArray[i%nSteps] ;
	chB = pathArray[(i+1)%nSteps] ;

	pan8(nFrames, start, array2D, chA, chB, monoInput) ;
	start += nFrames
}
return array2D ;
}

