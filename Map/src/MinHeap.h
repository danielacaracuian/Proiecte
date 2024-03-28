#ifndef HEAP_H_
#define HEAP_H_

#include <stdlib.h>
#include <math.h>

typedef struct
{
    char *u;
    int pos;
    int dist;
} Item;

typedef struct
{
    Item content;
    float prior;
} ItemType;

typedef struct heap
{
    long int capacity;
    long int size;
    ItemType *elem;
} PriQueue, *APriQueue;

int getLeftChild(int i);
APriQueue makeQueue(int capacity);
int getRightChild(int i);
int getParent(int i);
void siftUp(APriQueue h, int idx);
void insert(APriQueue h, ItemType x);
ItemType getMin(APriQueue h);
void siftDown(APriQueue h, int idx);
ItemType removeMin(APriQueue h);

#endif
