#ifndef __FFTLIST_H__
#define __FFTLIST_H__
#include <complex.h>

typedef struct FFT {
    int index ;
    int fftSize ;    // length of each array = fftSize / 2
    int srate ;
    double *Real ;
    double *Imag ;
    double complex *cList ;
    struct FFT *next ;
} FFT ;

enum windowType {HAMMING,HANNING,BARTLETT,BLACKMAN,BLACKMAN_HARRIS};

extern int FFT_ANALYZE, FFT_SYNTHESIZE ;
extern void fft(double *x, int N, int forward) ;
extern FFT *fftListCreate(double *data, double dur, int fftSize) ;
extern double *fftListCombine(FFT *list1, FFT *list2, double *dur) ;
extern void fftListFree(FFT *list) ;
extern FFT *fftListCreateWindow(double *data, double dur, int fftSize,
        int windowType) ;
extern void FFTaddToList(FFT *new, FFT **list) ;
extern FFT *FFTcreateLink(int index, int fftSize) ;
extern void fftNormalize(double *data, double dur, double *xmin, double *xmax);
extern void fftRescale(double *data, double dur, double x1, double x2);
extern void fftMakeWindow(double **window, const int len, const int type) ;

#endif


