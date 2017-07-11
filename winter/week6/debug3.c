#include <stdio.h>
#include <stdlib.h>
#include <cm.h>
#include <math.h>
#include <pitches.h>

void makeSeq(double out[], int start, int samples, double in[])
{
	int i, j;
	for(i = start, j = 0 ; i < start + samples; i++, j++)
		out[i] = in[j] ;
	return ;
}

void mixStereo(double *outStereo, int startFrame, int nFrames,  
		double *inStereo)
{
	while ( nFrames-- ) {
		*outStereo++ += *inStereo++ ;
		*outStereo++ += *inStereo++ ;
	}
		
	return ;
}

double *PanL2R(double *mono, int frames)
{
	double *stereo, *S, *M ;
	double Left, Right, boost, panIncrement;
	int nchan = 2;

	stereo = (double *)calloc(frames * nchan, sizeof(double));

	Left = 1;
	panIncrement = 1.0 / (frames - 1);

	S = stereo;
	M = mono;

	while(frames--) {
		Right = 1.0 - Left;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
		*S++ = (Left * boost ) * *M;
		*S++ = (Right * boost ) * *M++;
		Left -= panIncrement;
	}

	return stereo;
}

double *PanR2L(double *mono, int frames)
{
	double *stereo, *S, *M ;
	double Left, Right, boost, panIncrement;
	int  nchan = 2;

	stereo = (double *)calloc(frames * nchan, sizeof(double));

	Right = 1;
	panIncrement = 1.0 / (frames - 1);

	S = stereo;
	M = mono;

	while(frames--) {
		Left = 1.0 - Right;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
		*S++ = (Right * boost ) * *M;
		*S++ = (Left * boost ) * *M++;
		Right -= panIncrement;
	}

	return stereo;
}

double *CanonPan(double *sound, int frames)
{
	double *equal, *eq;
	double Left, Right, boost;
	int nchan = 2;
	double *env, *e;
	const int nPoints = 11;
	double T[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
	double L[] = {0, 0.4, 0.7, 0.1, 0.3, 0.9, 0.6, 1.0, 0.5, 0.2, 0.8};

	eq = equal = (double *)malloc(frames * nchan * sizeof(double));
	e = env = (double *)malloc(frames * sizeof(double));
	linearEnv(env, frames, T, L, nPoints);

	while (frames--) {
		Left = *e;
		Right = ( 1.0 - *e++) ;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
		*eq++ = (Left * boost) * *sound ;
		*eq++ = (Right * boost) * *sound++; 
	}

	free(env) ;

	return equal ;
}


double *PanM2L(double *sound, int frames)
{
	double *equal, *eq;
	double Left, Right, boost;
	int nchan = 2;
	double *env, *e;
	const int nPoints = 2;
	double T[] = {0,   1.0};
	double L[] = {0.5, 1.0};

	eq = equal = (double *)malloc(frames * nchan * sizeof(double));
	e = env = (double *)malloc(frames * sizeof(double));
	linearEnv(env, frames, T, L, nPoints);

	while (frames--) {
		Left = *e;
		Right = ( 1.0 - *e++) ;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
		*eq++ = (Left * boost) * *sound ;
		*eq++ = (Right * boost) * *sound++; 
	}

	free(env) ;
	
	return equal;
}


double *PanM2R(double *sound, int frames)
{
	double *equal, *eq;
	double Left, Right, boost;
	int nchan = 2;
	double *env, *e;
	const int nPoints = 2;
	double T[] = {0, 1.0};
	double L[] = {0.5, 0};

	eq = equal = (double *)malloc(frames * nchan * sizeof(double));
	e = env = (double *)malloc(frames * sizeof(double));
	linearEnv(env, frames, T, L, nPoints);

	while (frames--) {
		Left = *e;
		Right = ( 1.0 - *e++) ;
		boost = 1.0 / sqrt(Left * Left + Right * Right);
		*eq++ = (Left * boost) * *sound ;
		*eq++ = (Right * boost) * *sound++; 
	}

	free(env) ;

	return equal;
}

void envelope1(double tone[], double dur)
{
	double ampList[]  = {0, 1.0, 0.0};
	double timeList[] = {0, 0.1, 1.0};
	int nPoints = 3;
	envelope(tone, dur, timeList, ampList, nPoints);
	return ;
}

void envelope2(double tone[], double dur)
{
	double ampList[]  = {0, 1.0, 0.2, 0.8};
	double timeList[] = {0, 0.1, 0.3, 1.0};
	int nPoints = 4;
	envelope(tone, dur, timeList, ampList, nPoints);
	return ;
}

double *fm(double cF, double mF, double dur, double SR, double dB)
{
	double cPhase, mPhase,  A;
	double cInc, mInc;
	double *wave, *p;
	int samples;
	int PD = 10;
	

	cInc = (2*M_PI * cF/ SR);
	mInc = (2*M_PI * mF/ SR);

	samples = dur * SR;

	A = pow(10.0, dB/20);

	cPhase = 0;
	mPhase = 0;
	
	wave = (double *)calloc(samples, sizeof(double));

	for(p = wave ; p < wave + samples;  p++){
		*p = A * sin(cPhase + (PD * sin(mPhase)));
		cPhase = cPhase + cInc;
		if(cPhase >= 2*M_PI){
			cPhase = cPhase - 2*M_PI;
		}
		mPhase = mPhase + mInc;
		if(mPhase >= 2*M_PI){
			mPhase = mPhase - 2*M_PI;
		}
	}

	return wave;
}


double *SeqA1(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 6;
	double cf[] = {300, 300, 1500, 300, 300, 2700};
	double mf[] = {600, 600, 1800, 1200, 1200, 3000};
	double db = 120;
	double dur = 2;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf[i], mf[i], dur, srate, db);	
		samples = dur * srate;						
		envelope1(in, dur);
		PanM2L(in, samples);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}

double *SeqA2(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 6;
	double cf[] = {900, 300, 300, 2100, 300, 300};
	double mf[] = {1200, 900, 900, 2400, 1500, 1500};
	double db = 120;
	double dur = 2;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf[i], mf[i],  dur, srate, db);	
		samples = dur * srate;						
		envelope1(in, dur);
		PanM2R(in, samples);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}

double *SeqB3(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 5;
	double cf[] = {A4, B4, Cs4, Ds4, F4};
	double mf[] = {G3, A4, B4, Cs4, Ds4};
	double db = 120;
	double dur = 1;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf[i], mf[i],  dur, srate, db);	
		samples = dur * srate;						
		envelope2(in, dur);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}
double *SeqB2(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 1;
	double cf = 1800;
	double mf = 1200;
	double db = 108;
	double dur = 30;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf, mf,  dur, srate, db);	
		samples = dur * srate;						
		PanL2R(in, samples);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}
double *SeqB1(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 1;
	double cf = 300;
	double mf = 450;
	double db = 108;
	double dur = 30;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf, mf, dur, srate, db);	
		samples = dur * srate;						
		PanR2L(in, samples);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}

double *SeqC(double *totalDuration, int srate)
{
	double *output, *in;
	int outputIndex, i, samples;

	int nTones = 10 ;
	double cf[] = {400, 700, 100, 300, 900, 600, 1000, 500, 200, 800};
	double mf[] = {600, 1050, 150, 450, 1350, 900, 1500, 750, 300, 1200};
	double db = 112;
	double dur = 6;
	int nchan = 2;

	*totalDuration = 0;			
	for(i=0; i<nTones; i++)
		*totalDuration += dur;
	
	output = (double *)calloc(*totalDuration * srate * nchan, sizeof(double));
	
	outputIndex = 0;	
	
	for(i=0; i<nTones; i++){
		in = fm(cf[i], mf[i],  dur, srate, db);	
		samples = dur * srate;						
		CanonPan(in, samples);
		makeSeq(output, outputIndex, samples, in);
		outputIndex = outputIndex + samples;			
		free(in);						
	}
	return output;
}
int main(void)
{
	int nchan = 2;
	int samples, start;
	int	SR = 96000;
	double *toneA1, *toneA2, *toneB1, *toneB2, *toneB3, *toneC, *final;
	double durA1, durA2, durB1, durB2, durB3, durC, totaldur;

	toneA1 = SeqA1(&durA1, SR);
	toneA2 = SeqA2(&durA2, SR);
	toneB1 = SeqB1(&durB1, SR);
	toneB2 = SeqB2(&durB2, SR);
	toneB3 = SeqB3(&durB3, SR);
	toneC  = SeqC(&durC, SR);

	totaldur = 3 * (durA1 + (durA2/6)) + durB1 + (1.1 * durC); 

	final = (double *)calloc(samples * totaldur * nchan, sizeof(double));
	
	start = 0;
	samples = durA1 * SR ;
	mixStereo(final, start, samples, toneA1);

	start += (durA1/6) * SR ;
	samples = durA2 * SR * nchan;
	mixStereo(final, start, samples, toneA2);

	start += durA2 * SR ;
	samples = durB1 * SR ;
	mixStereo(final, start, samples, toneB1);
	mixStereo(final, start, samples, toneB2);
	mixStereo(final, start, samples, toneB3);

	start += durB1 * SR;
	samples = durA1 * SR ;
	mixStereo(final, start, samples, toneA1);

	start += (durA1/6) * SR ;
	samples = durA2 * SR ;
	mixStereo(final, start, samples, toneA2);

	start += durA2 * SR ;
	samples = durC * SR ;
	mixStereo(final, start, samples, toneC);

	start += .05 * durC * SR ;
	mixStereo(final, start, samples, toneC);

	start += .05 * durC * SR ;
	mixStereo(final, start, samples, toneC);

	start += durC * SR ;
	samples = durA1 * SR ;
	mixStereo(final, start, samples, toneA1);

	start += (durA1/6) * SR ;
	samples = durA2 * SR ;
	mixStereo(final, start, samples, toneA2);
	
	free(toneA1);
	free(toneA2);
	free(toneB1);
	free(toneB2);
	free(toneB3);
	free(toneC);

	saveSoundfile("rondo.wav", final, totaldur);

	free(final);

	return 0;
}


	



