#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<cm.h>

void stereoMix(double *stereoOut, int startFrame, int nFrames, double *stereoIn)
{
	int nchan = 2 ;
	stereoOut += startFrame * nchan ;
	while ( nFrames-- ) 
	{
		*stereoOut++ += *stereoIn++ ;
		*stereoOut++ += *stereoIn++ ;
	}
	return ;
}

void mixSound(double *out, int start, int frames, double *in)
{
	out += start ;
			
	while ( frames-- ) 
	{
		*out = *out + *in ;
		out++ ;
		in++ ;									
	}
	return ;
}
