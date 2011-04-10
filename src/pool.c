#include "pool.h"

void cfg_pool_init_with_pool(cfg_pool_t *pool, cfg_size_t size, void *memory) {
	pool->size		= size;
	pool->allocated	= 0;
	pool->pool		= memory;
	pool->alloc		= NULL;
	pool->free		= NULL;
}

void cfg_pool_init_with_alloc(cfg_pool_t *pool, cfg_alloc_f alloc, cfg_free_f free) {
	pool->size		= 0;
	pool->allocated	= 0;
	pool->pool		= NULL;
	pool->alloc		= alloc;
	pool->free		= free;
}

void *cfg_pool_alloc(cfg_pool_t *pool, cfg_size_t size) {
	if (pool->alloc) {
		return pool->alloc(size);
	} else {
		// TODO: round size up to nearest x-byte
		void *ptr = pool->pool + pool->allocated;
		pool->allocated += size;
		if (pool->allocated > pool->size) {
			pool->allocated -= size;
			return NULL;
		}
		return ptr;
	}
}

void cfg_pool_free(cfg_pool_t *pool, void *thing) {
	if (pool->free) {
		pool->free(thing);
	}
}
