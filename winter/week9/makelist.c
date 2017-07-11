#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h> 
#include <cm.h> 

typedef struct FMstruct{     // a new variable: FMstruct
	double C ;
	double M ;
	double pd ;
	double dur ;
	double dB ;
	int i ;  //index
	double *tone ;
	struct FMstruct *next ;
} FMstruct ;

void mixSound(double out[], int start, int samples, double in[])
{int i, j;
	for( i = start, j=0; i<start + samples; i++, j++)
	out[i] = in[j]; 
		
	return;
}

double *FM(FMstruct *d)
{
	double cfreq = d->C ; 
	double mfreq = d->M ;
	double pd = d-> pd ;
	double dur = d-> dur ;
	double dB = d-> dB ;
	double cPhase, mPhase, cInc, mInc, A, twopi ;
	double *final, *p ;
	int samples ;
	twopi = 2*M_PI ;
	samples = dur * SAMPLING_RATE ;

	A = pow(10.0,dB/20)/2 ;

	cInc = (twopi * cfreq)/SAMPLING_RATE ;
	mInc = (twopi * mfreq)/SAMPLING_RATE ;

	final = (double *)calloc(samples, sizeof(double)) ;

	cPhase = 0 ;
	mPhase = 0 ;
	
	for(p=final; p<final+samples ; p++)
	{
		*p = A *sin(cPhase + pd * sin(mPhase)) ;
		cPhase += cInc ;
		mPhase += mInc ;

		if(cPhase >= twopi)cPhase -= twopi ;
		if(mPhase >= twopi)mPhase -= twopi ;
	}
	return final ;
}



void addToList(FMstruct **dataList, FMstruct *new)
{
	FMstruct *d ;

	if(*dataList == NULL)
	{
		*dataList = new ;
		new->next = NULL ;
		return ;
	}

	for (d=*dataList ; d->next !=NULL ; d = d->next) ;

	d->next = new ;
	new->next = NULL ;

	return ;
}

FMstruct *initData(double C, double M, double pd, double dur, double dB, int i, double *tone)
{
	FMstruct *new ;
	new = (FMstruct *)malloc(sizeof(FMstruct)) ;

	new->C = C ;
	new->M = M ;
	new->pd = pd ;
	new->dur = dur ;
	new->dB = dB ;
	new->i = i ;
	new->tone = tone ;
	
//	strcpy(new->C, M, pd, dur, dB, i, tone) ;

	return new ;
}

void showList(FMstruct *dataList)
{
	FMstruct *d ;
	for(d=dataList; d !=NULL; d = d->next)
		printf("d->C = %f\n d->M = %f\n d->pd = %f\n d->Dur = %f\n d->dB = %f\n d->i = %d\n", d->C, d->M, d->pd, d->dur, d->dB, d->i) ;
}

int main(void)
{
	double C = 220 ;
	double C2 = 440;
	double C3 = 660;
	double M = 440 ;
	double M2 = 660; 
	double M3 = 880;
	double pd = 12 ;
	double dur = 3 ;
	double dB = 144 ;
	int i = 0 ;
	int tonesamples, start; 
	double *tone, *final, *note1, *note2, *note3 ;
	double totaldur ;
	FMstruct *d ;
	FMstruct *dataList = NULL ;

	totaldur = 9 ;
	
	final = (double *)calloc(9,sizeof(double)) ;
	tone = (double *)calloc(3, sizeof(double)) ;

	d = initData(C, M, pd, dur, dB, i++, tone) ;
	addToList(&dataList, d) ;
	note1 = d->tone = FM(d) ;

	d = initData(C2, M2, pd, dur, dB, i++, tone) ;
	addToList(&dataList, d) ;
	note2 = d->tone = FM(d) ;

	d = initData(C3, M3, pd, dur, dB, i++, tone) ;
	addToList(&dataList, d) ;
	note3 = d->tone = FM(d) ;

	showList(dataList) ;

	for(d=dataList ; d !=NULL; d = d->next)
		d->tone = FM(d) ;

	totaldur = 0 ;
	for(d=dataList; d !=NULL ; d = d->next)
		totaldur += d->dur ;

	final = (double*)calloc(totaldur * SAMPLING_RATE,sizeof(double));

	start = 0; 
	for(d = dataList; d !=NULL; d=d->next)
	{tonesamples=d->dur*SAMPLING_RATE ;
	mixSound(final, start, tonesamples, note1); 
	start += tonesamples; 
	}
	saveSoundfile("Mur.wav", final, totaldur);  	

	return 0 ;
}

