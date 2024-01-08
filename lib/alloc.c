#include <inc/lib.h>

static Heap_Chunk Chunks[MAX_HEAP_CHUNKS];
static size_t IN_USE = 0;
static physaddr_t heap_addr = USER_HEAP_START;


void *
malloc(size_t size) {
    if (!size || IN_USE >= MAX_HEAP_CHUNKS ||
    	USER_HEAP_END <= heap_addr + size) {
   		return NULL;
   	}

    if (IN_USE > 0) {
    	int best_idx = -1;
    	size_t best_size = 0;
    	
    	for (size_t i = 0; i < IN_USE; ++i) {
    		if (!(Chunks[i].used) && Chunks[i].size >= size &&
    			(best_idx == -1 || best_size > Chunks[i].size)) {
    			
    			best_idx = i;
    			best_size = Chunks[i].size;
			} 
    	}
    	if (best_idx != -1) {
    		Chunks[i].used = true;
    		return (void *) Chunks[best_idx].addr;
    	}
    }
    // Not enough allocated memory

    int res = sys_alloc_region(sys_getenvid(), (void *) heap_addr, 
    				ROUNDUP(size, PAGE_SIZE), PROT_R | PROT_W | PROT_USER_);
    if (res < 0) return NULL;
        
    Chunks[IN_USE].used = true;
    Chunks[IN_USE].size = ROUNDUP(size, PAGE_SIZE);
    Chunks[IN_USE].addr = heap_addr;
    
    heap_addr += ROUNDUP(size, PAGE_SIZE);

    return (void*) (Chunks[IN_USE++].addr);
}


void *
calloc(size_t nmemb, size_t size) {
	void *address = malloc(nmemb * size);
	memset(address, 0, nmemb * size);
	return address;
}

void
free(void *ptr) {
	if (!ptr) return;
	
	for (size_t i = 0; i < IN_USE; ++i) {

    	if ((physaddr_t) ptr == Chunks[i].addr) {
    		Chunks[i].used = false;
    		return;
    	}
    }
    panic("Trying to free non-allocated memory.\n"); 
} 
