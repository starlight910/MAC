#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cm.h>

typedef struct myGRAIN 
{
	int index ;
	int density ;		// number of grains
	double F0, F1 ;		// frequency in hertz
	double DB0, DB1 ;	// amplitude in dB			
	double maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)						
	int seed ;			// random number seed
	struct myGRAIN *next ;
} myGRAIN ;

void addToList(myGRAIN **gList, myGRAIN *new)
{
	    myGRAIN *g ;

		    if ( *gList == NULL ) 
			{
				*gList = new ;
				new->next = NULL ;			
				return ;
			} 
			
			for ( g = *gList ; g->next != NULL ; g = g->next) ;	    
			g->next = new ;		   
			new->next = NULL ;			   
			return ;
}

void mixsound(double *out, int start, int samples, double *in)
{
	out += start ;

	while(samples--)
		*out++ += *in++ ;
}

void showList(myGRAIN *gList)
{
	myGRAIN *g ;

	for ( g = gList ; g != NULL ; g = g->next ) 
	{
		printf("\tIndex: %d\n", g->index) ;
		printf("\t\tF1: %f | F2: %f\n", g->F0, g->F1) ;
		printf("\t\tDB1: %f | DB2: %f\n", g->DB0, g->DB1) ;
		printf("\t\tMaxD: %f ms | MinD: %f ms\n", g->maxD, g->minD) ;
		printf("\t\tType: %d | Seed: %d\n", g->type, g->seed) ;
	}

	return ;
}

void Envelope1(double *cloud, double duration)
{
    double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
    double A[] = {0.0, 1.0, 0.6, 0.8, 0.7, 0.9, 0.6, 0.9, 0.7, 0.1, 0.0} ;
    int nPoints = 11 ;

    envelope(cloud, duration, T, A, nPoints) ;

    return ;
}

void Envelope2(double *cloud, double duration)
{
	 double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	 double A[] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0} ;
	 int nPoints = 11 ;
	 
	 envelope(cloud, duration, T, A, nPoints) ;
	
	 return ;
}

void Envelope3(double *cloud, double duration)
{
	double T[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0} ;
	double A[] = {1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0} ;
	int nPoints = 11 ;

	envelope(cloud, duration, T, A, nPoints) ;

	return ;
}
	
void mix8sound(double **output, int startFrame, int channel, 
		int samples, double *input)
{
	double *po ;
	po = output[channel] ;	// po points at the right channel
	po += startFrame ;		// po points at the right start frame

	while ( samples-- )		// copy input to output 
		*po++ += *input++ ;

	return ;
}

//sound loop
void createCloud(double **output, int startFrame, int channel, 
		myGRAIN G, double duration)
{
	double *sound ;
	GRAIN g ;
	int samples = duration * SAMPLING_RATE ;


#if 0
typedef struct myGRAIN 
{
	int index ;
	int density ;		// number of grains
	double F0, F1 ;		// frequency in hertz
	double DB0, DB1 ;	// amplitude in dB			
	int maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)						
	int seed ;			// random number seed
	struct myGRAIN *next ;
} myGRAIN ;

typedef struct GRAIN {
	int density ;		// number of grains
	double F0, F1 ; 	// frequency in hertz
	double DB0, DB1 ; 	// amplitude in dB
	double maxD, minD ;	// duration of each grain in milliseconds (0.001)
	int type ;			// see below (waveTypes)
	int seed ;			// random number seed
} GRAIN ;

#endif

	g.density = G.density ;
	g.F0 = G.F0 ;
	g.F1 = G.F1 ;
	g.DB0 = G.DB0 ;
	g.DB1 = G.DB1 ;
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
	double **channel8, *finalOutput ;
	double totalDuration ;
	double channel ;
	double duration ;
	int startFrame ;
	myGRAIN g ;

	totalDuration = 100 ;
	startFrame = 0 ;

	channel8 = channel8Allocate(totalDuration) ;

//sound1   ch1 d30
	g.density = 10 ;
	g.F0 = 440 ; g.F1 = 450 ;
	g.DB0 = 122 ; g.DB1 = 133 ;
	g.minD = 10 ; g.maxD = 30000 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 30 ;
	channel = 1 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 2 * SAMPLING_RATE ;

//sound2   ch2 d32
	g.density = 10 ;
	g.F0 = 110 ; g.F1 = 120 ;
	g.DB0 = 122 ; g.DB1 = 144 ;
	g.maxD = 30 ; g.minD = 15 ;
	g.type = gSQUARE ;
	g.seed = 600 ;
	duration = 30 ;
	channel = 2 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 2 * SAMPLING_RATE ;

//sound3   ch5 d44
	g.density = 40 ;
    g.F0 = 220 ; g.F1 = 230 ;
    g.DB0 = 133 ; g.DB1 = 144 ;
    g.maxD = 15 ; g.minD = 10 ;
    g.type = gSQUARE ;
    g.seed = 800 ;
    duration = 40 ;
    channel = 5 ;

    createCloud(channel8, startFrame, channel, g, duration) ;
    startFrame += 1.5 * SAMPLING_RATE ;
	
//sound4   ch3  dur45.5
	g.density = 40 ;
	g.F0 = 220 ; g.F1 = 230 ;
	g.DB0 = 133 ; g.DB1 = 144 ;
	g.maxD = 55 ; g.minD = 50 ;
	g.type = gSQUARE ;
	g.seed = 200 ;
	duration = 40 ;
	channel = 3 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 2 * SAMPLING_RATE ;

//sound5   ch4 dur47.5
	g.density = 40 ;
	g.F0 = 550 ; g.F1 = 560 ;
	g.DB0 = 133 ; g.DB1 = 144 ;
	g.maxD = 100 ; g.minD = 90 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 4 ;
	createCloud(channel8, startFrame, channel, g, duration) ;

//sound6   ch1 dur67.5
	g.density = 10 ;
	g.F0 = 55 ; g.F1 = 66 ;
	g.DB0 = 122 ; g.DB1 = 133 ;
	g.minD = 10 ; g.maxD = 30 ;
	g.type = gSQUARE ;
	g.seed = 1000 ;
	duration = 60 ;
	channel = 1 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 10 * SAMPLING_RATE ;

//sound 7   ch6 dur 37.5
	g.density = 500 ;
	g.F0 = 220 ; g.F1 = 230 ;
	g.DB0 = 133 ; g.DB1 = 144 ;
	g.minD = 40 ; g.maxD = 70 ;
	g.type = gSQUARE ;
	g.seed = 1000 ;
	duration = 20 ;
	channel = 6 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 1.5 * SAMPLING_RATE ;

//sound 8   ch7 dur79
	g.density = 500 ;
	g.F0 = 55 ; g.F1 = 66 ;
	g.DB0 = 122 ; g.DB1 = 144 ;
	g.minD = 10 ; g.maxD = 50 ;
	g.type = gSQUARE ;
	g.seed = 1000 ;
	duration = 60 ;
	channel = 7 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 2 * SAMPLING_RATE ;

//sound 9	ch0 dur49
	g.density = 10 ;
	g.F0 = 110 ; g.F1 = 120 ;
	g.DB0 = 122 ; g.DB1 = 133 ;
	g.minD = 10 ; g.maxD = 25000 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 30 ;
	channel = 0 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 3 * SAMPLING_RATE ;

//sound 10   ch4 dur62
	g.density = 500 ;
	g.F0 = 880 ; g.F1 = 890 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 4 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 3 * SAMPLING_RATE ;


//sound 11   ch5 dur85    gap 25
	g.density = 500 ;
	g.F0 = 55 ; g.F1 = 66 ;
	g.DB0 = 122 ; g.DB1 = 133 ;
	g.minD = 10 ; g.maxD = 30 ;
	g.type = gSQUARE ;
	g.seed = 1000 ;
	duration = 60 ;
	channel = 5 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	
//sound 12   ch7 dur65
	g.density = 300 ;
	g.F0 = 55 ; g.F1 = 66 ;
	g.DB0 = 111 ; g.DB1 = 122 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 7 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 3 * SAMPLING_RATE ;

//sound 13 ch2 dur 68
	g.density = 100 ;
	g.F0 = 110 ; g.F1 = 120 ;
	g.DB0 = 100 ; g.DB1 = 110 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 2 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 2 * SAMPLING_RATE ;

//sound 14 ch0 dur70    gap 30
	g.density = 1000 ;
	g.F0 = 440 ; g.F1 = 450 ;
	g.DB0 = 122 ; g.DB1 = 133 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 0 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 3 * SAMPLING_RATE ;

//sound 15 ch3 dur73   gap 33
	g.density = 100 ;
	g.F0 = 1760 ; g.F1 = 1770 ;
	g.DB0 = 99 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 3 ;

	createCloud(channel8, startFrame, channel, g, duration) ;
	startFrame += 5 * SAMPLING_RATE ;

//sound 16 ch6 dur78   gap 38   
	g.density = 500 ;
	g.F0 = 880 ; g.F1 = 890 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 40 ;
	channel = 4 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	startFrame += 32 * SAMPLING_RATE ;

	g.density = 500 ;
	g.F0 = 880 ; g.F1 = 890 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 1 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 1000 ;
	g.F0 = 770 ; g.F1 = 780 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 2 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 100 ;
	g.F0 = 660 ; g.F1 = 670 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 3 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 30 ;
	g.F0 = 550 ; g.F1 = 560 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 4 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 500 ;
	g.F0 = 440 ; g.F1 = 450 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 6 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 700 ;
	g.F0 = 330 ; g.F1 = 340 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 7 ;

	createCloud(channel8, startFrame, channel, g, duration) ;

	g.density = 10 ;
	g.F0 = 220 ; g.F1 = 230 ;
	g.DB0 = 100 ; g.DB1 = 111 ;
	g.type = gSQUARE ;
	g.seed = 500 ;
	duration = 15 ;
	channel = 0 ;

	createCloud(channel8, startFrame, channel, g, duration) ;


	finalOutput = channel8Interleave(channel8, totalDuration) ;

	channel8Free(channel8) ;

	saveSoundfile("EA-JAMMING.wav", finalOutput, totalDuration) ;
					//Electric Attack JAMMING!
	Free(finalOutput) ;
	//Free(channel8) ;

	return 0 ;
}
