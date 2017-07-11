#ifndef __HEADER_H__
#define __HEADER_H__
// in init.c
extern double SAMPLING_RATE ;
extern int BITS_PER_SAMPLE ;
extern int CHANNEL_COUNT ;

// set sampling rate, channel count and  BITS_PER_SAMPLE
extern void init(int nchan, int srate, int bps) ;	
extern double getSamplingRate(void) ;
extern int getBitsPerSample(void) ;
extern int  getChannelCount(void) ;
extern void setChannelCount(int nchan) ;
extern void setSamplingRate(int sr) ;
extern void setBitsPerSample(int bps) ;

// in rand.c
extern void setRandSeed(int newSeed) ;	// initialize random number generator
extern int irand(void) ;	// Park-Miller random number generator
extern double frand(void) ;
extern void uniqueRandomListI(int *list, int N) ;
extern void uniqueRandomListD(double *list, int N) ;
extern void shuffleD(double *list, int N) ;
extern void shuffleI(int *list, int N) ;

// in scale.c
extern void getLimits(double *data, int nSamp, double *xmax, double *xmin);
extern void scale(double *data, int samples, double dB) ;
extern void rescale(double *data, int N, double ymin, double ymax) ;
extern void linearScale(double *data, int samples, double A) ;

// in sort.c
extern void insertionSortD(double *a, int N, int direction) ;
extern void insertionSortI(int *a, int N, int direction) ;
extern void bubbleSortD(double *data, int N, int direction) ;
extern void bubbleSortI(int *data, int N, int direction) ;

// in waves.c
extern double *Square(double F, double dB, double dur, int nPartials);
extern double *Triangle(double F, double dB, double dur, int nPartials);
extern double *Sawtooth(double F, double dB, double dur, int nPartials);
extern double *Sine(double frequency, double dB, double dur);

// in mix.c
extern double *mix(int nSounds, double *outputDur, ...) ;
extern double *concatenate(int nSounds, double *totalDur, ...) ;

// in io.c
extern void saveSoundfile(char *name, double *data, double dur) ;
//extern void saveSoundfile16(
//		char *name, double *data, double dur, int sr, int nchan);
//extern void saveSoundfile24(
//		char *name, double *data, double dur, int sr, int nchan) ;
//extern double *readSoundfile16(char *name, double *dur, int *sr, int *nchan) ;
//extern double *readSoundfile24(char *name, double *dur, int *sr, int *nchan) ;
extern double *readSoundfile(char *filename, double *dur) ;
extern double *sfread(char *filename, 
        int *sr, int *nchan, double *dur, int *bps);

// in aoplay.c
extern void playbuffer(double *data, double dur) ;

// in path.c
extern double *AddsynPath(
		double *flist,		// list of frequencies in Hertz
		double *dblist,		// list of amplitudes, in dB
		double *tlist,		// list of time points between 0 and 1
		int nPoints,		// number of points
		double dur,			// duration in seconds

		double *propFreq,	// proportional frequences, 1 to (srate/fund)
		double *propAmp,	// proportional amplitudes, 0 to 1
		int nHarmonics);	// number of harmonics

extern double *SquarePath(
		double *flist, double *dblist, double *tlist, int nPoints, double dur) ;

extern double *SawtoothPath(
		double *flist, double *dblist, double *tlist, int nPoints, double dur) ;

extern double *TrianglePath(
		double *flist, double *dblist, double *tlist, int nPoints, double dur) ;

// in env.c
extern void envelope(double *sound, double dur, double *T, double *A, int N) ;
extern void applyRandomEnv(double *sound, double dur) ;
extern void randomEnvelope(double *env, int len, double amax) ;
extern void linearEnv(double *env, int len, double *X, double *Y, int N) ;
extern void exponentialEnv(double *env, int len, double *X, double *Y, int N) ;
extern void applyRandomEnv2(double *sound, double dur, int mxP, int mnP) ;

// in addsyn.c
extern double *addsyn(double fundamental, double D, double DB,
		double *harmonicsF, double *harmonicsA, int nHarmonics) ;
extern double *addsynRandomEnv(double fundamental, double D, double DB,
		double *harmonicsF, double *harmonicsA, int nHarmonics) ;

// in pluck.c
extern double *pluck(double f, double db, double decayDB, double dur) ;


// in fm.c
extern double *bassoon(double F, double db, double dur);
extern double *brass(double F, double db, double dur);
extern double *bell(double F, double db, double dur);
extern double *clarinet(double F, double db, double dur);
extern double *woodwind(double F, double db, double dur);
extern double *wooddrum(double F, double db, double dur);
extern double *drum(double F, double db, double dur);
extern double *cello(double f, double db, double dur);
extern double *am(double C, double M, double mi, double db, double dur);


// in aqua.c
extern int aqua_makeWindow(char *windowName, int width, int height, int margin);
extern int aqua_makeDefaultWindow(char *windowName) ;
extern void aqua_close(int index);
extern void plot_setSleepTime(int index, double seconds) ;
extern void aqua_plotDouble(int ID, double *data, int N, double ymax, double ymin);
extern void aqua_setTitle(int id, char *newTitle);

// in gliss.c
extern double *SineGliss(double f1, double f2, double dB, double dur) ;
extern double *SawtoothGliss(double f1, double f2, double dB, 
		double dur, int nPartials) ;
extern double *TriangleGliss(double f1, double f2, double dB, 
		double dur, int nPartials) ;
extern double *SquareGliss(double f1, double f2, double dB, 
		double dur, int nPartials) ;

// in grain.c

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

enum waveTypes {gSINE, gSAWTOOTH, gSQUARE, gTRIANGLE, gPARALLELOGRAM} ;

extern double *grainCloud(GRAIN g, double duration) ;
extern double *grainDrift(GRAIN g, double duration) ;

// in channel8.c
extern void channel8PanSequence(
        int channelSeq[], double timeSeq[], int nPoints,
        int initialFrame, int totalFrames,
        double **buffer8, double *sound) ;
extern void channel8Pan(int c0, int c1, 
        int startFrame, int nFrames, 
        double **eightChannelArray, double *sound) ;
extern void channel8Free(double **data8) ;
extern double **channel8Allocate(double totalDuration) ;
extern double *channel8Interleave(double **channel8, double dur) ;
extern void channel8Sequence(double **out, double **in, int start, int len) ;

// in srconvert.c
extern void srateConvert(double **data, int *srIn, int framesIn, int nchan);

// in sfmalloc.c
#include <sfmalloc.h>
#include <fftList.h>
#include <gnuplot_i.h>

#endif
