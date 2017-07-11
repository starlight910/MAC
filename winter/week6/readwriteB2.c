#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

typedef struct {
	double f ;
	double db ;
	double dur ;
} DATA ;

void readBinary(char *name, 
		double **flist, double **dblist, double **durlist, int *N)
{
	FILE *f ;
	DATA d ;
	double *pf, *pdb, *pdur ;
	int nItems, R ;

	if ( ( f = fopen(name, "rb") ) == NULL ) {
		printf("Can't open %s for binary read\n", name) ;
		exit(EXIT_FAILURE) ;
	}

	nItems = 0 ;
	while ( 1 ) {
		R = fread(&d, sizeof(DATA), 1, f) ;
		if ( R == 0 )
			break ;
		nItems++ ;
	}
	*N = nItems ;

	printf("nItesm = %d\n", nItems) ;

	rewind(f) ;

	pf = *flist = (double *)calloc(nItems, sizeof(double)) ;
	pdb = *dblist = (double *)calloc(nItems, sizeof(double)) ;
	pdur = *durlist = (double *)calloc(nItems, sizeof(double)) ;

	while ( 1 ) {
		R = fread(&d, sizeof(DATA), 1, f) ;
		if ( R == 0 ) 
			break ;
		*pf++ = d.f ;
		*pdb++ = d.db ;
		*pdur++ = d.dur ;
	}

	fclose(f) ;

	return ;
}

void writeBinary(char *name, 
		double *flist, double *dblist, double *durlist, int N)
{
	DATA d ;
	FILE *f ;
	int i ;

	if ( ( f = fopen(name, "wb") ) == NULL ) {
		printf("Can't open %s for binary write\n", name) ;
		exit(EXIT_SUCCESS) ;
	}

	for ( i = 0 ; i < N ; i++ ) {
		d.f = *flist++ ;
		d.db = *dblist++ ;
		d.dur = *durlist++ ;
		fwrite(&d, sizeof(DATA), 1, f) ;
	}

	fclose(f) ;

	return ;
}

int main(void)
{
	int N = 10000 ;
	double *flist, *dbList, *durList ;
	char *filename = "arunB.dat" ;
	void createData(double *fList, double *dbList, double *durList, int N) ;
	void showData(double *flist, double *dbList, double *durList, int N) ;

	flist = (double *)calloc(N, sizeof(double)) ;
	dbList = (double *)calloc(N, sizeof(double)) ;
	durList = (double *)calloc(N, sizeof(double)) ;

	createData(flist, dbList, durList, N) ;

	writeBinary(filename, flist, dbList, durList, N) ;

	free(flist) ;
	free(dbList) ;
	free(durList) ;

	readBinary(filename, &flist, &dbList, &durList, &N) ;

	showData(flist, dbList, durList, N) ;

	free(flist) ;
	free(dbList) ;
	free(durList) ;

	return 0 ;
}







void createData(double *fList, double *dbList, double *durList, int N)
{
	double fundamental, cents, K ;
	double dbInc, db, dbMax, dbMin ;
	double durMin, durMax, durInc, dur ;
	int i, n ;

	fundamental = 440 ;		// create a sequence of frequencies
	cents = 100 ;			// separated by 100 cents
	K = 1200.0 / log10(2.0) ;
	for ( i = 0, n = 0 ; i < N ; i++, n++ ) {
		fList[i] = fundamental * pow(10.0, (n*cents)/K) ;
		if ( (n+1) % 13 == 0 ) {
			cents *= -1 ;
			n = 0 ;
		}
	}


	dbInc = -6 ;	// create a sequence of decibel values
	dbMax = 144 ;	// that oscillate between 144 and 84 db
	dbMin = 84 ;
	db = dbMax ;		

	for ( i = 0 ; i < N ; i++ ) {
		dbList[i] = db ;
		db += dbInc ;
		if ( db < dbMin || db > dbMax ) {
			dbInc *= -1 ;
			if ( db < dbMin )
				db = dbMin ;
			else 
				db = dbMax ;
		}
	}

	durMin = 1 ;		// create a set of durations
	durMax = 3 ;		// that oscillate between max and min
	durInc = 0.3 ;
	dur = durMin ;

	for ( i = 0 ; i < N ; i++ ) {
		durList[i] = dur ;
		dur += durInc ;
		if ( dur > durMax || dur < durMin ) {
			durInc *= -1 ;
			if ( dur > durMax )
				dur = durMax ;
			else 
				dur = durMin ;
		}
	}

	return ;
}

void showData(double *flist, double *dbList, double *durList, int N)
{
	int i ;
	for ( i = 0 ; i < N ; i++ )
		printf("%2d:  %10.6f  %3g  %3.1f\n", i, *flist++, *dbList++, *durList++) ;
	return ;
}
