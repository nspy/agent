
#include "Vector.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int i, j, *p = &j;
    X_VectorPtr IntV;

    IntV = X_Vector_New(sizeof(int), 10, NULL);

    assert(IntV != NULL);
    assert(IntV->Capacity == 10);
    assert(IntV->ItemSize == sizeof(int));
    assert(IntV->Size == 0);
    assert(IntV->Free == NULL);

    for(i = 0; i < 10; ++i)
    {
        X_Vector_Append(IntV, &i);
    }

    assert(IntV->Capacity == 10);
    assert(IntV->ItemSize == sizeof(int));
    assert(IntV->Size == 10);
    assert(IntV->Free == NULL);

    for(i = 0; i < 10; ++i)
    {
        p = (int*)X_Vector_At(IntV, i);
        assert(*p == i);
    }


    X_Vector_Free(IntV);

    IntV = X_Vector_New(sizeof(int), 0, NULL);

    assert(IntV != NULL);
    assert(IntV->Capacity == 0);
    assert(IntV->ItemSize == sizeof(int));
    assert(IntV->Size == 0);
    assert(IntV->Free == NULL);

    for(i = 0; i < 10; ++i)
    {
        X_Vector_Append(IntV, &i);
    }

    assert(IntV->Capacity == 10);
    assert(IntV->ItemSize == sizeof(int));
    assert(IntV->Size == 10);
    assert(IntV->Free == NULL);

    for(i = 0; i < 10; ++i)
    {
        p = (int*)X_Vector_At(IntV, i);
        assert(*p == i);
    }


    X_Vector_Free(IntV);


    return 0;
}
