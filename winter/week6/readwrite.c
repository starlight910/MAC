#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

void createData(double *fList, double *dbList, double *durList, int N)
{
	double fundamental, cents, K ;
	double dbInc, db, dbMax, dbMin ;
	double durMin, durMax, durInc, dur ;
	int i ;

	fundamental = 440 ;  // create a sequence of frequencies
	cents = 100 ;        // separated by 100 cents
	K = 1200.0 / log10(2.0) ;
	for ( i = 0 ; i < N ; i++ )
		fList[i] = fundamental * pow(10.0, (i*cents)/K) ;


	dbInc = -6 ;   // create a sequence of decibel values
	dbMax = 144 ;  // that oscillate between 144 and 84db
	dbMin = 84 ;
	db = dbMax ;

	for ( i = 0 ; i < N ; i ++ )
	{
		dbList[i] = db ;
		db += dbInc ;
		if ( db < dbMin || db > dbMax )
		{
			dbInc *= -1 ;
			if ( db < dbMin )
				db = dbMin ;
			else
				db = dbMax ;
		}
	}

	durMin = 1 ;   // create a set of durations
	durMax = 3 ;   // that oscillate betwwen max and min
	durInc = 0.3 ;
	dur = durMin ;

	for ( i = 0 ; i < N ; i++ )
	{
		durList[i] = dur ;
		dur += durInc ;
		if ( dur > durMax || dur < durMin )
		{
			durInc *= -1 ;
			if ( dur > durMax )
				dur = durMax ;
			else
				dur = durMin ;
		}
	}

	return ;
}

void readData(char *filename, double **flist, double **dblist, double **durlist, int *N)
{
	FILE *f ;
	int nLines ;
	char *line, string[256] ;
	double *pf, * pdb, *pdur ;
	double freq, db, dur ;

	// open a file for reading, if possible
	if ( ( f = fopen(filename, "r") ) == NULL )
	{
		printf("Can't open %s for reading\n", filename) ;
		exit(EXIT_FAILURE) ;
	}

	// find out how many lines of data are in the file
	nLines = 0 ;
	while ( 1 )
	{
		line = fgets(string, sizeof(string), f) ;  // get one line
		if ( line == NULL )                        // exit loop if NULL
			break ;
		nLines++ ;
	}

	*N = nLines ;     // set number of lines

	rewind(f) ;       // rewind file back to beginning

	// allocate memory for the three lists
	pf = *flist = (double *)calloc(nLines, sizeof(double)) ;
	pdb = *dblist = (double *)calloc(nLines, sizeof(double)) ;
	pdur = *durlist = (double *)calloc(nLines, sizeof(double)) ;

	//read data from the file
	while ( 1 )
	{
		line = fgets(string, sizeof(string), f) ;  //get one line
		if ( line == NULL )                        // exit on NULL
			break ;
		sscanf(line, "%lf %lf %lf", &freq, &db, &dur) ; // parse line
		*pf++ = freq ;
		*pdb++ = db ;
		*pdur++ = dur ;
	}

	fclose(f) ;   // close the file

	return ;
}

void showData(double *flist, double *dbList, double *durList, int N)
{
	int i ;
	for ( i = 0 ; i < N ; i++ )
		printf("%2d: %f %f %f\n", i, *flist++, *dbList++, *durList++) ;
	return ;
}

void writeData(char *name, double *flist, double *dbList, double *durList, int N)
{
	FILE *f ;
	int i ;

	// open file for writing, if possible
	if (( f = fopen(name, "w")) == NULL )
	{
		printf("Can't open %s for writing\n", name) ;
		exit(EXIT_FAILURE) ;
	}

	// write arrays to a file
	for ( i = 0 ; i < N ; i++ )
		fprintf(f, "%f %f %f\n", *flist++, *dbList++, *durList++) ;

	fclose(f) ;

	return ;
}

int main(void)
{
	int N = 40 ;
	double *flist, *dbList, *durList ;
	char *filename = "arun.dat" ;

	flist = (double *)calloc(N, sizeof(double)) ;
	dbList = (double *)calloc(N, sizeof(double)) ;
	durList = (double *)calloc(N, sizeof(double)) ;

	createData(flist, dbList, durList, N) ;

	writeData(filename, flist, dbList, durList, N) ;

	free(flist) ;
	free(dbList) ;
	free(durList) ;

	readData(filename, &flist, &dbList, &durList, &N) ;

	showData(flist, dbList, durList, N) ;

	return 0 ;
}
