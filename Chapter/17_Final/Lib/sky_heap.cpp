#include "simple_heap.h"


/* A first-fit heap */

static int *array;
static int len;
static bool safe = true;
static int avail = 0;

void makeTaken(int i, int ints);
void makeAvail(int i, int ints);

void Heap::init(void* base, size_t bytes) {
    array = (int*) base;
    len = bytes / 4;
    makeTaken(0,2);
    makeAvail(2,len-4);
    makeTaken(len-2,2);
}

int abs(int x) {
    if (x < 0) return -x; else return x;
}

int size(int i) {
    return abs(array[i]);
}

int headerFromFooter(int i) {
    return i - size(i) + 1;
}

int footerFromHeader(int i) {
    return i + size(i) - 1;
}
    
int sanity(int i) {
    if (safe) {
        if (i == 0) return 0;
        if ((i < 0) || (i >= len)) {
            Debug::panic("bad header index %d\n",i);
            return i;
        }
        int footer = footerFromHeader(i);
        if ((footer < 0) || (footer >= len)) {
            Debug::panic("bad footer index %d\n",footer);
            return i;
        }
        int hv = array[i];
        int fv = array[footer];
  
        if (hv != fv) {
            Debug::panic("bad block at %d, %d != %d\n", i, hv, fv);
            return i;
        }
    }

    return i;
}

int left(int i) {
    return sanity(headerFromFooter(i-1));
}

int right(int i) {
    return sanity(i + size(i));
}

int next(int i) {
    return sanity(array[i+1]);
}

int prev(int i) {
    return sanity(array[i+2]);
}

void next(int i, int x) {
    array[i+1] = x;
}

void prev(int i, int x) {
    array[i+2] = x;
}

void remove(int i) {
    int prevIndex = prev(i);
    int nextIndex = next(i);

    if (prevIndex == 0) {
        /* at head */
        avail = nextIndex;
    } else {
        /* in the middle */
        next(prevIndex,nextIndex);
    }
    if (nextIndex != 0) {
        prev(nextIndex,prevIndex);
    }
}

void makeAvail(int i, int ints) {
    array[i] = ints;
    array[footerFromHeader(i)] = ints;    
    next(i,avail);
    prev(i,0);
    if (avail != 0) {
        prev(avail,i);
    }
    avail = i;
}

void makeTaken(int i, int ints) {
    array[i] = -ints;
    array[footerFromHeader(i)] = -ints;    
}

bool isAvail(int i) {
    return array[i] > 0;
}

bool isTaken(int i) {
    return array[i] < 0;
}
    
extern "C"
void* malloc(size_t bytes) {
    //Debug::printf("malloc(%d)\n",bytes);
    if (bytes == 0) return (void*) array;

    int ints = ((bytes + 3) / 4) + 2;
    if (ints < 4) ints = 4;

    Process::disable();

    int p = avail;
    sanity(p);

    void* res = 0;
    while ((p != 0) && (res == 0)) {
        if (!isAvail(p)) {
            Debug::panic("block @ %d is not available\n",p);
        }
        int sz = size(p);
        if (sz >= ints) {
            remove(p);
            int extra = sz - ints;
            if (extra >= 4) {
                makeTaken(p,ints);
                //Debug::printf("idx = %d, sz = %d, ptr = %p\n",p,ints,&array[p+1]);
                makeAvail(p+ints,extra);
            } else {
                makeTaken(p,sz);
                //Debug::printf("idx = %d, sz = %d, ptr = %p\n",p,sz,&array[p+1]);
            }
            res = &array[p+1];
        } else {
            p = next(p);
        }
    }
    Process::enable();
    if (res == 0) {
        Debug::panic("heap is full, bytes=0x%x",bytes);
    }
    return res;
}        

extern "C"
void free(void* p) {
    if (p == 0) return;
    if (p == (void*) array) return;

    Process::disable();

    int idx = ((((uintptr_t) p) - ((uintptr_t) array)) / 4) - 1;
    sanity(idx);
    if (!isTaken(idx)) {
        Debug::panic("freeing free block %p %d\n",p,idx);
        return;
    }

    int sz = size(idx);

    int leftIndex = left(idx);
    int rightIndex = right(idx);

    if (isAvail(leftIndex)) {
        remove(leftIndex);
        idx = leftIndex;
        sz += size(leftIndex);
    }

    if (isAvail(rightIndex)) {
        remove(rightIndex);
        sz += size(rightIndex);
    }

    makeAvail(idx,sz);
    Process::enable();
}
    
/*****************/
/* C++ operators */
/*****************/

void* operator new(size_t size) {
    return malloc(size);
}

void operator delete(void* p) {
    return free(p);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete[](void* p) {
    return free(p);
}

