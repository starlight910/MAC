#include <stdlib.h>
#include <stdio.h>
#include <cm.h>
#include <math.h>
#include <time.h>

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
	while ( frames-- ) 
	{
		*s++ = L * *mono ;
		*s++ = R * *mono++ ;				
	}
	return stereo ;
}

// put a mono sound into a stereo location
double *monoToStereoPan(double *monoIn, int nFrames, double pan)
{
	double L, R, boost ;
	double *stereoOut, *S ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan > 1.0 || pan < 0.0 ) 
	{	// make sure values are within range
		printf("\tmonoToStereoPan[%03d]: bad value for pan: %f\n", counter, pan);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}

	L = pan ;
	R = L - 1 ;
	boost = 1.0 / sqrt( L*L + R*R ) ;
	L *= boost ;
	R *= boost ;
		
	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	while ( nFrames-- ) 
	{
		*S++ = L * *monoIn ;
		*S++ = R * *monoIn++ ;
	}						
	return stereoOut ;
}

// Put a mono sound in a stereo outpit and shift
double *monoToStereoPanShift(double *monoIn, int nFrames, double pan1, double pan2)
{
	double L, R, boost, pan, panInc ;
	double *S, *stereoOut ;
	int nchan = 2 ;
	static int counter ;

	counter++ ;

	if ( pan1 > 1.0 || pan1 < 0.0 || pan2 > 1.0 || pan2 < 0.0 ) 
	{
		printf("\tmonoToStereoPanShift[%03d]: bad value for pan: %f %f\n", counter, pan1, pan2);
		printf("\tExiting\n") ;
		exit(EXIT_FAILURE) ;
	}
	S = stereoOut = (double *)calloc(nFrames * nchan, sizeof(double)) ;

	panInc = (pan2 - pan1) / ( nFrames - 1 ) ;
	pan = pan1 ;

	while ( nFrames-- ) 
	{
		L = pan ;
		R = L - 1 ;
		boost = 1.0 / sqrt( L*L + R*R ) ;
		L *= boost ;
		R *= boost ;

		*S++ = L * *monoIn ;
		*S++ = R * *monoIn++ ;
		
		pan += panInc ;
	}	
	return stereoOut ;
}


double *makeFList(int nTones, double F, double *clist)
{
	double K = 1200 / log10(2.0) ;
	double *flist ;
	int i ;
	
	flist = (double *)calloc(nTones, sizeof(double)) ;
		
	flist[0] = F ;		// first frequency

	for ( i = 1 ; i < nTones ; i++ )	//subsequent frequencies
		flist[i] = flist[i-1] * pow(10.0, clist[i]/K) ;

	return flist ;
}

//Envelope-----------
void makeEnvelope(double *data, double dur)
{
	double T[] = {0.0, 0.1, 0.25, 0.75, 1.0} ;
	double A[] = {0.0, 1.0, 0.75, 0.5, 0.0} ;
	int N = 5 ;

	envelope(data, dur, T, A, N) ;

	return ;
}

void makeEnvelope2(double *data, double dur)
{
	double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double A[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0} ;
	int N = 11 ;

	envelope(data, dur, T, A, N) ;

	return ;
}


//Panning--------------
//Random pan
double *RandomPan(int nTones)
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
//Left to right pan
double *PanLR(double *mono, int frames)
{
	double *stereo, *S, *M ;
	double Left, Right, boost, panIncrement;
	int nchan = 2;

	stereo = (double *)calloc(frames * nchan, sizeof(double));

	Left = 1;
	panIncrement = 1.0 / (frames - 1);

	S = stereo;
	M = mono;

	while(frames--) 
	{
		Right = 1.0 - Left;
		boost = 1.0 / sqrt(Left * Left + Right * Right);

		*S++ = (Left * boost ) * *M;											
		*S++ = (Right * boost ) * *M++;
		Left -= panIncrement;	
	}						
	return stereo;
}

//Right to Left pan
double *PanRL(double *mono, int frames)
{
	double *stereo, *S, *M ;
	double Left, Right, boost, panIncrement;
	int  nchan = 2;

	stereo = (double *)calloc(frames * nchan, sizeof(double));

	Left = 0;
	panIncrement = 1.0 / (frames - 1);

	S = stereo;
	M = mono;

	while(frames--) 
	{
		Right = 1.0 - Left;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
						
		*S++ = (Right * boost ) * *M;
		*S++ = (Left * boost ) * *M++;							
		Left += panIncrement;
	}						
	return stereo;
}


//FM-------------------
//FM
double *FM(double cFreq, double mFreq, double dur, double dB, double pd)
{
	double cPhase, mPhase, cInc, mInc, A, I ;
	double *tone, *p;
	int samples;

	samples = dur * SAMPLING_RATE;

	A = pow(10.0, dB/20) / 2;
	
	cInc = (2 * M_PI * cFreq) / SAMPLING_RATE;
	mInc = (2 * M_PI * mFreq) / SAMPLING_RATE;
	
	tone = (double *)calloc(samples, sizeof(double));
									
	cPhase = 0;
	mPhase = 0;

	I = pd/mFreq ;

	for ( p = tone; p < tone +samples; p++ ) 
	{
		*p = A * sin(cPhase + I  *sin(mPhase));
		cPhase += cInc;
		mPhase += mInc;
		
		if(cPhase >= 2 * M_PI) cPhase -= 2 * M_PI ;
		if(mPhase >= 2 * M_PI) mPhase -= 2 * M_PI ;
	}
	return tone ;
}

//Cascade
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

//Parallel
double *fmParallel(double carrier, double mod1, double I1, double mod2, double I2, double dur, double dB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.1, 0.5, 0.8, 1.0} ;
	double P[] = {0.0, 1.0, 0.75, 0.5, 0.0} ;
	int N = 5;

	double T1[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double P1[] = {1.0, 0.8, 0.6, 0.4, 1.0, 0.6, 0.4, 1.0, 0.4, 0.2, 0.0} ;
	int N1 = 11;

	double T2[] = {0.0, 0.5, 0.8, 1.0} ;
	double P2[] = {0.0, 1.0, 0.6, 0.0} ;
	int N2 = 4;
	double *env1, *env2, *e1, *e2 ;

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

//tones
/*
double *gap(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 0.1 ;
	double pd2 = 0.3 ;
	double *stereo ;

	double cflist[] = {0} ;
	int nTones = 1;
	int nchan = 2 ;

	double durlist[] = {2} ;

	double db = 144 ;
															
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
	*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) 
	{										
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
			
		stereo = makePan(input, durlist[i], panList[i]) ;
		
		samples = durlist[i] * SAMPLING_RATE * nchan ;
																										
		mixSound(output, start, samples, stereo) ;
																											
		start +=samples ;

		free(input) ;									
		free(stereo) ;
		
	}					
	return output ;
}
*/

double *part1A(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 0.1 ;
	double pd2 = 0.3 ;
	double *stereo ;

	double cflist[] = {0, 103.83, 103.83, 0, 103.83, 103.83, 
					   0, 103.83, 103.83, 0, 103.83, 103.83,
					   0, 103.83, 103.83, 0, 103.83, 103.83, 0} ;
	int nTones = 19;
	int nchan = 2 ;

	double durlist[] = {5, 0.1, 0.1, 3, 0.1, 0.1, 
					    3, 0.1, 0.1, 2, 0.1, 0.1,
						2, 0.1, 0.1, 1, 0.1, 0.1, 1.8} ;

	double db = 144 ;
				
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) 
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan ;
	
		mixSound(output, start, samples, stereo) ;
	
		start +=samples ;

		free(input) ;
		free(stereo) ;
	}	
	return output ;
}

double *part2A(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 5 ;
	double pd2 = 0.3 ;
	double *stereo ;

	double cflist[] = {55, 65.41, 73.42, 82.41} ;
	int nTones = 4;
	int nchan = 2 ;

	double durlist[] = {10, 5, 3, 5} ;
	double db = 138 ;
												
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
	*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

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

double *part3A(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 0.1 ;
	double pd2 = 0.1 ;
	double *stereo ;

	double cflist[] = {55, 110, 55, 110, 55, 110, 55, 110} ;
	int nTones = 8;
	int nchan = 2 ;

	double durlist[] = {3, 3, 3, 3, 2, 2, 2, 2} ;
	double db = 138 ;
																							
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )

		*totaldur += durlist[i] ;
	
	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

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

double *part3B(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 1 ;
	double pd2 = 11 ;
	double *stereo ;

	double cflist[] = {220, 440, 440, 110, 440, 440, 220, 440, 440, 110, 440, 440, 
					   220, 880, 440, 110, 880, 440, 220, 880, 440, 110, 880, 440} ;
	int nTones = 24;
	int nchan = 2 ;

	double durlist[] = {2, 0.5, 0.5, 2, 0.5, 0.5, 2, 0.5, 0.5, 2, 0.5, 0.5,
						1, 0.5, 0.5, 1, 0.5, 0.5, 1, 0.5, 0.5, 1, 0.5, 0.5} ;
	double db = 138 ;
																							
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
	*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) 
	{										
		input = fmCascade(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;
		
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
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 3 ;
	double pd2 = 0.3 ;
	double *stereo ;

	double cflist[] = {103.83, 110, 0, 220, 220, 0, 103.83, 55, 0, 78, 55, 0} ;
	int nTones = 12;
	int nchan = 2 ;

	double durlist[] = {3, 4, 2, 4, 3, 1, 2, 2, 4, 5, 15, 10} ;

	double db = 138 ;
															
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) 
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;																				
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan ;		
		mixSound(output, start, samples, stereo) ;
						
		start +=samples ;
		free(input) ;				
		free(stereo) ;
		
	}					
	return output ;
}

double *part4B(double *totaldur)
{
	double *output, *input , *panList ;
	int i, samples, start ;
	double pd1 = 1 ;
	double pd2 = 3 ;
	double *stereo ;

	double cflist[] = {55, 110, 78, 220, 78, 55, 55, 0} ;
	int nTones = 8;
	int nchan = 2 ;

	double durlist[] = {7, 4, 2, 4, 3, 8, 17, 10} ;

	double db = 138 ;
																										
	panList = RandomPan(nTones) ;

	for ( i = 0, *totaldur = 0 ; i < nTones ; i++ )
		*totaldur += durlist[i] ;

	output = (double *)calloc( *totaldur * SAMPLING_RATE * nchan, sizeof(double)) ;

	for ( i = 0, start = 0 ; i < nTones ; i++ ) 
	{
		input = fmParallel(cflist[i], cflist[i], pd1, (cflist[i] * 3), pd2, durlist[i], db) ;																				
		stereo = makePan(input, durlist[i], panList[i]) ;
		samples = durlist[i] * SAMPLING_RATE * nchan ;		
		mixSound(output, start, samples, stereo) ;
		
		start +=samples ;
		free(input) ;							
		free(stereo) ;
								
	}							
	return output ;
}



/*
double *readwav(double *totalDur)
{
	char *wfile
*/
int main(void)
{
	double *p1a, *p2a, *p3a, *p3b, *p4a, *p4b ;
//	double *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8, *v9, *v10, *v11, *v12, *v13, *v14 ;
	double *voice ;
	double *final ;
	double dur1, dur2, dur3, dur4, totalDur ;
	double durA ;
//	double durB ;
//	double durC ;
//	double durD ;
	int nframesX, nframesA, nframesB, nframesC, nframesD, startframe, totalframes ;
//	int nframes1, nframes2, nframes3, nframes4 ;

	setChannelCount(1) ;
//	setbuf(stdout, NULL) ;

	p1a = part1A(&dur1) ; printf("Part1A: %f\n", dur1) ;
	p2a = part2A(&dur2) ; printf("Part2A: %f\n", dur2) ;
	p3a = part3A(&dur3) ; printf("Part3A: %f\n", dur3) ;
	p3b = part3B(&dur3) ; printf("Part3B: %f\n", dur3) ;
	p4a = part4A(&dur4) ; printf("Part4A: %f\n", dur4) ;
	p4b = part4B(&dur4) ; printf("Part4B: %f\n", dur4) ;
//	g = gap(&dur0) ; printf("Gap: %f\n", dur0) ;
	
	voice = readSoundfile("allmix11.wav", &durA) ;
	printf("Audio: %f\n", durA) ;
/*
	v1 = readSoundfile("1Your.wav", &durC) ; printf("Voice1: %f\n", durC) ;
	v2 = readSoundfile("2voice.wav", &durA) ; printf("Voice2: %f\n", durA) ;
	v3 = readSoundfile("3my.wav", &durD) ;
	v4 = readSoundfile("4friend.wav", &durB) ;
	v5 = readSoundfile("5wonders.wav", &durB) ;
	v6 = readSoundfile("6in.wav", &durB) ;
	v7 = readSoundfile("7my.wav", &durB) ;
	v8 = readSoundfile("8heart.wav", &durB) ;
	v9 = readSoundfile("9like.wav", &durB) ;
	v10 = readSoundfile("10the.wav", &durB) ;
	v11 = readSoundfile("11muffled.wav", &durB) ;
	v12 = readSoundfile("12sound.wav", &durB) ;
	v13 = readSoundfile("13of.wav", &durB) ;
	v14 = readSoundfile("14the.wav", &durB) ;
*/
	

//	totalDur = dur1 + dur2 + dur3 + dur4;
	totalDur = durA ;
	totalframes = totalDur * SAMPLING_RATE ;
				
	final = (double *)calloc(totalframes * 2, sizeof(double)) ;

	nframesA = dur1 * SAMPLING_RATE ;
	nframesB = dur2 * SAMPLING_RATE ;
	nframesC = dur3 * SAMPLING_RATE ;
	nframesD = dur4 * SAMPLING_RATE ;
	nframesX = durA * SAMPLING_RATE ;

//	nframes1 = durA * SAMPLING_RATE ;
//	nframes2 = durB * SAMPLING_RATE ;
//	nframes3 = durC * SAMPLING_RATE ;
//	nframes4 = durD * SAMPLING_RATE ;
	startframe = 0 ;

	printf("\tPart1") ;
	StereoMix(final, startframe, nframesX, voice) ;
	StereoMix(final, startframe, nframesA, p1a) ;
	startframe += nframesA ;
	//StereoMix(final, startframe, nframes1, v1) ;
	//startframe = nframes1 ;
	//StereoMix(final, startframe, nframes2, v2) ;
	//startframe = nframes2 ;
	//StereoMix(final, startframe, nframesX, g) ;
	//startframe = nframesX ;
	//StereoMix(final, startframe, nframes3, v3) ;
	//startframe += nframesX ;
	//StereoMix(final, startframe, nframesX, g) ;
	//startframe = nframesX ;
	//StereoMix(final, startframe, nframesX, g) ;
	//startframe = nframesX ;
	//StereoMix(final, nframesX, nframes4, v4) ;
	//startframe += nframesA ;
	printf("\tDone\n") ;

	printf("\tPart2") ;
	StereoMix(final, startframe, nframesB, p2a) ;
	//StereoMix(final, startframe, nframes1, v1) ;
	//StereoMix(final, startframe, nframesB, v2) ;
	//StereoMix(final, startframe, nframes2, v3) ;
	//StereoMix(final, startframe, nframes3, v4) ;
	//StereoMix(final, startframe, nframes4, v5) ;
	//StereoMix(final, startframe, nframesB, v6) ;
	startframe += nframesB ;
	printf("\tDone\n") ;

	printf("\tPart3") ;
	StereoMix(final, startframe, nframesC, p3a) ;
	StereoMix(final, startframe, nframesC, p3b) ;	
	startframe += nframesC ;
	printf("\tDone\n") ;

	printf("\tPart4") ;
	StereoMix(final, startframe, nframesD, p4a) ;
	StereoMix(final, startframe, nframesD, p4b) ;
	printf("\tDone\n") ;

	saveSoundfile("Lim.wav", final, totalDur) ;
	
	free(voice) ;
	free(p1a) ;
	free(p2a) ;
	free(p3a) ;
	free(p3b) ;
	free(p4a) ;
	free(p4b) ;
//	free(v1) ;
//	free(v2) ;
//	free(v3) ;
//	free(v4) ;

	return 0 ;
}


/*

int main(void)
{
	char *wfile[4] = {"1Your.wav", "2voice.wav", "3my.wav", "4friend.wav", NULL } ;
	double wdur[4] ;
	double *wav[4] ; 
	double *p1a, *p2a,  *final ;
	double dur1, dur2, totalDur ;
	int nframesA, nframesB, startframe, totalframes ;

	double *w1 ;
	int start, frames, i, nChan = 2 ;
								
	setChannelCount(2) ;
	setbuf(stdout, NULL) ;

	p1a = part1A(&dur1) ; printf("Part1A: %f\n", dur1) ;
	p2a = part2A(&dur2) ; printf("Part2A: %f\n", dur2) ;
	
	for ( i = 0 ; i < 4 ; i++ )
		wav[i] = readSoundfile(wfile[i], &wdur[i]) ;

	for ( i = 0, totalDur = 0 ; i < 4 ; i++ )
		totalDur += wdur[i] ;

	totalDur = dur1 + dur2 ;
	totalframes = totalDur *SAMPLING_RATE ;

	final = (double *)calloc(totalDur * SAMPLING_RATE * nChan, sizeof(double)) ;
										
	nframesA = dur1 * SAMPLING_RATE ;
	nframesB = dur2 * SAMPLING_RATE ;

	for ( i = 0, start = 0 ; i < 4 ; i++ )
	{
	frames = wdur[i] * SAMPLING_RATE ;

		StereoMix(final, start, frames, wav[i]) ;
		start += frames ;
		
	}
																
	StereoMix(final, startframe, nframesA, p1a) ;
	startframe += nframesA ;
	StereoMix(final, startframe, nframesB, p2a) ;
															
	saveSoundfile("test.wav", final, totalDur) ;

	free(p1a) ;
	free(p2a) ;

	return 0 ;
}
*/
