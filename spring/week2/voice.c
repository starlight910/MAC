#include <stdio.h>
#include <stlib.h>
#include <string.h>
#include <math.h>
#include <cm.h>

void makeSeq(double *out, int start, int samples, double *in)
{
	out += start ;
	while (samples-- )
		*out++ = *in++ ;
	return ;
}

double **makeSoundArray(double dur, int nchan)
{
	}

typedef struct grainStruct {
	int index ;
	double startTime, duration ;
	int density ;
	double F0, F1 ;
	double DB0, DB1 ;
	int maxD, minD ;
	int type ;
	int seed ;
	int c0, c1 ;
	struct grainStruct *next ;
} grainStruct ;

void addToList(grainStruct **gList, grainStruct *new)
{
	grainStruct *g ;

	if (*gList ==NULL)
	{
		*gList = new ; 
		new->next = NULL ;
		return ;
	}

	for (g=*gList ; g->next != NULL ; g = g->next) ;

	g->next = new ;
	new->next = NULL ;

	return ;
}

void showList(grainStruct *gList)
{
	grainStruct *g ;

	for (g=gList ; g != NULL ; g=g->next)
	{

int main
