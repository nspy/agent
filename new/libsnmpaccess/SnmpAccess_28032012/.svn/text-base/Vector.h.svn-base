
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>
#include <string.h>

typedef void (*X_Vector_MemFree_t)(void*);

typedef struct
{
    /** Size of an element **/
    unsigned int                ItemSize;
    /** Total capacity **/
    unsigned int                Capacity;
    /** Number of elements in the vector **/
    unsigned int                Size;
    /** Memory release function **/
    X_Vector_MemFree_t          Free;
    /** Storage **/
    char*                       Data;
} X_Vector;

typedef X_Vector* X_VectorPtr;

X_VectorPtr
X_Vector_New(unsigned int ItemSize, unsigned int InitCapacity, X_Vector_MemFree_t Free);

void
X_Vector_Free(X_VectorPtr);

void
X_Vector_Clear(X_VectorPtr);

int
X_Vector_Append(X_VectorPtr, void*);

int
X_Vector_Get(X_VectorPtr, unsigned int, void**);

void*
X_Vector_At(X_VectorPtr, unsigned int);

int
X_Vector_Grow(X_VectorPtr, unsigned int);


#endif
