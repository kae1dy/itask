#include "inc/lib.h"

#define USER_HEAP_START 0x00800000
#define USER_HEAP_END (USER_HEAP_START - HUGE_PAGE_SIZE * 256)

physaddr_t heap_address = USER_HEAP_START;

typedef struct {
	size_t size;
	bool used;
} Heap_Chunk;

void *
malloc(size_t size) {
	physaddr_t address = heap_address - size - sizeof(Heap_Chunk);
	assert(address >= USER_HEAP_END);
	
	const Heap_Chunk chunk = {
		.size = size;
		.used = true;
	}
	*(Heap_Chunk *) address = chunk; 
	heap_address = address;
	return address + sizeof(Heap_Chunk);
}

void *
calloc(size_t nmemb, size_t size) {
	void *address = malloc(nmemb * size);
	memset(address, 0, nmemb * size);
	return address;
}

void
free(void * ptr) {
	if (!ptr) return;
	
	Heap_Chunk *chunk = (Heap_Chunk *) ptr;
	chunk->used = false;
} 
