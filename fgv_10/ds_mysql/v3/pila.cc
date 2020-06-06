/*
 * pila.c
 *
 *  Created on: 7/2/2015
 *      Author: juan
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pila.h"

void Push(Pila *pila, const char *v)
{
   pNodo nuevo;

   nuevo = (pNodo)malloc(sizeof(tipoNodo));
   nuevo->lon_valor = strlen(v) + 1;
   nuevo->valor = (char *)malloc(nuevo->lon_valor);
   memcpy(nuevo->valor, v, nuevo->lon_valor);
   nuevo->siguiente = *pila;
   *pila = nuevo;
}

void Pop(Pila *pila, char *v)
{
   pNodo nodo;

   nodo = *pila;
   if(nodo)
   {
	   *pila = nodo->siguiente;
	   memcpy(v, nodo->valor, nodo->lon_valor);
	   free(nodo->valor);
	   free(nodo);
   }
   else
	   strcpy(v, "PILA VACIA!!!");
}

int Elementos(Pila *pila)
{
	int n = 0;
	pNodo nodo;

	nodo = *pila;
	if(nodo)
	{
		n++;
		while((nodo = nodo->siguiente))
			n++;
	}
	return n;
}

void Vacia(Pila *pila)
{
   pNodo nodo;

   nodo = *pila;
   while(nodo)
   {
	   *pila = nodo->siguiente;
	   free(nodo->valor);
	   free(nodo);
	   nodo = *pila;
   }
}
