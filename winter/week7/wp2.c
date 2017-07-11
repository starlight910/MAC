#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cm.h>

/*
 * buildCPDList( *probabilities, N )
 *
 * builds and returns a cumulative probablity distribution list, given
 * a list of *probablities with length N
 *
 */

double *buildCPDList(double *probabilities, int N)
{
	double *normalized, *cpd, sum ;
	int i, j ;

	// first, create a list of normalized probabilities
	// (where each value is scaled to be between 1 and 0)

	normalized = (double *)calloc(N, sizeof(double)) ;

	for ( i = sum = 0 ; i < N ; i++ )	// find the sum of the list
		sum += probabilities[i] ;

	for ( i = 0 ; i < N ; i++ )			// divide each value by the sum
		normalized[i] = probabilities[i] / sum ;

	// now create the cumulative probability distribution

	cpd = (double *)calloc(N, sizeof(double)) ;

	for ( i = 0 ; i < N ; i++ ) {
		cpd[i] = 0 ;
		for ( j = 0 ; j <= i ; j++ )
			cpd[i] += normalized[j] ;
	}

	free(normalized) ;

	return cpd ;	// return the created list
}

double biasedPick(double *cpd, int N, double *data)
{
	double R ;
	int index ;

	R = frand() ;
	for ( index = 0 ; index < N ; index++ ) {
		if ( cpd[index] >= R )
			return data[index] ;
	}

	return 0 ;
}

int biasedPickIndex(double *cpd, int N)
{
	double R ;
	int index ;

	R = frand() ;
	for ( index = 0 ; index < N ; index++ )
		if ( cpd[index] >= R )
			break ;
	
	return index ;
}

int main(void)
{
	char *dynamics[] = {"fff", "ff", "f", "mf", "mp", "p", "pp", "ppp"} ;
	double probabilities[] = {0.06, 0.06, 0.06, 0.06, 0.06, 0.60, 0.06, 0.06} ; 
	double *cpd ;
	int i, index, N = 8 ;
	double *answers, sum ;

	setRandSeed( time(0) ) ;

	cpd = buildCPDList(probabilities, N) ;

	for ( i = 0 ; i < N ; i++ )		// print the results
		printf("%2d: probabilities: %f  cpd: %f\n",
				i, probabilities[i], cpd[i]) ;

	answers = (double *)calloc(N, sizeof(double)) ;

	printf("\n") ;
	for ( i = 0 ; i < 1000 ; i++ ) {
		index = biasedPickIndex(cpd, N) ;
		// printf("%-4s ", dynamics[index]) ;
		answers[index] += 1 ;
		// if ( (i+1) % 15 == 0 ) printf("\n") ;
	}
	printf("\n\n") ;

	sum = 0 ;
	for ( i = 0 ; i < N ; i++ )
		sum += answers[i] ;

	for ( i = 0 ; i < N ; i++ )
		answers[i] = answers[i] / sum ;

	for ( i = 0 ; i < N ; i++ )
		printf("%3d: prob: %f  answers: %f\n", 
				i, probabilities[i], answers[i]);

	return 0 ;
}
