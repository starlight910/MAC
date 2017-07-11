#include <stdio.h>
#include <stdlib.h>
#include <cm.h>

int main(void)
{
	double f2 = 110, f1 = 880;
	double dB = 144 ;
	double dur = 10 ;
	double *tone ;
	int nPartials = 1000 ;
	double T[] = {0.0, 0.1, 0.2, 0.5, 0.7, 0.9, 1.0} ;
	double A[] = {0.0, 1.0, 0.1, 0.3, 0.5, 1.0, 0.0} ;
	int N = 7 ;

//	tone = SawtoothGliss(f2, f1, dB, dur, nPartials) ;
//	envelope(tone, dur, T, A, N) ;
//	saveSoundfile("test.wav", tone, dur) ;
//	free(tone) ;

	tone = TriangleGliss(f1, f2, dB, dur, nPartials) ;
	envelope(tone, dur, T, A, N) ;
	playbuffer(tone, dur) ;
	saveSoundfile("test1.wave",tone, dur);
	free(tone);
//
//	tone = SquareGliss(f2, f1, dB, dur, nPartials) ;
//	envelope(tone, dur, T, A, N) ;
//	playbuffer(tone, dur) ;
//	free(tone);
//
//  tone = SineGliss(f1, f2, dB, dur) ;
//	envelope(tone, dur, T, A, N) ;
//	playbuffer(tone, dur) ;
//	free(tone);


	return 0 ;
}
