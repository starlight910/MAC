#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<cm.h>

typedef struct Snippet {
	int index;
	double *extract;
	int samples;
	int channel ;
	struct Snippet *next;
} Snippet;

void addToList(Snippet **list, Snippet *new)
{
	Snippet *s ;

	if ( *list == NULL ) {
		*list = new ;
		new->next = NULL ;
		return ;
	}

	for( s = *list; s->next != NULL; s = s->next ) ;

	s->next = new ;
	new->next = NULL ;

	return ;
}

void showList(Snippet **list)
{
	Snippet *s ;
	for ( s = *list ; s != NULL ; s = s->next ) { 
		printf("index = %d\n", s->index) ;
		printf("samples = %d\n", s->samples) ;
		printf("channel = %d\n\n", s->channel) ;
	}
	return ;
}

Snippet *searchIndex(Snippet **snipList, int index)
{
	Snippet *s ;

	for ( s = *snipList ; s != NULL ; s = s->next )
		if ( s->index == index )
			break ;

	if ( s == NULL ) 
		fprintf(stderr,"Index %d not in list\n", index) ;

	return s ;
}

double getDuration(Snippet **snipList)
{
    Snippet *s ;
    double dur = 0 ;

    for ( s = *snipList ; s != NULL ; s = s->next )
        dur += s->samples ;

	dur /= SAMPLING_RATE ;	

    return dur ;
}

void parseSound(Snippet **list, double *word, 
				int totalSamples, int nDivisions)
{
	Snippet *s ;
	double *ps, *pw, snipDur ;
	int length, i, chanDiv, chanCT ;
	int channels[] = {0, 1, 2, 3, 4, 5, 6, 7} ; 

	double T[] = {0.0, 0.01, 0.99, 1.0} ;
	double A[] = {0.0, 1.0, 1.0, 0.0} ;
	int nPoints = 4 ;

	pw = word ;

	// each extract has same length
	length = totalSamples / nDivisions ;
	
	chanDiv = nDivisions / 8 ;
	chanCT = 0 ;

	for(i = 0; i < nDivisions; i++) {
		s = (Snippet *)calloc(1, sizeof(Snippet)) ;
		s->index = i ;
		s->samples = length ;

		// set channel
		if(i <= chanDiv) {
			s->channel = channels[chanCT] ;
			if(i == chanDiv) {
				chanDiv += (nDivisions / 8) ;
				chanCT++ ;
			}
		}

		s->extract = (double *)calloc(s->samples, sizeof(double)) ;

		for(ps = s->extract; ps < s->extract + s->samples; ps++)
			*ps = *pw++ ;

		snipDur = s->samples / SAMPLING_RATE ;
		envelope(s->extract, snipDur, T, A, nPoints) ;

		addToList(list, s) ;
	}
	return ;
}

void makeList(Snippet **list, int nDivisions)
{
	double *word, dur ;
	int totalSamples ;

	word = readSoundfile("disposition.wav", &dur) ;
	totalSamples = dur * SAMPLING_RATE ;

	parseSound(list, word, totalSamples, nDivisions) ;

	return ; 
}

void panPlace(double **array2D, Snippet *link, int startFrame)
{
    double *out, *in ;
	int frames ;

	// set pointer to correct channel and correct starting
	// frame
    out = array2D[link->channel] + startFrame ;
	in = link->extract ;
	frames = link->samples ;

	while ( frames-- )		// copy the sound
		*out++ += *in++ ;

    return ;
}

double *interleave(double **input2D, double dur)
{
    int nchan = 8 ;
    int i, samples, frames ;
    double *output1D, *pi, *po ;

    frames = dur * SAMPLING_RATE ;
    samples = frames * nchan ;

    output1D = (double *)calloc(samples, sizeof(double)) ;

    for ( i = 0 ; i < nchan ; i++ ) {
        po = output1D + i ;
        for ( pi = input2D[i] ; pi < input2D[i] + frames ; pi++ ) {
            *po = *pi ;
            po += nchan ;
        }
    }

    return output1D ;
}

void bubbleSort(int *data, int N, int direction, 
				Snippet **snipList, int start, double **array2D)
{
	Snippet *s ;
	int flag, loopCap ;
	int *pd, tmp ;
	int counter = 0 ;

	loopCap = 21100 ;

	if ( direction == 0 ) {

		do {
			flag = 0 ;
			for ( pd = data ; pd < data + (N-1) ; pd++ ) {
				if ( *pd > *(pd+1) ) {
					tmp = *pd ;
					*pd = *(pd+1) ;
					*(pd+1) = tmp ;
					flag = 1 ;
				}
				if(++counter < loopCap){
					s = searchIndex(snipList, *pd) ;
					panPlace(array2D, s, start) ;
					start += s->samples ;
				}
			}
		} while ( flag == 1 ) ;

	} else {
		do {
			flag = 0 ;
			for ( pd = data ; pd < data + (N-1) ; pd++ ) {
				if ( *pd < *(pd+1) ) {
					tmp = *pd ;
					*pd = *(pd+1) ;
					*(pd+1) = tmp ;
					flag = 1 ;
				}
				if(++counter < loopCap) {
					s = searchIndex(snipList, *pd) ;
					panPlace(array2D, s, start) ;
					start += s->samples ;
				}
			}
		} while ( flag == 1 ) ;
	}
	
	return ;
}

void playWord(Snippet **snipList, int indexArr[], 
				int arrLength, int start, double **array2D)
{
	Snippet *s ;
	int i ;

	for(i = 0; i < arrLength; i++) {
		s = searchIndex(snipList, indexArr[i]) ;
		panPlace(array2D, s, start) ;
		start += s->samples ;
	}

	return ;
}

int main(void)
{
	Snippet *snipList = NULL ;
	double **array2D, *array1D ;
	double tDur ;
	int i, start, nDiv = 200, wordDur, nchan = 8, frames, finalWord ;
	int index[nDiv] ;

	makeList(&snipList, nDiv) ;

	wordDur = getDuration(&snipList) * SAMPLING_RATE ;
	finalWord = 155 * SAMPLING_RATE ;

	tDur = 160 ;
	frames = tDur * SAMPLING_RATE ;

	array2D = (double **)calloc(nchan, sizeof(double *)) ;
	for(i = 0; i < nchan; i++)
		array2D[i] = (double *)calloc(frames, sizeof(double)) ;

	// fill index array 0 - 199
	for(i = 0; i < nDiv; i++)
		index[i] = i ;

	start = 0 ;
	
	// sort list in decending order and play
	bubbleSortI(index, nDiv, 1) ;
	
	for(i = 0; i < 2; i++) {
		playWord(&snipList, index, nDiv, start, array2D) ;
		start += wordDur * 2 ;
	}
	
	// play word with overlap
	playWord(&snipList, index, nDiv, start, array2D) ;
	start += wordDur / 2 ;
	
	// play forward with overlap
	bubbleSortI(index, nDiv, 0) ;
	playWord(&snipList, index, nDiv, start, array2D) ;
	start += wordDur / 2 ;

	// shuffle index array
	shuffleI(index, nDiv) ;

	// "the sound of the bubble sort"
	bubbleSort(index, nDiv, 0, &snipList, start, array2D) ;	

	// play sorted word at end
	playWord(&snipList, index, nDiv, finalWord, array2D) ;

	// put in 1D array
	array1D = interleave(array2D, tDur) ;

	setChannelCount(nchan) ;
	
	unlink("project1.wav") ;
	saveSoundfile("project1.wav", array1D, tDur);

	channel8Free(array2D) ;
	free(array1D) ;

	return 0 ;
}
