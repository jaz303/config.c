#ifndef __CONFIG__POOL_H__
#define __CONFIG__POOL_H__

#include "config.h"
#include "types.h"

void 	cfg_pool_init_with_pool(cfg_pool_t *pool, cfg_size_t size, void *memory);
void	cfg_pool_init_with_alloc(cfg_pool_t *pool, cfg_alloc_f alloc, cfg_free_f free);
void*	cfg_pool_alloc(cfg_pool_t *pool, cfg_size_t size);
void	cfg_pool_free(cfg_pool_t *pool, void *thing);

#endif