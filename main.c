/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

const char* algoritmo;

int nframes;
char *physmem;
struct disk *disk = NULL;

//Contadores
int cantidad_falta_de_paginas = 0;
int cantidad_lectura_de_disco = 0;
int cantidad_escritura_en_disco = 0;

//estructura que representa un marco
typedef struct
{
	int numero;
	int bit;
}marco;

//lista de marcos
marco* marcos;

//declaro la funcion reemplazo_rand para la funcion page_fault_handler, esta funcion sera definida al final del codigo
void reemplazo_rand( struct page_table *pt, int page);
//declaro la funcion FIFO para la funcion page_fault_handler, esta funcion sera definida al final del codigo
void FIFO( struct page_table *pt, int page);

void page_fault_handler( struct page_table *pt, int page)
{
	cantidad_falta_de_paginas++;
	printf("\npage fault on page #%d",page);
	if(strcmp(algoritmo,"rand")==0)
	{
		//printf("\nejecutar rand");
		reemplazo_rand(pt,page);
	}
	else if (strcmp(algoritmo,"FIFO"))
	{
		//printf("\nejecutar FIFO");
	}
	else
	{
		printf("\ningrese un algoritmo valido, programa abortado");
		exit(1);
	}
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("\nuse: virtmem <npages> <nframes> <lru|fifo> <access pattern>\n");
		return 1;
	}
	
	int npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	const char *program = argv[4];
	algoritmo = argv[3];
	
	marcos = malloc(nframes*sizeof(marco));

	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

	physmem = page_table_get_physmem(pt);

	if(!strcmp(program,"pattern1")) {
		access_pattern1(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"pattern2")) {
		access_pattern2(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"pattern3")) {
		access_pattern3(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n",argv[4]);

	}

	page_table_delete(pt);
	disk_close(disk);

	printf("Cantidad de falta de paginas: %d\n", cantidad_falta_de_paginas);
	printf("Cantidad de lecturas a disco: %d\n", cantidad_lectura_de_disco);
	printf("Cantidad de escrituras a disco: %d\n", cantidad_escritura_en_disco);
	
	return 0;
}

// algoritmo RAND
void reemplazo_rand( struct page_table *pt, int page)
{
	//obtenemos el marco y los bits por la funcion entregada en el enunciado
	int frame,bits;
	page_table_get_entry(pt,page,&frame,&bits);
	//printf("\nframe:%d\nbits:%d",frame,bits);
		
	if(bits==0)//si los bits son 0 la pagina no esta cargada en memoria, por ende tendremos que cargarla
	{
		printf("\npagina no cargada en memoria");
		srand(time(0));
		int randframe = (rand() % nframes);//frame al azar al cual se le borrara su pagina
		int disponible = -1;//variable por la cual veremos si hay algun frame disponible
		for(int i = 0; i<nframes; i++)//recorremos los marcos
		{
			if(marcos[i].bit == 0)//si el bit del marco i es 0, esta disponible
			{
				disponible = i;
				break;
			}
		}
		
		if(disponible == -1)//si no hay frames disponibles
		{
			printf("\nno hay frames disponibles");
			bits = PROT_READ;//el marco tendra bits de proteccion de lectura
			
			//procedemos a sobreescribir las pagina random
			if(marcos[randframe].bit & PROT_WRITE)// si el frame tiene un bit de escritura, escribimos al disco
			{
				disk_write(disk, marcos[randframe].numero, &physmem[randframe*PAGE_SIZE]);
			}
			marcos[randframe].bit=bits;//ponemos el bit del marco random igual que la variable bits
			marcos[randframe].numero = page;
			//sobreescribimos la nueva pagina con los respectivos parametros
			page_table_set_entry(pt,page,randframe,bits);
			disk_read(disk,page, &physmem[randframe*PAGE_SIZE]);//leemos del disco al marco random	
			cantidad_lectura_de_disco++;	
		}
		else
		{
			printf("\nframe disponible");
			bits = PROT_READ;//el marco tendra bits de proteccion de lectura
			disk_read(disk,page, &physmem[disponible*PAGE_SIZE]);//leemos del disco al marco disponible
			marcos[disponible].bit=bits;//ponemos el bit del marco random igual que la variable bits
			marcos[disponible].numero = page;
			//sobreescribimos la nueva pagina con los respectivos parametros
			page_table_set_entry(pt,page,disponible,bits);
		}
		
	}
	else if(bits != 0)//pagina cargada en memoria
	{
		printf("\npagina ya cargada en memoria");
		//si la pagina ya esta cargada, significa que ahora se requiere de operaicones de escritura
		bits = PROT_READ | PROT_WRITE;//le damos permisos de lectura y escritura
		//utilizamos de indice para el array de marcos el mismo frame entregado y actualizamos los valores
		marcos[frame].numero = page;
		marcos[frame].bit = bits;
		page_table_set_entry(pt,page,frame,bits);
	}
}

//algoritmo FIFO		FALTA AGREGARLE CONTADOR DE LECTURA A DISCO
void FIFO( struct page_table *pt, int page)
{
	//obtenemos el marco y los bits por la funcion entregada en el enunciado
	int frame,bits;
	page_table_get_entry(pt,page,&frame,&bits);
	//printf("\nframe:%d\nbits:%d",frame,bits);
	
	if(bits==0)//si los bits son 0 la pagina no esta cargada en memoria, por ende tendremos que cargarla
	{
	
	}
	else if(bits != 0)//pagina cargada en memoria
	{
		printf("\npagina ya cargada en memoria");
		//si la pagina ya esta cargada, significa que ahora se requiere de operaicones de escritura
		bits = PROT_READ | PROT_WRITE;//le damos permisos de lectura y escritura
		//utilizamos de indice para el array de marcos el mismo frame entregado y actualizamos los valores
		marcos[frame].numero = page;
		marcos[frame].bit = bits;
		page_table_set_entry(pt,page,frame,bits);
	}
}
