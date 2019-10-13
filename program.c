/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

static int compare_bytes( const void *pa, const void *pb )
{
	int a = *(char*)pa;
	int b = *(char*)pb;

	if(a<b) {
		return -1;
	} else if(a==b) {
		return 0;
	} else {
		return 1;
	}

}

void access_pattern1( char *data, int length ) //sequencial
{
	for (int i = 0; i < length; i++)
	{
		data[i] = 0;
	}
	// TODO: Implementar
}

void access_pattern2( char *data, int length ) //random
{
	for(int i = 0;i<length;i++)
	{
		long valor = lrand48()%length;
		data[valor] = 0;
	}
	// TODO: Implementar
}

void access_pattern3( char *cdata, int length ) //reunion
{
	// TODO: Implementar
}
