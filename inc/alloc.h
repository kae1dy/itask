#define USER_HEAP_START 0x4000000000
#define USER_HEAP_END (USER_HEAP_START + HUGE_PAGE_SIZE * 256)
#define MAX_FREE_HEAP_CHUNK (1 << 10)

typedef struct {
	physaddr_t addr;
	size_t size;
	bool used;
} Heap_Chunk;