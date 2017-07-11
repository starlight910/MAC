#include <stdio.h>
#include <stdlib.h>
#include <cm.h>

void mixSound(double *out, int start, int samples, double *in)
{
	out = out + start ;      //move *out to the starting memory address

	while ( samples-- )
	{
		*out = *out + *in ;
		out++ ;
		in++ ;
	}

	return ;
}

int main(void)
{
	int nTones = 5 ;
	double flist1[] = {329.63, 261.63, 523.25, 659.25, 41.20} ; //freq tune 440Hz    
	double flist2[] = {440, 493.88, 523.25, 587.33, 659.25} ;
	double flist3[] = {110, 55, 55, 55, 22.5} ;
	double flist4[] = {332.62, 264.00, 528.01, 665.25, 41.58} ; // freq tune 444Hz
	double flist5[] = {323.63, 256.87, 513.74, 647.27, 40.45} ; //freq tune 432Hz
	double dblist1[] = {28.8, 57.6, 86.4, 129.6, 144} ;
	double dblist2[] = {86.4, 144, 115.2, 100.8, 129.6} ;
	double dblist3[] = {28.8, 129.6, 129.6, 129.6, 129.6} ;
	double tlist1[] = {0.0, 0.2, 0.3, 0.7, 1.0} ;
	double tlist2[] = {0.0, 0.1, 0.3, 0.5, 1.0} ;
	double tlist3[] = {0.0, 0.26, 0.39, 0.78, 1.0} ;
//0.1 =3sec
	double *seq1, *seq2, *seq3, *seq4, *seq5, *final ;

//	double pfreq[] = {1.1, 1.2, 1.3, 1.4, 1.5} ; // eh?
//	double pamp[] = {0.9, 0.8, 0.7, 0.6, 0.5} ;  // eh?
//	int nHarmonics = 5 ;                        // eh?

	double dur = 30 ;
	int start ;
	int samples ;

	seq1 = TrianglePath(flist2, dblist1, tlist2, nTones, dur) ;
	seq2 = SawtoothPath(flist3, dblist3, tlist2, nTones, dur) ;
	seq3 = SquarePath(flist1, dblist2, tlist1, nTones, dur) ;
	seq4 = TrianglePath(flist4, dblist2, tlist3, nTones, dur) ;
	seq5 = SquarePath(flist5, dblist2, tlist3, nTones, dur) ;

	samples =dur * SAMPLING_RATE ;
	final = (double*)calloc(samples, sizeof(double)) ;

	start = 0 ;
	mixSound(final, start, samples, seq1) ;
	mixSound(final, start, samples, seq2) ;
	mixSound(final, start, samples, seq3) ;
	mixSound(final, start, samples, seq4) ;
	mixSound(final, start, samples, seq5) ;

	saveSoundfile("eh.wav", final, dur) ;

	free(seq1) ;
	free(seq2) ;
	free(seq3) ;
	free(seq4) ;
	free(seq5) ;
	free(final) ;

	return 0 ;
}
