#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>

void readBinary(char *name, double **flist, double **dblist, double **durlist, int *N)
{
	FILE *f ;
	struct	{
		double f ;
		double db ;
		double dur ;
	} data ;
	double *pf, *pdb, *pdur ;
	int nItems, R ;

	if (( f = fopen(name, "rb")) == NULL )
	{
		printf("Can't open %s for binary read\n", name ) ;
		exit(EXIT_FAILURE) ;
	}

	nItems = 0 ;
	while (1)
	{
		R = fread(&data, sizeof(data), 1, f) ;
		if(R==0)
			break ;
		nItems++ ;
	}
	*N = nItems ;

	printf("nItems = %d\n", nItems) ;

	rewind(f) ;

	pf = *flist = (double *)calloc(nItems, sizeof(double)) ;
	pdb = *dblist = (double *)calloc(nItems, sizeof(double)) ;
	pdur = *durlist = (double *)calloc(nItems, sizeof(double)) ;

	while (1)
	{
		R = fread(&data, sizeof(data), 1, f) ;
		if(R==0)
			break ;
		*pf++ = data.f ;
		*pdb++ = data.db ;
		*pdur++ = data.dur ;
	}
	fclose(f) ;
	return ;
}

void writeBinary(char *name, double *flist, double *dblist, double *durlist, int N)
{
	struct
	{
		double f ;
		double db ;
		double dur ;
	} data ;
	FILE *f ;
	int i ;

	if (( f = fopen(name, "wb"))==NULL)
	{
		printf("Can't open %s for binary write\n", name) ;
		exit(EXIT_SUCCESS) ;
	}

	for ( i = 0 ; i < N ; i++)
	{
		data.f = *flist++ ;
		data.db = *dblist++ ;
		data.dur = *durlist++ ;
		fwrite(&data, sizeof(data), 1, f) ;
	}
	fclose(f) ;

	return ;
}

int main(void)
{
	int N = 40 ;
	double *flist, *dbList, *durList ;
	char *filename = "arunB.dat" ;
	void createData(double *flist, double *dbList, double *durList, int N) ;
	void showData(double *flist, double dbList, double durList, int N) ;

	flist = (double *)calloc(N, sizeof(double)) ;
	dbList = (double *)calloc(N,sizeof(double)) ;
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
