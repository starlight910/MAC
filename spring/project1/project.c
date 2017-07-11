#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>

typedef struct word 
{
	int index ; 
	double *data ; 
	int samples ; 
	int ms ; 
	struct word *next ;
} word ;

void addToList(word **list, word *new)
{
	word *w ;
	if(*list==NULL)
	{
	*list = new ; 
	new->next = NULL ; 
	return;
	}
	for(w = *list; w->next !=NULL; w=w->next) ;

	w->next = new ;
	new->next = NULL ;

	return ;
}

void setupLL(double *data, double dur, word **list)
{
	int maxDurMS = 100, minDurMS = 20 ; 
	int durationInMilliseconds ;
	double *pd, *pz ;
	word *list ;
	int counter = 0 ;

	durationInMilliseconds = dur * 1000 ;
	
	pd = data ;
	
	while (durationInMilliseconds > minDurMS ) 
	{
		list = (word*)calloc(1, sizeof(word)) ; 
	    
		list->index = counter++ ;
		list->ms = (maxDurMS-minDurMS)*frand()+minDurMS ;
    
		z->samples = (z->ms * 0.001) * SAMPLING_RATE ;
		z->data = (double*)calloc(z->samples,sizeof(double)) ;

		for(pz= z->data; pz< z->data + z->samples; pz++)
			
			*pz = *pd++ ;
		    addToList(z, ZList);
			durationInMilliseconds -= z->ms ;
	
	}
	return ;
}


void showList(word **list)
{
	word *w ;
	for(w= *list ; w != NULL ; w = w->next)
	{
		printf("Index = %d\n", z->index) ;
		printf("Samples = %d\n", z->samples) ;
		printf("Channel = %d\n", z->channel) ;
	}
	return ;
}



word *searchindex(word **



void makeMono(double **data, double dur)
{
	int i, frames, samples ;
	double *mono, *m, *pd ;
	double xmin, xmax ;
	
	frames = dur * SAMPLING_RATE ;
	samples = frames * CHANNEL_COUNT ;
	getLimits(*data, samples, &xmax, &xmin);

	mono = (double *)calloc(frames, sizeof(double)) ;

	for ( m = mono, pd = *data ; m < mono + frames ; m++ ) 
	{
	for ( i = 0 ; i < CHANNEL_COUNT ; i++ )
	*m += *pd++ ;
	}
	rescale(mono, frames, xmin, xmax) ;

	free(*data) ;

	*data = mono ;

	return ;
}

double getDuration(word *list, int N)
{
    double dur ;
    int i ;
    word *w ;

    dur = 0 ;
    for ( i = 0 ; i < N ; i++ ) 
	{
		z = &zlist[i] ;
	    dur += z->ms * 0.001 ;
	}
    return dur ;
}

void makeList(word **list, int




int main(void)
{
	word *list = NULL ;
	word w1[100], w2[100], w3[100], w4[100], w5[100], w6[100], w7[100], w8[100] ;
	int len1, len2, len3, len4, len5, len6, len7, len8 ;

	double *data ;
	double dur ;
	double **channel8 ;
	double totalDuration ;
	double *finalOutput ;

	data = readSoundfile("runaway96.wav", &dur) ;
	if ( CHANNEL_COUNT != 1 )
	makeMono(&data, dur) ;
	setBitsPerSample(24) ;

	setupLL(data, dur, &list) ;

