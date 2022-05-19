#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "heap.h"

typedef struct nodo{
   void* data;
   int priority;
}heapElem;

typedef struct Heap{
  heapElem* heapArray;
  int size;
  int capac;
} Heap;


void* heap_top(Heap* pq){
   if(pq->size == 0) return NULL;
   return (void*)pq->heapArray[0].data;
}



void heap_push(Heap* pq, void* data, int priority){
  if(pq->size == pq->capac){
      pq->capac = (pq->capac*2)+1;
      pq->heapArray = (heapElem*)realloc(pq->heapArray,sizeof(heapElem)*pq->capac);
   }

   pq->heapArray[pq->size].data = data;
   pq->heapArray[pq->size].priority = priority;

   int posInf = pq->size;
   int posSup = (pq->size - 1)/2;

   heapElem* elemAux = (heapElem*)malloc(sizeof(heapElem));

   while(posInf > 0){
      if(pq->heapArray[posInf].priority > pq->heapArray[posSup].priority){
         *elemAux = pq->heapArray[posSup];
         pq->heapArray[posSup] = pq->heapArray[posInf];
         pq->heapArray[posInf] = *elemAux;
      }

      posInf = posSup;
      posSup = (posInf - 1)/2;
   }

   pq->size++;
}


void heap_pop(Heap* pq){
   heapElem* elemAux = (heapElem*)malloc(sizeof(heapElem));

   pq->heapArray[0] = pq->heapArray[pq->size-1];
   pq->heapArray[pq->size-1].priority = 0;
   pq->size--;

   int posSup = 0;
   int posInfIzq = (2*posSup)+1;
   int posInfDer = (2*posSup)+2; 

   while(posSup < pq->size){
      if(pq->heapArray[posSup].priority <= pq->heapArray[posInfIzq].priority && pq->heapArray[posInfIzq].priority > pq->heapArray[posInfDer].priority){
         *elemAux = pq->heapArray[posSup];
         pq->heapArray[posSup] = pq->heapArray[posInfIzq];
         pq->heapArray[posInfIzq] = *elemAux;

         posSup = posInfIzq;
         posInfIzq = (2*posSup)+1;
         posInfDer = (2*posSup)+2; 
         continue;

      }else if(pq->heapArray[posSup].priority <= pq->heapArray[posInfDer].priority){
         *elemAux = pq->heapArray[posSup];
         pq->heapArray[posSup] = pq->heapArray[posInfDer];
         pq->heapArray[posInfDer] = *elemAux;

         posSup = posInfDer;
         posInfIzq = (2*posSup)+1;
         posInfDer = (2*posSup)+2; 
         continue;
      } else break;


   }
   
}

Heap* createHeap(){
   Heap* aux = (Heap*)malloc(sizeof(Heap));
   aux->capac=3;
   aux->size=0;
   
   aux->heapArray= (heapElem*)malloc(3*sizeof(heapElem));


   return aux;
}
