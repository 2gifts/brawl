#ifndef GAMESPY_COMMON_DARRAY_H
#define GAMESPY_COMMON_DARRAY_H

// GameSpy SDK dynamic array and hash table (darray.c / hashtable.c). The
// layouts are the SDK's DArrayImplementation and HashImplementation; our
// symbols do not name these functions yet, so the SDK names are noted next to
// each placeholder (HYPOTHESIS: by address order and behavior).
#include <types.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GS_DARRAY_NOT_FOUND (-1)

typedef void (*ArrayElementFreeFn)(void* elem);
typedef int (*ArrayCompareFn)(const void* elem1, const void* elem2);
typedef void (*ArrayMapFn)(void* elem, void* clientData);
typedef int (*ArrayMapFn2)(void* elem, void* clientData);

typedef struct DArrayImplementation {
    int count;                       // 0x0
    int capacity;                    // 0x4
    int elemsize;                    // 0x8
    int growby;                      // 0xC
    ArrayElementFreeFn elemfreefn;   // 0x10
    void* list;                      // 0x14
} DArrayImplementation;
typedef DArrayImplementation* DArray;

typedef int (*TableHashFn)(const void* elem, int numBuckets);
typedef int (*TableCompareFn)(const void* elem1, const void* elem2);
typedef void (*TableElementFreeFn)(void* elem);
typedef void (*TableMapFn)(void* elem, void* clientData);
typedef int (*TableMapFn2)(void* elem, void* clientData);

typedef struct HashImplementation {
    DArray* buckets;                 // 0x0
    int nbuckets;                    // 0x4
    TableElementFreeFn freefn;       // 0x8
    TableHashFn hashfn;              // 0xC
    TableCompareFn compfn;           // 0x10
} HashImplementation;
typedef HashImplementation* HashTable;

__static_assert(sizeof(DArrayImplementation) == 0x18, "DArrayImplementation");
__static_assert(sizeof(HashImplementation) == 0x14, "HashImplementation");

DArray ArrayNew(int elemSize, int numElemsToAllocate, ArrayElementFreeFn elemFreeFn);  // ArrayNew
void ArrayFree(DArray array);                                                          // ArrayFree
int ArrayLength(DArray array);                                                           // ArrayLength
void* ArrayNth(DArray array, int n);                                                  // ArrayNth
void ArrayAppend(DArray array, const void* newElem);                                     // ArrayAppend
void ArrayInsertSorted(DArray array, const void* newElem, ArrayCompareFn comparator);          // ArrayInsertSorted
void ArrayRemoveAt(DArray array, int n);                                                   // ArrayRemoveAt
void ArrayDeleteAt(DArray array, int n);                                                   // ArrayDeleteAt
void ArrayReplaceAt(DArray array, const void* newElem, int n);                              // ArrayReplaceAt
void ArraySort(DArray array, ArrayCompareFn comparator);                               // ArraySort
int ArraySearch(DArray array, const void* key, ArrayCompareFn comparator, int fromIndex, int isSorted); // ArraySearch
void ArrayMapBackwards(DArray array, ArrayMapFn fn, void* clientData);                         // ArrayMapBackwards
void* ArrayMapBackwards2(DArray array, ArrayMapFn2 fn, void* clientData);                       // ArrayMapBackwards2
void ArrayClear(DArray array);                                                          // ArrayClear

HashTable TableNew(int elemSize, int nBuckets, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn); // TableNew
HashTable TableNew2(int elemSize, int nBuckets, int nChains, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn); // TableNew2
void TableFree(HashTable table);                                                       // TableFree
int TableCount(HashTable table);                                                        // TableCount
void TableEnter(HashTable table, const void* newElem);                                  // TableEnter
int TableRemove(HashTable table, const void* delElem);                                   // TableRemove
void* TableLookup(HashTable table, const void* elemKey);                                 // TableLookup
void TableMapSafe(HashTable table, TableMapFn fn, void* clientData);                      // TableMapSafe
void* TableMapSafe2(HashTable table, TableMapFn2 fn, void* clientData);                    // TableMapSafe2

#ifdef __cplusplus
}
#endif

#endif
