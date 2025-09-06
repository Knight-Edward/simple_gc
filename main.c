#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

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
static header_t *usedp;         // Points to first used block of memory
static uintptr_t stack_bottom;  // Address of the bottom of the stack

// Add a block of memory to the free list
static void add_to_free_list(header_t *bp) {
  // When user calls free, we add the block to the free list
  // The free list is circularly linked

  header_t *p = freep;
  for (;; p = p->next) {
    // Find the correct place to insert the block
    if (p <= bp && bp <= p->next) {
      break; // Found the correct place
    }
    if (p == freep) {
      break; // Went around the list
    }
    // Now p points to the block before the correct place
  }
  // TODO: Coalesce adjacent free blocks
  header_t *next = p->next;
  p->next = bp;
  bp->next = next;
  freep = p; // Update freep to point to the new block
}

// Request more memory from the system
// use sbrk
static header_t *morecore(unsigned int nu) {
  // TODO: make sure nu is at least 1024 bytes
  header_t *start_new_memory = (header_t *)sbrk((nu + 1) * sizeof(header_t));
  if (start_new_memory == (header_t *)-1) {
    return NULL; // sbrk failed
  }
  // add the new memory to the free list
  start_new_memory->size = nu;
  add_to_free_list(start_new_memory);
  // return a pointer to the free list
  return freep;
}

/**
 This is the GC version of malloc:
1. Traverse the free list to find a block of memory that is large enough
2. If the block is larger than needed, split it and return the remainder to the
free list
3. If no block is found, request more memory from the system
4. Add the block to the used list
5. Return a pointer to the memory after the header
==============
Example usage:
@code
    int *array = (int *)GC_malloc(sizeof(int) * 10);
    for (int i = 0; i < 10; i++) {
        array[i] = i;
    }
    // User does not call free
    // GC will reclaim the memory when needed
@end
*/
void *GC_malloc(unsigned size) {
  unsigned int nunits;
  header_t *p, *prevp;

  // Calculate the number of header-sized units needed
  nunits = (size + sizeof(header_t) - 1) / sizeof(header_t) + 1;

  if ((prevp = freep) == NULL) { // No free list yet
    base.next = freep = prevp = &base;
    base.size = 0;
  }

  for (p = prevp->next;; prevp = p, p = p->next) {
    if (p->size >= nunits) {   // Found a block large enough
      if (p->size == nunits) { // Exact fit
        // remove a memory block(held by p) from free list
        prevp->next = p->next;
      } else { // Allocate tail end
        p->size -= nunits;
        p += p->size; // Move pointer to the allocated block
        p->size = nunits;
      }
      freep = prevp; // Update freep to point to the previous block

      // memory block pointed to by p is now in use
      // Add it to used list
      p->next = usedp;
      usedp = p;

      return (void *)(p + 1); // Return pointer to memory after header
    }
    if (p == freep) { // Wrapped around free list
      p = morecore(nunits);
      if (p == NULL) {
        return NULL; // No memory left
      }
    }
  }
}

// Scan a region of memory and mark any items in the used list
// mostly stack, BSS
static void scan_region(uintptr_t *sp, uintptr_t *end) {
  // FIXME: why sp is uintptr_t* ?
  header_t *bp;
  for (; sp < end; sp++) {
    uintptr_t val = *sp;
    for (bp = usedp; bp != NULL; bp = bp->next) {
      // Check if val points to a block in the used list
      if ((uintptr_t)(bp + 1) <= val &&
          val < (uintptr_t)(bp + 1) + (bp->size - 1) * sizeof(header_t)) {
        // Mark the block as in use
      }
    }
  }
}

// Scan the heap and mark blocks for references to other unmarked blocks
static void scan_heap(void) {}

// Find the absolute bottom of the stack
void GC_init(void) {
  static int initialized;
  if (initialized) {
    return;
  }
  initialized = 1;
  FILE *statfp = fopen("/proc/self/stat", "r");
  assert(statfp != NULL);
  fscanf(statfp,
         "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d "
         "%*d %*d %*d %*u %*u %*d %*d %*u %lu",
         &stack_bottom);
  fclose(statfp);

  usedp = NULL;
  // Initialize the free list by using the dummy block 'base'
  base.next = freep = &base;
  base.size = 0;
}

// Mark blocks of memory in use and free the ones not in use.
void GC_collect(void) {
  header_t *p, *prevp, *tp;
  uintptr_t stack_top; // used with stack_bottom to scan the stack
  extern char end, etext; // Provided by the linker

  if (usedp == NULL) {
    return; // No used blocks
  }

  // scan the BSS and initialized data segment
  scan_region((uintptr_t *)&etext, (uintptr_t *)&end);

  // scan the stack
  {
    int local_var;
    stack_top = (uintptr_t)&local_var;
  }
  // FIXME: why the cast to uintptr_t* ?
  scan_region((uintptr_t *)stack_top, (uintptr_t *)stack_bottom);

  // scan the heap
  scan_heap(); 

  // Now collect the garbage and free.
  // TODO: implement this 

  // Now collect the garbage and free.
  // TODO: implement this 
}

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
  char *new_memory_start = (char *)sbrk(1024);
  if (new_memory_start == (char *)-1) {
    perror("sbrk failed");
    exit(1);
  }
  char *p = new_memory_start;
  for (int i = 0; i < 1024; i++) {
    p[i] = (char)(i % 256); // Fill with some data
  }
  for (int i = 0; i < 1024; i++) {
    printf("%c ", p[i]);
    if (p[i] != (char)(i % 256)) {
      printf("Memory verification failed at index %d\n", i);
      exit(1);
    }
  }
}
