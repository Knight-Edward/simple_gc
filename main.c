// header of a block of memory

#include <stdlib.h>
#include <stdint.h>

typedef struct header {
	unsigned int size;
	struct header *next;
} header_t;

static header_t base;
static header_t *freep = &base; // Points to first free block of memory
static header_t *usedp; // Points to first used block of memory

// Add a block of memory to the free list
static void add_to_free_list(header_t *bp) { }

// Request more memory from the system
static header_t *morecore(unsigned int nu) { }

// Find a chunk from the free list and put it in the used list
void * GC_malloc(unsigned size) { }

// Scan a region of memory and mark any items in the used list
// mostly stack, BSS
static void scan_region(uintptr_t *sp, uintptr_t *end) { }

// Scan the heap and mark blocks for references to other unmarked blocks
static void scan_heap(void) { }

// Find the absolute bottom of the stack
void GC_init(void) { }

// Mark blocks of memory in use and free the ones not in use.
void GC_collect(void) { }

int main() {
	return 0;
}
