
#include "Vector.h"

X_VectorPtr
X_Vector_New(unsigned int ItemSize, unsigned int InitCapacity, X_Vector_MemFree_t Free)
{
    X_VectorPtr pVec = (X_VectorPtr)malloc(sizeof(X_Vector));

    if (pVec != NULL)
    {
        pVec->ItemSize = ItemSize;
        pVec->Capacity = 0;
        pVec->Size = 0;
        pVec->Data = NULL;
        pVec->Free = Free;

        if (InitCapacity > 0)
        {
            if (0 != X_Vector_Grow(pVec, InitCapacity))
            {
                X_Vector_Free(pVec);
                return NULL;
            }
        }
    }
    return pVec;
}

void
X_Vector_Free(X_VectorPtr pVec)
{
    unsigned int i;

    if (pVec != NULL)
    {
        if (pVec->Free != NULL && pVec->Data != NULL)
        {
            for(i = 0; i < pVec->Size; ++i)
            {
                pVec->Free(&pVec->Data[i * pVec->ItemSize]);
            }
        }

        if (pVec->Data != NULL)
            free(pVec->Data);

        free(pVec);
    }
}

void
X_Vector_Clear(X_VectorPtr pVector)
{
    int i;

    if (pVector == NULL)
        return;

    if (pVector->Free != NULL && pVector->Data != NULL)
    {
        for(i = 0; i < pVector->Size; ++i)
        {
            pVector->Free(&pVector->Data[i * pVector->ItemSize]);
        }
    }

    pVector->Size = 0;
}

int
X_Vector_Append(X_VectorPtr pVec, void *Object)
{
    if (pVec == NULL || Object == NULL)
        return -1;

    if (pVec->Size == pVec->Capacity)
    {
        if (0 != X_Vector_Grow(pVec, 1))
            return -1;
    }

    memcpy(&pVec->Data[pVec->Size * pVec->ItemSize], Object, pVec->ItemSize);

    pVec->Size++;

    return 0;
}

int
X_Vector_Get(X_VectorPtr Vec, unsigned int Index, void **Ptr)
{
    if (Vec == NULL || Ptr == NULL)
        return -1;

    if (Index >= Vec->Size)
        return -1;

    memcpy(*Ptr, &Vec->Data[Index * Vec->ItemSize], Vec->ItemSize);

    return 0;
}

void*
X_Vector_At(X_VectorPtr Vec, unsigned int Index)
{
    if (Vec == NULL)
        return NULL;

    if (Index >= Vec->Size)
        return NULL;

    return &Vec->Data[Index * Vec->ItemSize];
}

int
X_Vector_Grow(X_VectorPtr pVec, unsigned int Grow)
{
    char *NewData;

    if (pVec == NULL || Grow == 0)
        return -1;

    NewData = (char*)malloc((pVec->Capacity + Grow)*pVec->ItemSize);

    if (NewData == NULL)
        return -1;

    if (pVec->Data != NULL)
    {
        memcpy(NewData, pVec->Data, pVec->Size * pVec->ItemSize);
        free (pVec->Data);
    }

    pVec->Data = NewData;

    pVec->Capacity += Grow;

    return 0;
}
