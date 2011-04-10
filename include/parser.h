#ifndef __CONFIG__PARSER_H__
#define __CONFIG__PARSER_H__

#include "types.h"
#include "object.h"
#include "pool.h"

typedef struct cfg_parser {
    cfg_pool_t              *pool;
    const char              *input;
    const char              *curr;
    const char              *error;
} cfg_parser_t;

void cfg_parser_init(cfg_parser_t *parser, cfg_pool_t *pool, const char *input);
const char *cfg_parser_get_error(cfg_parser_t *parser);

int cfg_parse(cfg_parser_t *p, cfg_obj_dict_t **out);

#endif