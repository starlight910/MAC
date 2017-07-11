#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cm.h>

int main(void)
{
	char *dynamics[] = {"fff", "ff", "f", "mf", "mp", "p", "pp", "ppp"} ;
	double probabilities[] = {2, 1, 1, 1, 1, 1, 1, 1} ; 
	double cpd[8] ;
	double normalized[8] ;

	int i, j, N = 8 ;
	double sum, R ;

	setRandSeed( time(0) ) ;

	sum = 0 ;
	for ( i = 0 ; i < N ; i++ )		// add up the probabilities
		sum += probabilities[i] ;

	for ( i = 0 ; i < N ; i++ )		// normalize the probabilities
		normalized[i] = probabilities[i] / sum ;

	for ( i = 0 ; i < N ; i++ )		// cumulative probability distribution 
		for ( cpd[i] = j = 0 ; j <= i ; j++ )
			cpd[i] += normalized[j] ;

	for ( i = 0 ; i < N ; i++ )		// print the results
		printf("%2d: probabilities: %f  normalized: %f    cpd: %f\n",
				i, probabilities[i], normalized[i], cpd[i]) ;


	printf("\n") ;
	for ( i = 0 ; i < 100 ; i++ ) {
		R = frand() ;
		for ( j = 0 ; j < N ; j++ ) {
			if ( cpd[j] >= R ) {
				printf("%-4s ", dynamics[j]) ;
				break ;
			}
		}
		if ( (i+1) % 15 == 0 )
			printf("\n") ;
	}
	printf("\n\n") ;

	return 0 ;
}
