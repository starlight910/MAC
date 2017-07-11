#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DATA {	// a new variable: DATA
	int index ;
	char *name ;
	struct DATA *next ;	// pointer to the next item in the linked list
} DATA ;


void addToList(DATA **dataList, DATA *new)
{
	DATA *d ;

	if ( *dataList == NULL ) {		// first time
		*dataList = new ;
		new->next = NULL ;
		return ;
	}

	for ( d = *dataList ; d->next != NULL ; d = d->next )
		;

	d->next = new ;
	new->next = NULL ;

	return ;
}

DATA *initData(int index, char *name)
{
	DATA *new ;

	new = (DATA *)malloc(sizeof(DATA)) ;

	new->index = index ;
	new->name = (char *)malloc(strlen(name) + 1) ;
	strcpy(new->name, name) ;

	return new ;
}

void showList(DATA *dataList)
{
	DATA *d ;
	for ( d = dataList ; d != NULL ; d = d->next ) 
		printf("d->index = %d  d->name = %s\n", d->index, d->name) ;
	return  ;
}

void show(DATA *d)
{
	fprintf(stderr,"\tindex: %d  name: %s\n", d->index, d->name) ;
	return ;
}

DATA *searchIndex(DATA *dataList, int index)
{
	DATA *d ;

	for ( d = dataList ; d != NULL ; d = d->next )
		if ( d->index == index )
			break ;

	if ( d == NULL ) 
		fprintf(stderr,"Index %d not in list\n", index) ;

	return d ;
}

int main(void)
{
	int i, index = 0 ;
	DATA *d ;
	DATA *dataList = NULL ;

	d = initData(index++, "Arun") ; 
	addToList(&dataList, d) ;

	d = initData(index++, "Lori") ; 
	addToList(&dataList, d) ;

	d = initData(index++, "Rian") ; 
	addToList(&dataList, d) ;

	d = initData(index++, "Alina") ; 
	addToList(&dataList, d) ;

	d = initData(index++, "Anna") ; 
	addToList(&dataList, d) ;

	showList(dataList) ;

	for ( i = index ; i >= 0 ; i-- ) {
		d = searchIndex(dataList, i) ;
		if ( d != NULL )
			show(d) ;
	}

	return 0 ;
}
