/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

void access_pattern1( char *data, int length ) //secuencial
{
	for (int i = 0; i < length; i++)
	{
		data[i] = 0;
	}
}

void access_pattern2( char *data, int length ) //random
{
	for(int i = 0;i<length;i++)
	{
		long valor = lrand48()%length;
		data[valor] = 0;
	}
}

void access_pattern3( char *data, int length ) //random seguido por 5 números
{
	long valorrand = lrand48()%length;
	int contador=0;
	for(int i = 0;i<length;i++)
	{
		if (contador==4)
		{
			valorrand = lrand48()%length;
			contador=0;
		}
		else
		{
			valorrand++;
			valorrand = valorrand%length;
			contador++;
		}
		
		data[valorrand] = 0;
	}
}
