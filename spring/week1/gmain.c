#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cm.h"

/*
typedef struct GRAIN {
	int density ;		// number of grains
	double F0, F1 ; 	// frequency in hertz
	double DB0, DB1 ; 	// amplitude in dB
	double maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)
	int seed ;			// random number seed
} GRAIN ;
*/

typedef struct grainStruct {
    int index ;
    double startTime, duration ;
	int density ;		// number of grains
	double F0, F1 ; 	// frequency in hertz
	double DB0, DB1 ; 	// amplitude in dB
	int maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)
	int seed ;			// random number seed
    int c0, c1 ;    // channel pan
    struct grainStruct *next ;
} grainStruct ;

void addToList(grainStruct **gList, grainStruct *new)
{
    grainStruct *g ;

    if ( *gList == NULL ) {
        *gList = new ;
        new->next = NULL ;
        return ;
    } 
    
    for ( g = *gList ; g->next != NULL ; g = g->next )
        ;

    g->next = new ;
    new->next = NULL ;

    return ;
}

void showList(grainStruct *gList)
{
    grainStruct *g ;

    for ( g = gList ; g != NULL ; g = g->next ) {
        printf("\tindex: %d\n", g->index) ;
        printf("\t\tstartTime: %f  duration: %f\n", g->startTime, g->duration);
        printf("\t\tf0: %f  f1: %f\n", g->F0, g->F1) ;
        printf("\t\tdb00: %f  db01: %f\n", g->DB0, g->DB1) ;
        printf("\t\tmaxD: %d ms  minD: %d\n", g->maxD, g->minD) ;
        printf("\t\ttype: %d  seed: %d\n", g->type, g->seed) ;
    }

    return ;
}

void readData(char *datafile, grainStruct **gList)
{
    FILE *f ;
    char *p, input[128] ;
    int n, lineNumber = 0 ;
    grainStruct *g ;
    float st, dur, f0, f1, db0, db1 ;
    int density, maxD, minD, type, seed, cStart, cEnd ;

    if ( ( f = fopen(datafile, "r") ) == NULL ) {
        fprintf(stderr,"Can't open '%s' for reading.  Exiting\n", datafile) ;
        exit(EXIT_FAILURE) ;
    }

    lineNumber = 0 ;

    while ( ( p = fgets(input, sizeof(input), f) ) != NULL ) {

        lineNumber++ ;

        if ( input[0] == '#' )      // ignore comment lines
            continue ;

        n = sscanf(input, "%f %f %f %f %d %f %f %d %d %d %d %d %d",
                &st, &dur,
                &f0, &f1,
                &density,
                &db0, &db1,
                &maxD, &minD,
                &cStart, &cEnd,
                &type, &seed);

        if ( n != 13 ) {
            fprintf(stderr,"Error in line %d:\n", lineNumber) ;
            fprintf(stderr,"\t%s\n", input) ;
            fprintf(stderr,"\t13 items expected but %d read\n", n) ;
            exit(EXIT_FAILURE) ;
        }

        g = (grainStruct *)malloc(sizeof(grainStruct));

        g->index = lineNumber ;
        g->startTime = st ; 
        g->duration = dur ;
        g->density = density ; 
        g->F0 =  f0 ; 
        g->F1 =  f1 ; 
        g->DB0 =  db0 ; 
        g->DB1 =  db1 ;
        g->maxD =  maxD ; 
        g->minD =  minD ;
        g->c0 = cStart ;
        g->c1 = cEnd ;
        g->type =  type ;
        g->seed =  seed ;

        addToList(gList, g) ;
    }

    fclose(f) ;

    return ;
}

double getDuration(grainStruct *gList)
{
    grainStruct *g ;
    double maxDuration, dur ;

    maxDuration = 0 ;
    for ( g = gList ; g != NULL ; g = g->next ) {
        dur = g->startTime + g->duration ;
        if ( dur > maxDuration )
            maxDuration = dur ;
    }

    return maxDuration ;
}

void pan(double **array2D, int c0, int c1, double *cloud, int frames, 
        int startFrame)
{
    double *p1, *p2 ;
    double A1, A2, ainc, B ;

    p1 = array2D[c0] ; p1 += startFrame ;
    p2 = array2D[c1] ; p2 += startFrame ;

    A1 = 1 ;
    A2 = 0 ;
    ainc = 1.0 / ( frames - 1 ) ;

    while ( frames-- ) {
        B = 1.0 / sqrt(A1*A1 + A2*A2) ;
        *p1++ = (A1 * B) * *cloud ;
        *p2++ = (A2 * B) * *cloud++ ;
        A1 -= ainc ;
        A2 += ainc ;
    }

    return ;
}

void simpleEnvelope(double *cloud, double dur)
{
    double T[] = {0.0, 0.01, 0.99, 1.0} ;
    double A[] = {0.0, 1.00, 1.00, 0.0} ;
    int nPoints = 4 ;

    envelope(cloud, dur, T, A, nPoints) ;

    return ;
}

double **makeSound(grainStruct *gList, double *totalDur)
{
    double **array2D ;
    double *cloud ;
    int frames, startFrame ;
    GRAIN gg ;
    grainStruct *g ;
    int i, nchan = 8 ;

    *totalDur = getDuration(gList) ;
    frames = *totalDur * SAMPLING_RATE ;

    array2D = (double **)calloc(nchan, sizeof(double *));
    for ( i = 0 ; i < nchan ; i++ )
        array2D[i] = (double *)calloc(frames, sizeof(double)) ;

    startFrame = 0 ;
    for ( g = gList ; g != NULL ; g = g->next ) {

        gg.density = g->density ;
        gg.F0 = g->F0 ; gg.F1 = g->F1 ;
        gg.DB0 = g->DB0 ; gg.DB1 = g->DB1 ;
        gg.maxD = g->maxD ;
        gg.minD = g->minD ;
        gg.type = g->type ;
        gg.seed = g->seed ;

        cloud = grainCloud(gg, g->duration) ;
        simpleEnvelope(cloud, g->duration) ;

        startFrame = g->startTime * SAMPLING_RATE ;
        frames = g->duration * SAMPLING_RATE ;

        pan(array2D, g->c0, g->c1, cloud, frames, startFrame) ;

        free(cloud) ;
    }

    return array2D ;
}

double *interleave(double **input2D, double dur)
{
    int nchan = 8 ;
    int i, samples, frames ;
    double *output1D, *pi, *po ;

    frames = dur * SAMPLING_RATE ;
    samples = frames * nchan ;

    output1D = (double *)calloc(samples, sizeof(double)) ;

    for ( i = 0 ; i < nchan ; i++ ) {
        po = output1D + i ;
        for ( pi = input2D[i] ; pi < input2D[i] + frames ; pi++ ) {
            *po = *pi ;
            po += nchan ;
        }
    }

    return output1D ;
}

int main(int argc, char *argv[])
{
    double totalDur ;
    double **array2D, *array1D ;
    int i, nchan = 8 ;

    grainStruct *g, *gList = NULL ;

    if ( argc != 2 ) {
        printf("usage: %s datafile\n", argv[0]) ;
        exit(0) ;
    }

    readData(argv[1], &gList) ;
    // showList(gList) ;

    array2D = makeSound(gList, &totalDur) ;
    array1D = interleave(array2D, totalDur) ;

    setChannelCount(nchan) ;

    saveSoundfile("grainTest.wav", array1D, totalDur) ;

    free(array1D) ;

    for ( i = 0 ; i < nchan ; i++ )
        free(array2D[i]) ;
    free(array2D) ;

    for ( g = gList ; g != NULL ; g = g->next )
        free(g) ;

	return 0 ;
}
