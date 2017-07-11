void 8chanpan(int frames, int startframe, double **array2D, int startchan, int endchan, double *sound)
{
	double *p1, *p2, boost ;

	p1 = array2D[startchan%8] ;
	p2 = array2D[endchan%8] ;

	double Ainc = 1.0/(frames-1.0) ;

	double A1 = 1, A2 = 0 ;

	while ( frames-- ){
		boost = 1/sqrt(A1*A1 + A2*A2) ;
		*p1++ += (A1 * boost) * *sound ;
		*p2++ += (A2 * boost) * *sound++ ;
		A1 -= Ainc ;
		A2 += Ainc ;
	}

	return ;
}
