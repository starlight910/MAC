#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<unistd.h>
#include<cm.h>

typedef struct VOICE
{
	int index;
	double *data;
	int samples;
	int ms ;
	struct VOICE *next;
} VOICE;

typedef struct MYGRAIN
{
	int index ;     
	int density ;          // No. of grains
	double F1, F2 ;
	double DB1, DB2 ;      // Amp
	double maxD, minD ;
	int type ;	 // Wave type  gSINE, gSAWTOOTH, gSQUARE, gTRIANGLE, gPARALLELOGRAM
	int seed ;
	struct MYGRAIN *next ;
} MYGRAIN ;

void addToList(VOICE **list, VOICE *new)
{
	VOICE *v ;
	
	if ( *list == NULL ) 
	{
		*list = new ;
		new->next = NULL ;
		return ;
    }
	
	for(v = *list; v->next != NULL; v = v->next ) ;
	
    v->next = new ;
	new->next = NULL ;
   
	return ;
}

void addToListG(MYGRAIN **list, MYGRAIN *new)
{
	MYGRAIN *G ;

	if ( *list == NULL )
	{
		*list = new ;
		new->next = NULL ;
		return ;
	}

	for(G = *list; G->next != NULL; G = G->next ) ;

	G->next = new ;
	new->next = NULL ;

	return ;
}

void showList(VOICE **list)
{
	VOICE *v ;   	

	for ( v = *list ; v != NULL ; v = v->next ) 
	{
		printf("index = %d\n", v->index) ;
	    printf("samples = %d\n", v->samples) ;
	    printf("MS = %d\n\n", v->ms) ;
	}

	return ;
}

void showListG(MYGRAIN *list)
{
	MYGRAIN *G ;

	for ( G = list; G != NULL; G = G->next )
	{
			printf("\tindex: %d\n", G->index) ;
			printf("\t\tF1: %f  F2: %f\n", G->F1, G->F2) ;
			printf("\t\tDB1: %f  DB2: %f\n", G->DB1, G->DB2) ;
			printf("\t\tmaxD: %f ms  minD: %f ms\n", G->maxD, G->minD) ;
			printf("\t\ttype: %d  seed: %d\n", G->type, G->seed) ;
	}

	return ;
}

void lengthen(double *input, double inputDur, 
		double *output, double outputDur)
{
	int inputSamples = inputDur * SAMPLING_RATE ;
	int outputSamples = outputDur * SAMPLING_RATE ;
	double *out = output ;

	while ( outputSamples > inputSamples ) {
		memcpy(out, input, inputSamples * sizeof(double)) ;
		out += inputSamples ;
		outputSamples -= inputSamples ;
	}

	if ( outputSamples > 0 ) 
		memcpy(out, input, outputSamples * sizeof(double)) ;

	return ;
}

void VOICEdur(double *data, double dur, VOICE **list)
{
	int maxDurMS = 700, minDurMS = 100 ;
	int durationInMilliseconds ;
	double *pd, *pv ;
	VOICE *v ;
	int counter = 0 ;
	double *bigData, bigDataDur = 60 ;

	bigData = (double *)Malloc(bigDataDur * SAMPLING_RATE * sizeof(double));

	lengthen(data, dur, bigData, bigDataDur) ;

	durationInMilliseconds  = bigDataDur * 1000 ;
	pd = bigData ;

	while( durationInMilliseconds > minDurMS ) {
		v = (VOICE *)Calloc(1, sizeof(VOICE)) ;
		v->index = counter++ ;
		v->ms = (maxDurMS - minDurMS) * frand() + minDurMS ;
		v->samples = ( v->ms * 0.0001 ) * SAMPLING_RATE ;
		v->data = (double *)Calloc(v->samples, sizeof(double)) ;
		for ( pv = v->data ; pv < v->data + v->samples ; pv++ )
			*pv = *pd++ ;
		addToList(list, v) ;
		durationInMilliseconds -= v->ms ;
	}

	Free(data) ;
	Free(bigData) ;

	return ;
}

void selectArray(VOICE *list, VOICE array[], 
		int minMS, int maxMS, int *arrayLen, int maxLen)
{
	VOICE *v ;
	*arrayLen = 0 ;

	for ( v = list ; v != NULL ; v = v->next)
	{
		if ( *arrayLen + 1 == maxLen )
			break ;
		if ( v->ms >= minMS && v->ms < maxMS ) 
		{
			array[*arrayLen] = *v ;
			*arrayLen += 1 ;
		}
	}
}

void showArray(VOICE *list, int len)
{
	int i, totalSamples ;
	VOICE *v ;

	totalSamples = 0 ;
	for ( i = 0 ; i < len ; i ++ )
	{
		v = &list[i] ;
		fprintf(stderr,"\tindex: %2d | MS: %2d | samples: %d\n", v->index, v->ms, v->samples) ;
		totalSamples += v->samples ;
	}
	fprintf(stderr," Total Duration: %f seconds\n", totalSamples / SAMPLING_RATE) ;
	fputc('\n', stderr) ;

	return ;
}

double getDuration(VOICE *list, int N)
{
	double dur ;
	int i ;
	VOICE *v ;

	dur = 0 ;
	for ( i = 0 ; i < N ; i++ ) {
		v = &list[i] ;
		dur += v->ms * 0.001 ;
	}

	return dur ;
}

void rampCopy(double *out, double *in, int samples)
{
	int i ;
	int rampLen ;
	double R ;
	double rinc ;
	
	rampLen = samples / 10 ;
	rinc = 1.0 / (rampLen - 1 ) ;
	samples -= rampLen * 2 ;

	for ( i = 0, R = 0 ; i < rampLen ; i++, R+=rinc )
		*out++ = R * *in++ ;

	while ( samples-- )
		*out++ = *in++ ;

	for ( i = 0, R = 1 ; i < rampLen ; i++, R -= rinc )
		*out++ = R * *in++ ;

	return ;
}

void spread(VOICE *vArray, int nParts, 
		double **channel8, double finalDuration)
{
	int silence, start, i, channel, samples ;
	double dur, *out, *in ;
	VOICE *v ;

	dur = getDuration(vArray, nParts) ;

	finalDuration -= dur ;

	silence = (finalDuration / nParts) * SAMPLING_RATE ;

	for ( i = 0, start = 0 ; i < nParts ; i++ ) 
	{

		start += silence / 5 ;
		channel = irand() % 8 ;

		v = &vArray[i] ;
		samples = v->samples ;

		out = channel8[channel] + start ;
		in = v->data ;

		rampCopy(out, in, v->samples) ;

		start += v->samples ;
	}

	return ;
}


void mixsound(double *out, int start, int samples, double *in)
{
	out += start ;
	
	while(samples--)
		*out++ += *in++ ;
}


void mix8sound(double **output, int startFrame, int channel, int samples, double *input)
{
    double *po ;

	po = output[channel] ;  // po points at the right channel
	po += startFrame ;      // po points at the right start frame
	
	while ( samples-- )     // copy input to output

		*po++ += *input++ ;
	
	return ;
}


void Envelope1(double *MYGRAIN, double duration)
{
	double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double A[] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0} ;
	double npoints = 11 ;

	envelope(MYGRAIN, duration, T, A, npoints) ;			
}

void createCloud(double **output, int startFrame, int channel, MYGRAIN G, double duration)
{
	double *sound ;
	GRAIN g ;
	int samples = duration * SAMPLING_RATE ;

	g.density = G.density ;
	g.F0 = G.F1 ;
	g.F1 = G.F2 ;
	g.DB0 = G.DB1 ;
	g.DB1 = G.DB2 ;
	g.maxD = G.maxD ;
	g.minD = G.minD ;
	g.type = G.type ;
	g.seed = G.seed ;

	sound = grainCloud(g, duration) ;

	mix8sound(output, startFrame, channel, samples, sound) ;

	Free(sound) ;
	
	return ;
}			

int main(void)
{
	VOICE *vlist = NULL ;
	VOICE v1[1000], v2[1000], v3[500], v4[700], v5[300], v6[100] ;
	int len1, len2, len3, len4, len5, len6 ;
	double *data, dur ;
	double **channel8 ;
	double *finalOutput ;
	double totalDuration ;
	double channel ;
	double duration ;
	int startFrame ;
	MYGRAIN G ;

	data = readSoundfile("thiskillme.wav", &dur) ;
	VOICEdur(data, dur, &vlist) ;

	selectArray(vlist, v1, 300, 500, &len1, 1000) ;
	selectArray(vlist, v2, 100, 300, &len2, 1000) ;
	selectArray(vlist, v3, 200, 500, &len3, 500) ;
	selectArray(vlist, v4, 200, 700, &len4, 700) ;
	selectArray(vlist, v5, 400, 700, &len5, 300) ;
	selectArray(vlist, v6, 500, 700, &len6, 100) ;


	fprintf(stderr,"len1 = %d  len2 = %d  len3 = %d\n  len4 = %d\n  len5 = %d\n, len6 = %d\n" , len1, len2, len3, len4, len5, len6) ;

	showArray(v1, len1) ;
	showArray(v2, len2) ;
//	showArray(v3, len3) ;
//	showArray(v4, len4) ;
//	showArray(v5, len5) ;


	totalDuration = 180 ;
	startFrame = 0.5 ;

	channel8 = channel8Allocate(totalDuration) ;

// voice spread1

	spread(v1, len1, channel8, totalDuration) ;
    spread(v2, len2, channel8, totalDuration) ;
//	spread(v3, len3, channel8, totalDuration) ;
//	spread(v4, len4, channel8, totalDuration) ;
//	spread(v5, len5, channel8, totalDuration) ;


// Sound1

	//ch1	gap 0   end 20
	G.density = 300 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 110.0 ; G.DB2 = 120.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch2
	G.density = 300 ;
	G.F1 = 44.0 ; G.F2 = 55.0 ;
	G.DB1 = 115.0 ; G.DB2 = 125.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 2 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch3
	G.density = 300 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch4
	G.density = 300 ;
	G.F1 = 66.0 ; G.F2 = 77.0 ;
	G.DB1 = 110.0 ; G.DB2 = 120.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 4 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch5
	G.density = 300 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 5 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch6
	G.density = 200 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7
	G.density = 300 ;
	G.F1 = 440.0 ; G.F2 = 450.0 ;
	G.DB1 = 110.0 ; G.DB2 = 120.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 7 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch0
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 110.0 ; G.DB2 = 120.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 0 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
//	fprintf(stderr,"after channel 0\n") ;
//	fgetc(stdin) ;

	startFrame = 15 * SAMPLING_RATE ;

// Sound2   gap 15   end 35
	//ch1
	G.density = 1000 ;
	G.F1 = 22.0 ; G.F2 = 33.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 10.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch2
	G.density = 200 ;
	G.F1 = 220.0 ; G.F2 = 230.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 2 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch3
	G.density = 700 ;
	G.F1 = 66.0 ; G.F2 = 77.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch4
	G.density = 800 ;
	G.F1 = 880.0 ; G.F2 = 890.0 ;
	G.DB1 = 100.0 ; G.DB2 = 110.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 4 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch5 killed

	//ch6
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 30.0 ; G.maxD = 50.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 7 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch0
	G.density = 300 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 10.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 0 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	startFrame += 30 * SAMPLING_RATE ;

// Sound3	gap 30  end 45
	//ch1
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch2
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 2 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch3
	G.density = 100 ;
	G.F1 = 88.0 ; G.F2 = 99.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch4
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 4 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch6
	G.density = 800 ;
	G.F1 = 11.0 ; G.F2 = 22.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7
	G.density = 800 ;
	G.F1 = 110.0 ; G.F2 = 120.0 ;
	G.DB1 = 110.0 ; G.DB2 = 120.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 7 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch8 killed
	
	startFrame = 40 * SAMPLING_RATE ;

	
// voice spread2

	showArray(v4, len4) ;
	showArray(v5, len5) ;
	showArray(v6, len6) ;

	spread(v4, len4, channel8, totalDuration) ;
	spread(v5, len5, channel8, totalDuration) ;
	spread(v6, len6, channel8, totalDuration) ;


// Sound4	gap 40 end 60
	//ch1
	G.density = 10 ;
	G.F1 = 66 ; G.F2 = 77 ;
	G.DB1 = 120 ; G.DB2 = 130 ;
	G.minD = 1.0 ; G.maxD = 50.0 ;
	G.type = gSQUARE ;
	G.seed = 1000 ;
	duration = 20 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch2 killed

	//ch3
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch4
	G.density = 500 ;
	G.F1 = 110.0 ; G.F2 = 120.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 4 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch6
	G.density = 200 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch7
	G.density = 600 ;
	G.F1 = 440.0 ; G.F2 = 450.0 ;
	G.DB1 = 100.0 ; G.DB2 = 110.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	startFrame = 55 * SAMPLING_RATE ;

//sound5	gap 55 end 70
	//ch1
	G.density = 100 ;
	G.F1 = 110.0 ; G.F2 = 120.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 1 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch3
	G.density = 600 ;
	G.F1 = 220.0 ; G.F2 = 230.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch4 killed
	
	//ch6
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 10.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7
	G.density = 500 ;
	G.F1 = 440.0 ; G.F2 = 450.0 ;
	G.DB1 = 100.0 ; G.DB2 = 110.0 ;
	G.minD = 10.0 ; G.maxD = 50.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 7 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	startFrame = 65 * SAMPLING_RATE ;

//sound5 gap 65 end 85
	//ch1 killed
	
	//ch3
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 20.0 ; G.maxD = 40.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;
	
	//ch6
	G.density = 100 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 20.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 20 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	startFrame = 80 * SAMPLING_RATE ;

//sound6 gap 80 end 90
	
	//ch3
	G.density = 500 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 1.0 ; G.maxD = 10.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 10 ;
	channel = 3 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch6
	G.density = 200 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 10.0 ; G.maxD = 40.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch7 killed
	
	startFrame = 85 * SAMPLING_RATE ;

//sound7 gap 85 end 100
	//ch3
	G.density = 90 ;
	G.F1 = 55.0 ; G.F2 = 66.0 ;
	G.DB1 = 120.0 ; G.DB2 = 130.0 ;
	G.minD = 10.0 ; G.maxD = 30.0 ;
	G.type = gSQUARE ;
	G.seed = 500 ;
	duration = 15 ;
	channel = 6 ;
	createCloud(channel8, startFrame, channel, G, duration) ;

	//ch6 killed


	finalOutput = channel8Interleave(channel8, totalDuration) ;

	channel8Free(channel8) ;

	saveSoundfile("RAD.wav", finalOutput, totalDuration) ;

	Free(finalOutput) ;

	return 0 ;

}
