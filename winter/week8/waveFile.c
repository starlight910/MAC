#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * int *scaleTo24Bits(double *sound, int samples) 
 *
 * takes in 'sound' which is a double, of 'samples' in length, and
 * returns and the equivalent values as a 32-bit int
 */

int *scaleTo24Bits(double *sound, int samples)
{
	const double dbMax = 144 ;		// -0.5 dB less than max
	const double ymax = pow(10.0, (dbMax/20.0)) / 2 ;		// linear 
	const double ymin = -ymax ;

	double xmax, xmin, *x, scale ;
	int *data32, *p32 ;

	xmax = xmin = *sound ;		// find current max and min
	for ( x = sound + 1 ; x < sound + samples ; x++ )
		if ( *x > xmax ) xmax = *x ;
		else if ( *x < xmin ) xmin = *x ;

	// allocation memory for int data32
	p32 = data32 = (int *)malloc(samples * sizeof(int)) ;

	// scale factor
	scale = ( ymax - ymin ) / ( xmax - xmin ) ;

	// scale the sound if needed
	if ( scale < 1.0 ) {	// scale the sound
		for ( p32 = data32, x = sound ; p32 < data32 + samples ; p32++, x++ ) 
			*p32 = (int)(scale * ( *x - xmin ) + ymin) ;
	} else {				// no scaling required, just copy
		for ( p32 = data32, x = sound ; p32 < data32 + samples ; p32++, x++ )
			*p32 = (int)( *x ) ;
	}

	return data32 ;		// return integer equivalent
}





















/*
 * convertDoubleTo24(double *data, int nSamples)
 *
 * converts double 'data' into an array of 24-bit chars 
 * via a 'union'
 */

char *convertDoubleTo24(double *data, int nSamples)
{
	union {				// 'union' needed to convert 4 bytes of int
		int i ;			// into 3 bytes of 24 bits
		char c[4] ;
	} x ;

	char *data24, *p24 ;
	int *data32, *p32 ;

	// first convert doubles into 32bit ints
	p32 = data32 = scaleTo24Bits(data, nSamples) ;

	// now convert 32-bit ints into 24-bit chars
	p24 = data24 = (char *)calloc(nSamples * 3, sizeof(char)) ;

	while ( nSamples-- ) {
		x.i = *p32++ ;			// save the integer value as an int
		*p24++ = x.c[0] ;		// read the int as first three chars
		*p24++ = x.c[1] ;
		*p24++ = x.c[2] ;
	}

	free(data32) ;

	return data24 ;
}

/*
 * define the structs for the WAVE header
 */

typedef struct ID {		// ID : used by each of the other 'chunks'
	char ckID[4] ;		// name of this chunk
	int cksize ;		// size of this chunk
} ID ;

typedef struct RIFF_CHUNK {	// 1st chunk: RIFF
	ID id ;
	char waveID[4] ;		// this must be WAVE for a WAVE file
} RIFF_CHUNK ;

typedef struct FORMAT_CHUNK {	// 2nd chunk: fmt
	ID id ;
	short wFormatTag ;			// type of encoding: WAVE_FORMAT_PCM
	short nChannels ;			// number of channels
	int nSamplesPerSec ;		// the sampling rate
	int nAvgBytesPerSec ;		// SAMPLING_RATE * nchan * sizeof(short)
	short nBlockAlign ;			// nchan * (BITS_PER_SAMPLE/8)
	short wBitsPerSample ;		// sample size: 16-bits
} FORMAT_CHUNK ;

typedef struct DATA_CHUNK {		// 3rd chunk: data
	ID id ;
} DATA_CHUNK ;

static int WAVE_FORMAT_PCM = 0x0001 ;	// defined by Microsoft

/* writeWaveSoundfile(char *name, double *sound, double dur, int sr, 
 *		int nchan)
 *
 * writes out double array 'sound' to the soundfile 'name',  
 *
 */
void writeWaveSoundfile24(
		char *soundfile, double *sound, double dur, int srate, int nchan)
{
	RIFF_CHUNK riff ;
	FORMAT_CHUNK fmt ;
	DATA_CHUNK data ;
	int bytesPerSample, dataSizeInBytes, nSamples ;
	int bitsPerSample = 24 ;
	char *bits24 ;
	FILE *outputFile ;

	bytesPerSample = bitsPerSample / 8 ;
	nSamples = dur * srate * nchan ;		// data size in samples
	dataSizeInBytes = nSamples * bytesPerSample ;	// data size in bytes

	strncpy(riff.id.ckID, "RIFF", 4) ; // set up the RIFF chunk
	riff.id.cksize = 
			(sizeof(RIFF_CHUNK) - sizeof(ID)) 
			+ sizeof(FORMAT_CHUNK) 
			+ sizeof(DATA_CHUNK) 
			+ dataSizeInBytes ;
	strncpy(riff.waveID, "WAVE", 4) ;

	// set up the 'fmt ' chunk
	strncpy(fmt.id.ckID, "fmt ", 4) ;
	fmt.id.cksize = sizeof(FORMAT_CHUNK) - sizeof(ID) ;
	fmt.wFormatTag = WAVE_FORMAT_PCM ;
	fmt.nChannels = nchan ;
	fmt.nSamplesPerSec = srate ;
	fmt.nAvgBytesPerSec = srate * nchan * bytesPerSample ;
	fmt.nBlockAlign = nchan * bytesPerSample ;
	fmt.wBitsPerSample = bitsPerSample ;

	// set up the 'data' chunk
	strncpy(data.id.ckID, "data", 4) ;
	data.id.cksize = (sizeof(DATA_CHUNK) - sizeof(ID)) + dataSizeInBytes ;

	// convert the double data to 24-bit chars
	bits24 = convertDoubleTo24(sound, nSamples) ;

	// open the output file for binary writes
	if ( ( outputFile = fopen(soundfile, "wb") ) == NULL ) {
		printf("Can't open %s for writing\n", soundfile) ;
		exit(EXIT_FAILURE) ;
	}

	fwrite(&riff, sizeof(RIFF_CHUNK), 1, outputFile) ;		// write RIFF chunk
	fwrite(&fmt, sizeof(FORMAT_CHUNK), 1, outputFile) ;		// write fmt chunk
	fwrite(&data, sizeof(DATA_CHUNK), 1, outputFile) ;		// write data chunk

	// write the sample data as 24-bit chars
	fwrite(bits24, sizeof(char), dataSizeInBytes, outputFile) ;

	fclose(outputFile) ;	// close the output file

	return ;
}
/*
 * makeTone(double F, double dB, double dur, int sr, int nchan)
 *
 * create a sine tone of 'F' hertz, 'dB' decibels, 'dur' seconds and
 * 'sr' sampling rate, 'nchan' number of channels
 */
double *makeTone(double F, double dB, double dur, int sr, int nchan) 
{
	double phase, phaseInc, A ;
	int frames ;
	double *sound, *s ;
	double twopi = 2 * M_PI ;

	phase = 0 ;
	phaseInc = ( twopi * F ) / sr ;	// phase increment for this frequency
	frames = dur * sr ;			// duration of sound in frames
	A = pow(10.0, dB/20.0) / 2 ;	// convert decibels to linear amplitude

	// allocate memory for sound
	s = sound = (double *)calloc(frames * nchan, sizeof(double)) ;

	if ( nchan == 1 ) {
		while ( frames-- ) {	// create the sound
			*s++ = A * sin(phase) ;
			phase += phaseInc ;		// increment phase
			if ( phaseInc >= twopi )	// wrap around phase if needed.
				phaseInc -= twopi ;
		}
	} else {
		double phase2 = 0 ;
		double phase2Inc = ( twopi * ( F * 1.5 ) ) / sr ;
		double e1 = 1, e2 = 0, einc = 1.0 / ( frames - 1.0 ) ;
		while ( frames-- ) {	// create the sound
			*s++ = (e1 * A) * sin(phase) ;
			phase += phaseInc ;		// increment phase
			if ( phaseInc >= twopi )	// wrap around phase if needed.
				phaseInc -= twopi ;
			*s++ = (e2 * A) * sin(phase2) ;
			phase2 += phase2Inc ;		// increment phase2
			if ( phase2Inc >= twopi )	// wrap around phase2 if needed.
				phase2Inc -= twopi ;
			e1 -= einc ;	// decrescendo
			e2 += einc ;	// crescendo
		}
	}

	return sound ;
}
int main(void)
{
	double F = 440 ;		// frequency in Hertz
	double db = 144 ;		// amplitude in dB
	double dur = 5 ;		// duration  in  seconds
	int sr = 96000 ;		// sampling rate
	char *soundfile = "twave.wav" ;	// name of output soundfile
	int nchan = 2 ;			// number of channels of sound
	double *sound ;			// array to hold the samples 

	sound = makeTone(F, db, dur, sr, nchan) ;	// create the tone
	writeWaveSoundfile24(soundfile, sound, dur, sr, nchan) ;// write soundfile

	free(sound) ;
	return 0 ;
}
