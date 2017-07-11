#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <math.h>

typedef struct Z 
{
    int index ;
    double *data ;
    int samples ;
    int ms ;
    struct Z *next ;
} Z ;

//stereo sound to mono
void makeMono(double **data, double dur)
{
	int i, frames, samples ;
	double *mono, *m, *pd ;
	double xmin, xmax ;

	frames = dur * SAMPLING_RATE ;
	samples = frames * CHANNEL_COUNT ;

	getLimits(*data, samples, &xmax, &xmin);

	mono = (double *)Calloc(frames, sizeof(double)) ;

	for ( m = mono, pd = *data ; m < mono + frames ; m++ ) {
		for ( i = 0 ; i < CHANNEL_COUNT ; i++ )
			*m += *pd++ ;
	}

	rescale(mono, frames, xmin, xmax) ;

	*data = mono ;

	Free(*data) ;

	return ;
}

void addToList(Z *new, Z **ZList)
{
    Z *z ;

    if ( *ZList == NULL ) {
        *ZList = new ;
        new->next = NULL ;
        return ;
    }

    for ( z = *ZList ; z->next != NULL ; z = z->next )
        ;

    z->next = new ;
    new->next = NULL ;

    return ;
}

void setupLL(double *data, double dur, Z **ZList)
{
    int maxDurMS = 100, minDurMS = 20 ; 
	int durationInMilliseconds ;
    double *pd, *pz ;
    Z *z ;
    int counter = 0 ;

    durationInMilliseconds = dur * 1000 ;

    pd = data ;

    while ( durationInMilliseconds > minDurMS ) {

        z = (Z *)Calloc(1, sizeof(Z)) ;

        z->index = counter++ ;

        // random extract duration in milliseconds
        z->ms = (maxDurMS - minDurMS) * frand() + minDurMS ;
        // convert duration from milliseconds to samples
        z->samples = ( z->ms * 0.001 ) * SAMPLING_RATE ;

        // allocate and copy the sound samples
        z->data = (double *)Calloc(z->samples, sizeof(double)) ;
        for ( pz = z->data ; pz < z->data + z->samples ; pz++ )
            *pz = *pd++ ;

        addToList(z, ZList) ;   // add to the linked list

        durationInMilliseconds -= z->ms ;
    }

	Free(data) ;
    return ;
}

/*
 * selectArray(*ZList, array[], minMS, maxMS, *zcount)
 *
 * select those extracts from the linked list that are between minMS
 * and maxMS in length, and put them into the array.
 */

void selectArray(Z *ZList, Z array[], int minMS, int maxMS, int *arrayLen)
{
    Z *z ;
    *arrayLen = 0 ;
    for ( z = ZList ; z != NULL ; z = z->next ) {
        if ( z->ms >= minMS && z->ms < maxMS ) {
            array[*arrayLen] = *z ;
            *arrayLen += 1 ;
        }
    }
    return ;
}

void showArray(Z *zlist, int len)
{
    int i, totalSamples ;
    Z *z ;

    totalSamples = 0 ;
    for ( i = 0 ; i < len ; i++ ) {
        z = &zlist[i] ;
        fprintf(stderr,"\tindex: %2d  ms: %2d  samples: %d\n", 
                z->index, z->ms, z->samples) ;
        totalSamples += z->samples ;
    }
    fprintf(stderr,"total duration: %f seconds\n", totalSamples/SAMPLING_RATE);
    fputc('\n', stderr) ;

    return ;
}

double getDuration(Z *zlist, int N)
{
    double dur ;
    int i ;
    Z *z ;

    dur = 0 ;
    for ( i = 0 ; i < N ; i++ ) {
        z = &zlist[i] ;
        dur += z->ms * 0.001 ;
    }

    return dur ;
}

void rampCopy(double *out, double *in, int samples)
{
    int i, rampLen = samples / 10 ;     // duration of the ramp
    double R, rinc = 1.0 / ( rampLen - 1 ) ;    // ramp increment

    samples -= rampLen * 2 ;    // subtract ramp duration from total

    for ( i = 0, R = 0 ; i < rampLen ; i++, R += rinc ) // ramp up
        *out++ = R * *in++ ;

    while ( samples-- )     // copy
        *out++ = *in++ ;

    for ( i = 0, R = 1 ; i < rampLen ; i++, R -= rinc ) // ramp down
        *out++ = R * *in++ ;

    return ;
}





void spread(Z *zArray, int nParts, double **channel8, double finalDuration)
	{
    int silence, start, i, channel, samples, j ;
    double dur, *out, *in ;
    Z *z ;

    dur = getDuration(zArray, nParts) ; // get the duration of the entire list

    finalDuration -= dur ;          // subtract 

    // silence duration in samples
    silence = (finalDuration / nParts)  * SAMPLING_RATE ;

    for ( i = 0, start = 0 ; i < nParts ; i++ ) 
	{
        start += silence ;      // increment start by 'silence' samples
        channel = irand() % 8 ; // randomly choose a channel

        z = &zArray[i] ;         // assign 'z' to the current extract
        samples = z->samples ;  // duration of current extract

        out = channel8[channel] + start ; // set pointer in the 8channel array
        in = z->data ;                  // set pointer for the input

        rampCopy(out, in, z->samples) ; // ramp & copy the extract

        start += z->samples ;   // increment start time by dur in samples
    }

    // now backwards, same thing

    start = silence / 2 ;   // start with 1/2 of silence
    j = nParts - 1 ;        // set 'j' to last item in array

    for ( i = 0 ; i < nParts ; i++, j-- ) {

        channel = irand() % 8 ;

        z = &zArray[j] ;	// set z to the end of the array (j)
        samples = z->samples ; 

        out = channel8[channel] + start ;
        in = z->data ;

        rampCopy(out, in, z->samples) ;

        start += z->samples ;
        start += silence ;
    }

    return ;
}

 void spread2(Z *zArray, int nParts, double **channel8, double finalDuration)
{
	int silence, start, i, channel, samples ;
	double dur, *out, *in ;
	Z *z ;
	
	dur = getDuration(zArray, nParts) ;
	finalDuration -= dur ;
	silence = (finalDuration /nParts)  * SAMPLING_RATE ;

	for ( i = 0, start = 0 ; i < nParts ; i++ )
	{
		start += silence/4 ;      
		channel = irand() % 8 ; 
		z = &zArray[i] ;         
		samples = z->samples ;  
		out = channel8[channel] + start ; 
		in = z->data ;                  
		rampCopy(out, in, z->samples) ; 
		start += z->samples ;
		start += silence/4 ;
	}
	return ;
}



int main(void)
{
    Z *ZList = NULL ;   // linked list

    Z Z1[1000], Z2[100], Z3[100], Z4[100], Z5[100], Z6[100], Z7[100], Z8[100], Z9[100], Z10[100] ;  // arrays of structs
    int len1, len2, len3, len4, len5, len6, len7 ;            // lengths of arrays
    double *data, dur ;     // sound data, its duration
    double **channel8 ;     // 8 channel output
    double totalDuration ;
    double *finalOutput ;

    data = readSoundfile("runaway96.wav", &dur) ;
	if ( CHANNEL_COUNT != 1 )
		makeMono(&data, dur) ;
	setBitsPerSample(24) ;
	
    // parse 'data' into specified parts, create the linked list
    setupLL(data, dur, &ZList) ;
	
    // select those parts within the specified durations
	
    selectArray(ZList, Z1, 20, 30, &len1) ;
	selectArray(ZList, Z2, 50, 100, &len1) ;
	selectArray(ZList, Z3, 30, 40, &len2) ;
	selectArray(ZList, Z4, 20, 70, &len2) ;
	selectArray(ZList, Z5, 60, 80, &len3) ;
	selectArray(ZList, Z6, 20, 100, &len4) ;
	selectArray(ZList, Z7, 80, 100, &len5) ;
	selectArray(ZList, Z8, 20, 100, &len5) ;
	selectArray(ZList, Z9, 60, 100, &len6) ;
	selectArray(ZList, Z10, 30, 100, &len7) ;

    // display data
    showArray(Z1, len1) ; 
	showArray(Z2, len1) ; 
	showArray(Z3, len2) ; 
	showArray(Z4, len2) ;
	showArray(Z5, len3) ;	
	showArray(Z6, len4) ;
	showArray(Z7, len5) ;
	showArray(Z8, len5) ;
	showArray(Z9, len6) ;
	showArray(Z10, len7) ;

    totalDuration = 60 ; // total duration of output
    channel8 = channel8Allocate(totalDuration) ;    // allocation memory

    spread2(Z1, len1, channel8, totalDuration) ;
	spread(Z2, len1, channel8, totalDuration) ;
	spread2(Z3, len2, channel8, totalDuration) ;
	spread(Z4, len2, channel8, totalDuration) ;
	spread(Z5, len3, channel8, totalDuration) ;
	spread(Z6, len4, channel8, totalDuration) ;
	spread2(Z7, len5, channel8, totalDuration) ;
	spread(Z8, len5, channel8, totalDuration) ;
	spread(Z9, len6, channel8, totalDuration) ;
	spread(Z10, len7, channel8, totalDuration) ;


    finalOutput = channel8Interleave(channel8, totalDuration) ;

    channel8Free(channel8) ;

    saveSoundfile("Junho_P1.wav", finalOutput, totalDuration) ;

    Free(finalOutput) ;

    return 0 ;
}
