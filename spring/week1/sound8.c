#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cm.h>

void makeSeq(double *out, int start, int samples, double *in)
{
    out += start ;
    while ( samples-- )
        *out++ = *in++ ;
    return ;
}

/*
 * double **makeSoundArray(dur, nchan)
 *
 * returns a 2 X nchan pointer array, in which each separate channel
 * is a distinct tone with a unique envelope.
 *
 * each tone is 'dur' seconds long
 */

double **makeSoundArray(double dur, int nchan)
{
	double dB = 132, F = 440 ;
	double wholeStep = pow(2.0, 2.0/12.0) ;	// whole step between tones

	double **array2D,    // array for the 2D array
           *oneTone ;   // array for a single tone

    int nPartials = 10 ;
	int ch, frames, start ; 

    double T[] = {0.0, 0.5, 1.0} ;  // time points
    double A[] = {0.0, 1.0, 0.0} ;  // amplitude points
    int nPoints = 3 ;
    double peakInc ;

    frames = dur * SAMPLING_RATE ;  // duration of each tone in frames

	// allocate memory for 'nchan' array of pointers (1st dimension)    
    array2D = (double **)calloc(nchan, sizeof(double *)) ;

    // allocate memory for each single array (2nd dimension)
	for ( ch = 0 ; ch < nchan ; ch++ ) 
	 	array2D[ch] = (double *)calloc(frames, sizeof(double)) ;

    peakInc = 1.0 / ( nchan + 1 ) ; // location of amplitude peaks
    T[1] = peakInc ;                // peak for 1st tone

    start = 0 ;
	for ( ch = 0 ; ch < nchan ; ch++ ) {	// create each sound

        oneTone = Triangle(F, dB, dur, nPartials) ; // create one tone
        envelope(oneTone, dur, T, A, nPoints) ;     // apply envelope

        makeSeq(array2D[ch], start, frames, oneTone) ; // copy to channel

        free(oneTone) ;

		F *= wholeStep ;		// raise frequency by whole step
        T[1] += peakInc ;       // increment time of next amplitude peak 
	}

	return array2D ;
}

//
//	interleave(sound2Darray, dur, sr, nChan)
//		combine a 2D array into a 1D array with interleaved channels
//

double *interleave(double **input2D, const double dur, int nChan)
{
	int i, samples, frames ;
	double *output1D, *pi, *po ;

    frames = dur * SAMPLING_RATE ;
    samples = frames * nChan ;

    // create 1-D array big enough to hold the input2D array
    output1D = (double *)calloc(samples, sizeof(double)) ;

    for ( i = 0 ; i < nChan ; i++ ) {

        po = output1D + i ;   // set ouptut pointer to correct start address

        // set input point to start of array
        for ( pi = input2D[i] ; pi < input2D[i] + frames ; pi++ ) {
            *po = *pi ;     // increment input pointer by 1
            po += nChan ;   // skip every 'nChan' samples
        }
    }

	return output1D ;    // return 1-D array
}

int main(int argc, char *argv[])
{
    double **array2D ;   // two-dimensional array for individual sounds
	double *array1D ;   // one-dimonsional for interleaved samples

	double dur = 30 ;       // duration for each tone
	char soundfile[64] ;    // array for output soundfile name
	int i, nchan ;

    if ( argc == 1 )        // how many channels (tones) in the sound?
        nchan = 2 ;         // 2 channels (default)
    else 
        nchan = atoi(argv[1]) ; // what the user asks for

    if ( nchan > 8 || nchan < 2 ) {     // exit if 'nchan' is unacceptable
        printf("usage: %s [nchan 2-8]\n", argv[0]) ;
        exit(EXIT_FAILURE) ;
    }

    setChannelCount( nchan ) ;  // set number of channels

    // create the 'nchan' dimensional array
	array2D = makeSoundArray(dur, nchan) ;

    // interleave the 'nchan' array into a 1-dimensional array 
	array1D = interleave(array2D, dur, nchan) ;

	for ( i = 0 ; i < nchan ; i++ ) // release memory
		free(array2D[i]) ;       // for each data channel
	free(array2D) ;              // for the array of pointers

	sprintf(soundfile, "test%d.wav", nchan) ;   // name of output file
	saveSoundfile(soundfile, array1D, dur) ;    // save the data

	free(array1D) ;     // release memory

	exit(EXIT_SUCCESS) ;
}
