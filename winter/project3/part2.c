#include <stdlib.h>
#include <stdio.h>
#include <cm.h>
#include <math.h>
#include <unistd.h>
#include <time.h> 

double *part1(double *totaldur)
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
