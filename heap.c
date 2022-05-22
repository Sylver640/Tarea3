#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "heap.h"

typedef struct nodo{
   void* data;
   double priority;
}heapElem;

struct heap{
  heapElem* heapArray;
  int size;
  int capac;
};


void* heap_top(Mheap* pq)
{
   if (pq == NULL || pq->heapArray == NULL) return NULL;
   
   if (pq->size != 0)
   {
      return pq->heapArray[0].data;
   }
   return NULL;
}

void swap (Mheap* pq, int pos)
{
  heapElem aux;
  while (pos >= 0)
  {
    if (pq->heapArray[pos].priority > pq->heapArray[(pos-1)/2].priority)
    {
      aux = pq->heapArray[(pos-1)/2];
      pq->heapArray[(pos-1)/2] = pq->heapArray[pos]; 
      pq->heapArray[pos] = aux;
      pos = (pos-1)/2;
    }
    else
    {
      return;
    }
  } 
}

void heap_push(Mheap* pq, void* data, double priority)
{
  if (pq->capac == pq->size)
  {
    pq->capac = (pq->capac * 2) + 1;
    pq->heapArray = (heapElem*) realloc (pq->heapArray, (pq->capac)*sizeof(heapElem));
  }

  int pos = pq->size;
  pq->heapArray[pos].data = data;
  pq->heapArray[pos].priority = priority;
  swap(pq, pos);
  pq->size = pq->size + 1;
}

void heap_pop(Mheap* pq)
{
  pq->size = pq->size - 1; 
  pq->heapArray[0] = pq->heapArray[pq->size];
  int pos = 0;
  heapElem aux; 

  while ((2*pos)+ 2 <= pq->size && (pq->heapArray[(2*pos)+1].priority > pq->heapArray[pos].priority || pq->heapArray[pos].priority < pq->heapArray[(2*pos)+2].priority))
  {
    if (pq->heapArray[(2*pos)+1].priority > pq->heapArray[(2*pos)+2].priority)
    {
      aux =  pq->heapArray[(2*pos)+1];
      pq->heapArray[(2*pos)+1] = pq->heapArray[pos]; 
      pq->heapArray[pos] = aux;
      pos = (2*pos)+1;
    }
    else
    {
      aux = pq->heapArray[(2*pos)+2];
      pq->heapArray[(2*pos)+2] = pq->heapArray[pos]; 
      pq->heapArray[pos] = aux;
      pos = (2*pos)+2;          
    }
  }
}

Mheap* createMheap()
{
   Mheap* pq = (Mheap*) calloc (1, sizeof(Mheap));
   pq->capac = 3;//Capacidad del HEAP
   pq->size = 0;// Total de elementos en el HEAP
   pq->heapArray = (heapElem*) calloc (pq->capac, sizeof(heapElem));
   return pq;
}