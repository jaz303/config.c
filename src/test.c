#include "types.h"
#include "parser.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#define POOL_SIZE 65536

#define TEST_INT(var, val) \
	cfg_obj_integer_t *var = cfg_pool_alloc(&pool, sizeof(cfg_obj_integer_t)); \
	cfg_init_integer(var, val);

char pool_data[POOL_SIZE];

int main(int argc, char *argv[]) {
	
	FILE			*fd;
	unsigned long	size;
	char			*input;
	
	fd = fopen(argv[1], "r");
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	
	input = malloc((sizeof(char) * size) + 1);
	if (!input) {
		return 1;
	}
	
	fseek(fd, 0, SEEK_SET);
	fread(input, 1, size, fd);
	input[size] = '\0';
	
	cfg_pool_t pool;
	cfg_pool_init_with_alloc(&pool, malloc, free);
	
	cfg_parser_t parser;
	cfg_parser_init(&parser, &pool, input);
	
	cfg_obj_dict_t *config;
	if (cfg_parse(&parser, &config)) {
		cfg_debug_print((cfg_obj_t *)config);
	} else {
		printf("Error: %s\n", cfg_parser_get_error(&parser));
	}
	
	return 0;
}