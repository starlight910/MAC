#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cm.h>
#include <pitches.h>

void mixSound(double *out, int start, int samples, double *in)
{
		out += start ;

		while ( samples-- ) 
		{
				*out = *out + *in ;
				out++ ;
				in++ ;
		}
		return ;
}

void StereoMix(double *out, int start, int frames, double *in)
{
		int nchan = 2;
		
		out += start * nchan ;

		while(frames--)
		{	
			*out++ += *in++;
			*out++ += *in++;
		}
		return;
}

double *makePanList(int nTones)
{
		double panList[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
		double *p, *plist ;
		int index ;

		plist = (double *)calloc(nTones, sizeof(double)) ;

		for ( p = plist ; p < plist + nTones ; p++ ) 
		{
			index = irand() % 11 ;
			*p = panList[ index ] ;
		}

		return plist ;
}
/*
double *fmIShift(double C, double M, double dur, double dB)
{
	double cInc, mInc, cphase, mphase, A ;
	double *out, *po ;
	int samples ;
	double I, I2, iShift ;
	double twopi = 2 * M_PI ;

	cInc = ( twopi * C ) / SAMPLING_RATE ;
	mInc = ( twopi * M ) / SAMPLING_RATE ;
	cphase = mphase = 0 ;

	A = pow(10.0, dB/20.0) / 2 ;

	samples = dur * SAMPLING_RATE ;

	po = out = (double *)calloc(samples, sizeof(double)) ;

	I = 2 ;				// initial value of I
	I2 = 7 ;			// final value of I
	iShift = (I2 - I) / ( samples - 1 ) ;	// amount of increment

	while ( samples-- ) 
	{
		*po++ = A * sin(cphase + I * sin(mphase)) ;
		I += iShift ;	// change the value of I

		cphase += cInc ;
		mphase += mInc ;

		if ( cphase >= twopi )
			cphase -= twopi ;

		if ( mphase >= twopi )
			mphase -= twopi ;
	}

	return out ;
}

double *makeFList(int nTones, double F)
{
	// list of intervals in cents
	double clist[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10} ;
	double K = 1200 / log10(2.0) ;
	double *flist ;
	int i ;
					
	flist = (double *)calloc(nTones,
	sizeof(double)) ;
								
	flist[0] = F ;		// first frequency
												
	for ( i = 1 ; i < nTones ; i++ )	//subsequent frequencies
		flist[i] = flist[i-1] *	pow(10.0, clist[i%11]/K) ;
						
		return flist ;
}

*/

double *fmParallel(double carrier, double mod1, double I1, double mod2, double I2, double dur, double dB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.1, 0.5, 0.8, 1.0} ;
	double P[] = {0.0, 1.0, 0.75, 0.50, 0.0} ;
	int N = 5;

	double T1[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double P1[] = {1.0, 0.8, 0.6, 0.4, 1.0, 0.6, 0.4, 1.0, 0.4, 0.2, 0.0} ;
	int N1 = 11;

	double T2[] = {0.0, 0.1, 0.5, 1.0} ;
	double P2[] = {0.0, 1.0, 0.6, 0.0} ;
	int N2 = 4;

	double *env1, *env2, *e1, *e2 ;

//	printf("\tfmParallel[%03d]: C: %g M1: %g M2: %g I1: %g I2: %g\n", counter++, carrier, mod1, mod2, I1, I2) ;

	cInc = (twopi * carrier) / SAMPLING_RATE ;
	m1Inc = (twopi * mod1) / SAMPLING_RATE ;
	m2Inc = (twopi * mod2) / SAMPLING_RATE ;

	C = M1 = M2 = 0 ;

	amp = pow(10.0, dB/20.0)/2 ;

	samples = dur * SAMPLING_RATE ;

	sound = (double *)calloc(samples, sizeof(double)) ;

	env1 = (double *)calloc(samples, sizeof(double)) ;
	env2 = (double *)calloc(samples, sizeof(double)) ;

	linearEnv(env1, samples, T1, P1, N1) ;
	linearEnv(env2, samples, T2, P2, N2) ;

	rescale(env1, samples, 0.0, I1) ;
	rescale(env2, samples, 0.0, I2) ;

	e1 = env1 ;
	e2 = env2 ;

	for ( s = sound ; s < sound + samples ; s++ ) 
	{
		*s = amp * sin(C + *e1++ * sin(M1) + *e2++ * sin(M2)) ;
		C += cInc ;
		M1 += m1Inc ;
		M2 += m2Inc ;

		if( C >= twopi ) C -= twopi ;
		if( M1 >= twopi ) M1 -= twopi ;
		if( M2 >= twopi ) M2 -= twopi ;
																										}
	free(env1) ;
	free(env2) ;

	envelope(sound, dur, T, P, N) ;

	return sound ;
}

double *fmCascade(double carrier, double mod1, double I1, double mod2, double I2, double dur, double dB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0} ;
	double A[] = {0.0, 0.4, 1.0, 0.8, 0.2, 0.0} ;
	int N = 6 ;

	cInc = ( twopi * carrier ) / SAMPLING_RATE ;
	m1Inc = ( twopi * mod1 ) / SAMPLING_RATE ;
	m2Inc = (twopi * mod2 ) / SAMPLING_RATE ;

	C = M1 = M2 = 0 ;

	amp = pow(10.0, dB/20.0)/2 ;

	samples = dur * SAMPLING_RATE ;

	sound = (double *)calloc(samples, sizeof(double)) ;

	for ( s = sound ; s < sound + samples ; s++ ) 
	{
		*s = amp * sin(C + I1 * sin(M1 + I2 * sin(M2))) ;
		C += cInc ;
		M1 += m1Inc ;
		M2 += m2Inc ;

		if( C >= twopi ) C -= twopi ;
		if( M1 >= twopi ) M1 -= twopi ;
		if( M2 >= twopi ) M2 -= twopi ;
	}

	envelope(sound, dur, T, A, N) ;

	return sound ;
}

double *makePan(double *mono, double dur, double panLocation)
{
	int nchan, frames ;
	double *stereo, *s, L, R, boost ;

	nchan = 2 ;
	frames = dur * SAMPLING_RATE ;
	stereo = (double *)calloc(frames * nchan, sizeof(double)) ;

	L = panLocation ;
	R = 1 - L ;
	boost = 1.0 / sqrt(L*L+R*R) ;
	L *= boost ;
	R *= boost ;

	s = stereo ;
	while ( frames-- ) {
		*s++ = L * *mono ;
		*s++ = R * *mono++ ;
	}

	return stereo ;
}

double *part1A(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 0.1 ;
	double pd2 = 0.3 ;
	double *stereo ;

	double cflist[] = {55, 65.41, 73.42, 82.41, 98, 103.83, 
					   55, 65.41, 73.42, 82.41, 98, 103.83} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {3, 2, 1, 3, 2, 1, 3, 2, 1, 3, 2 ,1} ;

	double db = 144 ;
	
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, 
			sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) {

		input = fmParallel(cflist[i], cflist[i], pd1, 
				(cflist[i] * 3), pd2, durlist[i], db) ;

		stereo = makePan(input, durlist[i], panList[i]) ;

		samples = durlist[i] * SAMPLING_RATE * nchan ;

		mixSound(output, start, samples, stereo) ;
		
		start += samples ;
		
		free(input) ;
		free(stereo) ;
	}

	return output ;
}

double *part1B(double *totaldur)
{
     double *output, *input, *panList ;
     int i, samples, start ;
     double pd1 = 5 ;
     double pd2 = 13 ;
	 double *stereo ;

     double cflist[] = {220, 220, 261.63, 261.63, 261.63, 293.66, 293.66, 293.66, 329.23, 
						329.23, 196, 196, 196, 207.63, 207.63, 207.63,
					    220, 220, 261.63, 261.63, 261.63, 293.66, 293.66, 293.66, 329.23, 
						329.23, 196, 196, 196, 207.63, 207.63, 207.63} ;
     int nTones = 32;
	 int nchan = 2 ;

     double durlist[] = {0.9, 0.1, 1.8, 0.1, 0.1, 2.8, 0.1, 0.1, 0.9, 0.1, 1.8, 0.1, 0.1, 2.8, 0.1, 0.1} ;

     double db = 133 ;

	 panList = makePanList(nTones) ;

     for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i%16] ;

     output = (double *)calloc(
			 *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

     for ( i = 0, start = 0 ; i < nTones ; i++ )
     {
	     input = fmCascade(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i%16], db) ;

		 stereo = makePan(input, durlist[i%16], panList[i]) ;

         samples = durlist[i%16] * SAMPLING_RATE * nchan ;
         mixSound(output, start, samples, stereo) ;
         start += samples ;
         free(input) ;
         free(stereo) ;
     }

     return output ;
}

double * part1C(double *totaldur)
{
      double *output, *input, *panList ;
      int i, samples, start ;
      double pd1 = 11 ;
      double pd2 = 11 ;
	  double *stereo ;

      double cflist[] = {440, 523.25, 587.33, 659.25, 392, 415.30,
						 440, 523.25, 587.33, 659.25, 392, 415.30} ;
      int nTones = 12;
	  int nchan = 2 ;

      double durlist[] = {2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1} ;

      double db = 133 ;

	  panList = makePanList(nTones) ;

      for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
      *totaldur += durlist[i] ;

      output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

      for ( i = 0, start = 0 ; i < nTones ; i++ )
      {
          input = fmCascade(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
          
		  stereo = makePan(input, durlist[i], db) ;

		  samples = durlist[i] * SAMPLING_RATE * nchan ;

          mixSound(output, start, samples, stereo) ;
          start += samples ;

          free(input) ;
		  free(stereo) ;
      }

      return output ;
}
//5th
double *part2A(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 0.1 ;
	double pd2 = 0.1 ;
	double *stereo ;

	double cflist[] = {46.25, 55, 61.74, 69.30, 82.41, 87.31,
					   46.25, 55, 61.74, 69.30, 82.41, 87.31} ;
	int nTones = 12 ;
	int nchan = 2 ;

	double durlist[] = {3, 2, 1, 3, 2, 1} ;

	double db = 144 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i%6] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE* nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i%6], db) ;
		stereo = makePan(input, durlist[i%6], panList[i]) ;
		samples = durlist[i%6] * SAMPLING_RATE * nchan;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
																							
		free(input) ;
		free(stereo) ;
	}

	return output ;
}

double *part2B(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 4 ;
	double pd2 = 17 ;
	double *stereo ;

	double cflist[] = {220, 261.63, 293.66, 329.23, 329.23, 329.23, 329.23, 329.23, 329.23, 
					   329.23, 329.23, 329.23, 329.23, 329.23, 196, 207.63,
					   220, 261.63, 293.66, 329.23, 329.23, 329.23, 329.23, 329.23, 329.23,
					   329.23, 329.23, 329.23, 329.23, 329.23, 196, 207.63} ;
	int nTones = 32 ;
	int nchan = 2 ;

	double durlist[] = {3, 2, 1, 2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 2, 1,
					    3, 2, 1, 2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 2, 1} ;

	double db = 133 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE *nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
																							
		free(input) ;
		free(stereo) ;
	}

	return output ;
}
//7th
double *part2C(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 11 ;
	double pd2 = 0.5 ;
	double *stereo ;

	double cflist[] = {329.63, 392, 440, 493.88, 587.33, 622.25,
					   329.63, 392, 440, 493.88, 587.33, 622.25} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {1, 2, 3, 1, 2, 3} ;

	double db = 126 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i%6] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i%6], db) ;
		stereo = makePan(input, durlist[i%6], panList[i]) ;
		samples = durlist[i%6] * SAMPLING_RATE * nchan;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
																							
		free(input) ;
		free(stereo) ;
	}

	return output ;
}

double *part3A(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 3 ;
	double pd2 = 0.01 ;
	double *stereo ;

	double cflist[] = {65.41, 65.41, 65.41, 65.41, 65.41, 65.41,
					   77.78, 77.78, 77.78, 77.78,
					   87.31, 87.31,
					   98, 98, 98, 98, 98, 98,
					   58.27, 58.27, 58.27, 58.27,
					   61.74, 61.74,
					   82.41, 82.41, 82.41, 82.41, 82.41, 82.41,
					   98, 98, 98, 98,
					   110, 110,
					   123.47, 123.47, 123.47, 123.47, 123.47, 123.47,
					   146.83, 146.83, 146.83, 146.84,
					   155.56, 155.56} ;
	int nTones = 48;
	int nchan = 2 ;

	double durlist[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5 ,0.5, 0.5, 0.5,
						0.5, 0.5,
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5,
						0.5, 0.5,
						0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
						0.5, 0.5, 0.5, 0.5,
						0.5, 0.5} ;

	double db = 144 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
																							
		free(input) ;
		free(stereo) ;
	}   

	return output ;
}

double *part3B(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 5 ;
	double pd2 = 13 ;
	double *stereo ;

	double cflist[] = {220, 261.63, 293.66, 329.23, 196, 207.63,
					   220, 261.63, 293.66, 329.23, 196, 207.63} ;
	int nTones = 12;
	int nchan = 2;

	double durlist[] = {3, 2, 1, 3, 2, 1, 3, 2, 1, 3, 2, 1} ;

	double db = 133 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
	
		free(input) ;
		free(stereo) ;
	}
		
	return output ;
}


double *part3C(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 7 ;
	double pd2 = 22 ;
	double *stereo ;

	double cflist[] = {369.99, 369.99, 
					   440, 
					   493.88, 
					   554.37, 554.37, 
					   329.63, 
					   349.23,
					   261.63, 261.63, 
					   311.13, 
					   349.23, 
					   392, 392,
					   466.16, 
					   493.88} ;
	int nTones = 16 ;
	int nchan = 2 ;

	double durlist[] = {1, 1, 
						3, 
						1, 
						1, 1, 
						3, 
						1, 
						1, 1,
						3, 
						1, 
						1, 1, 
						3, 
						1} ;

	double db = 133 ;
										
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan ;
		mixSound(output, start, samples, stereo) ;
		start += samples ;
																							
		free(input) ;
		free(stereo) ;
	}

	return output ;
}

double *part4A(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 0.05 ;
	double pd2 = 0.05 ;
	double *stereo ;

	double cflist[] = {92.50, 110, 123.47, 138.59, 82.41, 87.31,
					   110, 130.81, 146.83, 164.81, 98, 103.83} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {3, 2, 1, 3, 2, 1, 3, 2, 1, 3, 2, 4} ;
	double db = 144 ;
																					
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
		{
			input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
			stereo = makePan(input, durlist[i], panList[i]) ;
			samples = durlist[i] * SAMPLING_RATE * nchan;
			mixSound(output, start, samples, stereo) ;
			start += samples ;
		
			free(input) ;
			free(stereo) ;
		}				
			return output ;
}

double *part4B(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 4 ;
	double pd2 = 1 ;
	double *stereo ;

	double cflist[] = {220, 261.63, 293.66, 329.23, 196, 207.63,
					   220, 261.63, 293.66, 329.23, 196, 207.63} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {3, 2, 1, 3, 2, 1, 3, 2, 1, 3, 2, 4} ;

	double db = 133 ;
																					
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
		{
			input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
			stereo = makePan(input, durlist[i], panList[i]) ;
			samples = durlist[i] * SAMPLING_RATE * nchan;
			mixSound(output, start, samples, stereo) ;
			
			start += samples ;
			
			free(input) ;
			free(stereo) ;
		}

		return output ;
}

double *part4C(double *totaldur)
{
	double *output, *input, *panList ;
	int i, samples, start ;
	double pd1 = 5 ;
	double pd2 = 13 ;
	double *stereo ;

	double cflist[] = {261.63, 311.13, 349.23, 392, 233.8, 246.94,
					   220, 261.63, 293.66, 329.23, 196, 207.63} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 4} ;

	double db = 133 ;
																					
	panList = makePanList(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc(*totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ )
		{
			input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		
			stereo = makePan(input, durlist[i], panList[i]) ;
			samples = durlist[i] * SAMPLING_RATE * nchan;
			mixSound(output, start, samples, stereo) ;
		
			start += samples ;
			
			free(input) ;
			free(stereo) ;
		}
		
	return output ;
}


int main(void)
{
	double *p1a, *p1b, *p1c, *p2a, *p2b, *p2c, *p3a, *p3b, *p3c, *p4a, *p4b, *p4c, *final ;
	double dur1, dur2, dur3, dur4, totalDur ;
	int nframesA, nframesB, nframesC, nframesD, startframe, totalframes ;

	setChannelCount(2) ;
	setbuf(stdout, NULL) ;

	p1a = part1A(&dur1) ; printf("After part1A: %f\n", dur1) ;
	p1b = part1B(&dur1) ; printf("After part1B: %f\n", dur1) ;
	p1c = part1C(&dur1) ; printf("After part1C: %f\n", dur1) ;

	p2a = part2A(&dur2) ; printf("After part2A: %f\n", dur2) ;
	p2b = part2B(&dur2) ; printf("After part2B: %f\n", dur2) ;
	p2c = part2C(&dur2) ; printf("After part2C: %f\n", dur2) ;

	p3a = part3A(&dur3) ; printf("After part3A: %f\n", dur3) ;
	p3b = part3B(&dur3) ; printf("After part3B: %f\n", dur3) ;
	p3c = part3C(&dur3) ; printf("After part3C: %f\n", dur3) ;
	
	p4a = part4A(&dur4) ; printf("After part4A: %f\n", dur4) ;
	p4b = part4B(&dur4) ; printf("After part4B: %f\n", dur4) ;
	p4c = part4C(&dur4) ; printf("After part4C: %f\n", dur4) ;


	totalDur = dur1 + dur2 + dur3 + dur4 ;
	totalframes = totalDur * SAMPLING_RATE ;

	final = (double *)calloc(totalframes * 2, sizeof(double)) ;

	nframesA = dur1 * SAMPLING_RATE ;
	nframesB = dur2 * SAMPLING_RATE ;
	nframesC = dur3 * SAMPLING_RATE ;
	nframesD = dur4 * SAMPLING_RATE ;

	startframe = 0 ;

	printf("\tPart1") ;
	StereoMix(final, startframe, nframesA, p1a) ;
	printf(".") ;
	StereoMix(final, startframe, nframesA, p1b) ;
	printf(".") ;
	StereoMix(final, startframe, nframesA, p1c) ;
	printf(".") ;
	startframe += nframesA ;

	printf("\tDone\n") ;


	printf("\tPart2") ;
	StereoMix(final, startframe, nframesB, p2a) ;
	printf(".") ;
	StereoMix(final, startframe, nframesB, p2b) ;
	printf(".") ;
	StereoMix(final, startframe, nframesB, p2c) ;
	printf(".") ;
	startframe += nframesB ;

	printf("\tDone\n") ;


	printf("\tPart3") ;
	StereoMix(final, startframe, nframesC, p3a) ;
	printf(".") ;
	StereoMix(final, startframe, nframesC, p3b) ;
	printf(".") ;
	StereoMix(final, startframe, nframesC, p3c) ;
	printf(".") ;
	startframe += nframesC ;

	printf("\tDone\n") ;


	printf("\tPart4") ;
	StereoMix(final, startframe, nframesD, p4a) ;
	printf(".") ;
	StereoMix(final, startframe, nframesD, p4b) ;
	printf(".") ;
	StereoMix(final, startframe, nframesD, p4c) ;
	printf(".") ;

	printf("\tDone\n") ;

	saveSoundfile("Project2.wav", final, totalDur) ;

	free(p1a) ;
	free(p1b) ;
	free(p1c) ;
	free(p2a) ;
	free(p2b) ;
	free(p2c) ;
	free(p3a) ;
	free(p3b) ;
	free(p3c) ;
	free(p4a) ;
	free(p4b) ;
	free(p4c) ;
	free(final) ;

	return 0 ;
}

