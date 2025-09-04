#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

// header of a block of memory
typedef struct header {
	unsigned int size;
	struct header *next;
} header_t;

/** 
 There are two lists of blocks of memory: free and used
 The free list is a circular linked list: freep
 The used list is a singly linked list: usedp
 The lists are made up of blocks of memory, each with a header
 The header contains the size of the block and a pointer to the next block
*/
static header_t base;
/*
   _________________________________
  | size | next |                   |
  |______|______|___________________|
            |
            v
   ____________________________
  | size | next |              |
  |______|______|______________|
            |
            v
   _____________________
  | size | next |       |
  |______|______|_______|
*/
static header_t *freep = &base; // Points to first free block of memory
static header_t *usedp; // Points to first used block of memory

// Add a block of memory to the free list
static void add_to_free_list(header_t *bp) {
    // When user calls free, we add the block to the free list
}

// Request more memory from the system
// use sbrk
static header_t *morecore(unsigned int nu) {
    header_t *start_new_memory = (header_t *)sbrk(nu * sizeof(header_t));
    if(start_new_memory == (header_t *)-1) {
        return NULL; // sbrk failed
    }
    // add the new memory to the free list
    add_to_free_list(start_new_memory);
    // return a pointer to the free list
    return freep; 
}

// Find a chunk from the free list and put it in the used list
void * GC_malloc(unsigned size) { }

// Scan a region of memory and mark any items in the used list
// mostly stack, BSS
static void scan_region(uintptr_t *sp, uintptr_t *end) { }

// Scan the heap and mark blocks for references to other unmarked blocks
static void scan_heap(void) { }

// Find the absolute bottom of the stack
void GC_init(void) {

}

// Mark blocks of memory in use and free the ones not in use.
void GC_collect(void) { }

void test();
void test_sbrk();

int main() {
    test();
    test_sbrk();
	return 0;
}

void test() {
    int *array = (int *)GC_malloc(sizeof(int) * 10);
    printf("Allocated array at %p\n", array);
}

void test_sbrk() {
    // Request 1024 bytes of memory from the system
    char* new_memory_start = (char*)sbrk(1024);
    if (new_memory_start == (char*)-1) {
        perror("sbrk failed");
        exit(1);
    }
    char* p = new_memory_start;
    for(int i=0; i<1024; i++) {
        p[i] = (char)(i % 256); // Fill with some data
    }
    for(int i=0; i<1024; i++) {
        printf("%c ", p[i]);
        if(p[i] != (char)(i % 256)) {
            printf("Memory verification failed at index %d\n", i);
            exit(1);
        }
    }
}
