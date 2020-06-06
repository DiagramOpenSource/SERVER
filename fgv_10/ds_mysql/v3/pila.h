/*
 * pila.h
 *
 *  Created on: 7/2/2015
 *      Author: juan
 */

#ifndef STORAGE_DIAGRAM_PILA_H_
#define STORAGE_DIAGRAM_PILA_H_

typedef struct _nodo
{
   char *valor;
   unsigned int lon_valor;
   struct _nodo *siguiente;
} tipoNodo;

typedef tipoNodo *pNodo;
typedef tipoNodo *Pila;

void Push(Pila *l, const char *v);
void Pop(Pila *l, char *v);
void Vacia(Pila *l);
int Elementos(Pila *l);

#endif /* STORAGE_DIAGRAM_PILA_H_ */
