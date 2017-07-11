#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cm.h>

double *fmParallel(double carrier, double mod1, double I1, double mod2, double I2, double dur, doubledB)
{
	double C, M1, M2 ;
	double cInc, m1Inc, m2Inc, amp ;
	double *s, *sound ;
	int samples ;
	double twopi = 2 * M_PI ;
	double T[] = {0.0, 0.1, 0.25, 0.72, 1.0} ;
	double A[] = {0.0, 1.0, 0.75, 0.50, 0.0} ;

