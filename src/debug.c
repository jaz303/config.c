#include "debug.h"

#ifndef CONFIG__USE_STDLIB
	#error "Compiling with debug support requires the C standard library"
#else
	#include <stdio.h>
#endif

#define DEBUG_INDENT if (indent) printf("%*c", indent, ' ')

void cfg_debug_print2(cfg_obj_t *o, int indent) {
	if (cfg_is_nil(o)) {
		printf("nil");
	} else if (cfg_is_integer(o)) {
		printf("%d", cfg_get_integer(o));
	} else if (cfg_is_float(o)) {
		printf("%f", cfg_get_float(o));
	} else if (cfg_is_string(o)) {
		printf("\"%s\"", cfg_get_string(o));
	} else if (cfg_is_date(o)) {
		printf("@%04d-%02d-%02d", cfg_get_year(o),
								  cfg_get_month(o),
 								  cfg_get_day(o));
	} else if (cfg_is_datetime(o)) {
		printf("@%04d-%02d-%02dT%02d:%02d:%02d.%03d", cfg_get_year(o),
								 					  cfg_get_month(o),
 								 					  cfg_get_day(o),
													  cfg_get_hour(o),
													  cfg_get_minute(o),
													  cfg_get_second(o),
													  cfg_get_millisecond(o));
	} else if (cfg_is_integer_vector(o)) {
		printf("%%(%d,%d,%d)", cfg_get_integer_x(o), cfg_get_integer_y(o), cfg_get_integer_z(o));
	} else if (cfg_is_float_vector(o)) {
		printf("(%f,%f,%f)", cfg_get_float_x(o), cfg_get_float_y(o), cfg_get_float_z(o));
	} else if (cfg_is_dict(o)) {
		int count = 0;
		cfg_dict_foreach_pair(o, i, k, v) {
			if (count++) printf(",\n");
			DEBUG_INDENT;
			printf("%s: ", k);
			if (cfg_is_dict(v)) {
				printf("{\n");
				cfg_debug_print2(v, indent + 2);
				DEBUG_INDENT;
				putc('}', stdout);
			} else if (cfg_is_array(v)) {
				printf("[\n");
				cfg_debug_print2(v, indent + 2);
				DEBUG_INDENT;
				putc(']', stdout);
			} else {
				cfg_debug_print2(v, indent + 2);
			}
		}
		if (count) putc('\n', stdout);
	} else if (cfg_is_array(o)) {
		int count = 0;
		cfg_array_foreach(o, i, v) {
			if (count++) printf(",\n");
			DEBUG_INDENT;
			if (cfg_is_dict(v)) {
				printf("{\n");
				cfg_debug_print2(v, indent + 2);
				DEBUG_INDENT;
				putc('}', stdout);
			} else if (cfg_is_array(v)) {
				printf("[\n");
				cfg_debug_print2(v, indent + 2);
				DEBUG_INDENT;
				putc(']', stdout);
			} else {
				cfg_debug_print2(v, indent + 2);
			}
		}
		if (count) putc('\n', stdout);
	}
}

void cfg_debug_print(cfg_obj_t *o) {
	cfg_debug_print2(o, 0);
}