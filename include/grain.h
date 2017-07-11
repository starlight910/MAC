#ifndef __GRAIN_H__
#define __GRAIN_H__

/*
*   definitions for the grain generator
*/

typedef struct GRAIN {
	int density ;		// number of grains
	double F0, F1 ; 	// frequency in hertz
	double DB0, DB1 ; 	// amplitude in dB
	double maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)
	int seed ;			// random number seed
} GRAIN ;

enum waveTypes {SINE, SAWTOOTH, SQUARE, TRIANGLE, PARALLELOGRAM} ;

// extern void grain(double *b, double st, double dur, GRAIN g) ;
// extern void grainDrift(double *b, double st, double dur, GRAIN g) ;

#endif

