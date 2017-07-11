#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void A1(void)
{
	int i, *p ;
	int N = 5 ;

    /*
     * 
     * print the memory location and value of 'i'
     */

    printf("print the memory location and value of 'i'\n") ;
    for ( i = 0 ; i < N ; i++ ) 
        printf("\t i = %2d\t\t&i = %p\n", i, &i) ;
    printf("Address does not change; value at the address changes\n") ;
    putchar('\n') ;

    /*
     * set 'p' to point to 'i'
     * print contents of 'p' (*p), address contained at 'p' (p) and
     * the address of 'p' (&p)
     */

    printf("Do the same thing with a pointer: p = &i\n") ;

    p = &i ;
    printf("Memory address of 'i': %p\n", &i) ;
	for ( i = 0 ; i < N ; i++ ) 
		printf("\t*p = %2d\t\t p = %p\t\t&p = %p\n", *p, p, &p) ;
    printf("value changes, address held at 'p' doesn't change\n") ;
    printf("\tand address of pointer does not change\n") ;

    return ;
}

void A2(void)
{
    char *array, *p ;
    char i, N = 5 ;

    printf("Allocate memory for 'array', ") ;
    array = (char *)calloc(N, sizeof(char)) ;
    printf("\tarray = %p\n", array) ;

    printf("\tset p = array\n") ;
    i = 1 ;
    for ( p = array ; p < array + N ; p++ ) 
        *p = i++ ;

    for ( p = array ; p < array + N ; p++ ) 
        printf("\t*p = %d\t\tp = %p\t\t&p = %p\n", *p, p, &p) ;
    printf("Content changes (*p), address held changes (p), but\n");
    printf("\taddress of pocharer does NOT change\n") ;

    free(array) ;

    return ;

}

void A3(void)
{
    char **array, **pp, *p ;
    char i, N = 5 ;
    char nchan = 3 ;

    printf("Two dimensional array: **array and **p\n") ;

    array = (char **)calloc(nchan, sizeof(char *)) ;
    for ( pp = array ; pp < array + nchan ; pp++ )
        *pp = (char *)calloc(N, sizeof(char)) ;

    i = 1 ;
    for ( pp = array ; pp < array + nchan ; pp++ ) 
        for ( p = *pp ; p < *pp + N ; p++ )
            *p = i++ ;

    for ( pp = array ; pp < array + nchan ; pp++ ) {
        printf("\tChannel: *pp = %p pp = %p &pp = %p\n", *pp, pp, &pp) ;
        for ( p = *pp ; p < *pp + N ; p++ )
                printf("\t*p = %d\t\tp = %p    &p = %p\n", *p, p, &p) ;
    }

    free(array) ;

    return ;

}

void A4(void)
{
    char **array, **pp ;
    char *p ;
    char i, N = 5 ;
    char nchan = 3 ;

    array = (char **)calloc(nchan, sizeof(char *)) ;
    for ( pp = array ; pp < array + nchan ; pp++ ) {
        *pp = (char *)calloc(N, sizeof(char)) ;
        printf("\t*pp = %p  pp = %p  &pp = %p\n", *pp, pp, &pp) ;
    }

    i = 1 ;
    for ( pp = array ; pp < array + nchan ; pp++ ) {
        for ( p = *pp ; p < *pp + N ; p++ )
            *p = i++ ;
    }

    for ( pp = array ; pp < array + nchan ; pp++ ) {
        printf("\t") ;
        for ( p = *pp ; p < *pp + N ; p++ )
            printf("%3d ", *p) ;
        printf("\n") ;
    }

    printf("\n") ;

    for ( pp = array ; pp < array + nchan ; pp++ ) {
        printf("\t") ;
        for ( p = *pp ; p < *pp + N ; p++ )
            printf("%3d ", *p) ;
        printf("\n") ;
    }

    for ( pp = array ; pp < array + nchan ; pp++ ) {
        printf("\t*pp = %p\n", *pp) ;
        for ( p = *pp ; p < *pp + N ; p++ )
            printf("\t*p = %3d  p = %p  &p = %p\n", *p, p, &p) ;
        printf("\n") ;
    }

    return ;

}

int main(void)
{
    A4() ;
	return 0 ;
}
