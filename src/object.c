#include "object.h"
#include "util.h"

void cfg_init_nil(cfg_obj_t *o) {
	o->type = NIL;
}

void cfg_init_integer(cfg_obj_integer_t *o, cfg_int_t v) {
	o->type.type = INTEGER;
	o->value = v;
}

void cfg_init_float(cfg_obj_float_t *o, cfg_float_t v) {
	o->type.type = FLOAT;
	o->value = v;
}

void cfg_init_string(cfg_obj_string_t *o, const char *v) {
	o->type.type = STRING;
	o->value = v;
}

void cfg_init_date(cfg_obj_date_t *o, int year, int month, int day) {
	o->type.type = DATE;
	o->year = year;
	o->month = month;
	o->day = day;
}
void cfg_init_datetime(cfg_obj_datetime_t *o, int year,
	  										   	 int month,
	  										   	 int day,
	  										   	 int hour,
	  										   	 int minute,
	  										   	 int second,
	  										   	 int millisecond) {
	o->date.type.type = DATETIME;
	o->date.year = year;
	o->date.month = month;
	o->date.day = day;
	o->hour = hour;
	o->minute = minute;
	o->second = second;
	o->millisecond = millisecond;
}

void cfg_init_integer_vector(cfg_obj_integer_vector_t *o, cfg_int_t x, cfg_int_t y, cfg_int_t z) {
	o->type.type = INTEGER_VECTOR;
	o->x = x;
	o->y = y;
	o->z = z;
}

void cfg_init_float_vector(cfg_obj_float_vector_t *o, cfg_float_t x, cfg_float_t y, cfg_float_t z) {
	o->type.type = FLOAT_VECTOR;
	o->x = x;
	o->y = y;
	o->z = z;
}
	  										
void cfg_init_array(cfg_obj_array_t *o) {
	o->type.type = ARRAY;
	o->length = 0;
	o->head = NULL;
	o->tail = NULL;
}

void cfg_init_dict(cfg_obj_dict_t *o) {
	o->type.type = DICT;
	o->length = 0;
	o->head = NULL;
	o->tail = NULL;
}

//
// Fre

static void cfg_object_do_free(cfg_pool_t *pool, cfg_obj_t *obj) {
	// TODO: deallocate everything
}

void cfg_object_free(cfg_pool_t *pool, cfg_obj_t *obj) {
	if (pool->free) {
		cfg_object_do_free(pool, obj);
	}
}

//
// Arrays

int cfg_array_push(cfg_pool_t *pool, void *ary, cfg_obj_t *obj) {
	cfg_obj_array_t *array = (cfg_obj_array_t *)ary;
	
	struct cfg_object_array_entry *entry = cfg_pool_alloc(pool, sizeof(struct cfg_object_array_entry));
	if (entry == NULL) {
		return 0;
	}
	
	entry->value = obj;
	entry->next = NULL;
	
	if (!array->head) {
		array->head = entry;
	} else {
		array->tail->next = entry;
	}
	
	array->length += 1;
	array->tail = entry;
	
	return 1;
}

void cfg_array_concat(cfg_pool_t *pool, void *ary1, void *ary2) {
	cfg_array_foreach(ary2, i, v) {
		// FIXME: this should probably copy instead of aliasing the values
		cfg_array_push(pool, ary1, v);
	}
}

//
// Dictionaries

struct cfg_object_dict_entry* cfg_dict_entry_for_key(cfg_obj_dict_t *dict, const char *key) {
	struct cfg_object_dict_entry *tmp = dict->head;
	while (tmp) {
		if (cfg_streq(tmp->key, key)) {
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

int	cfg_dict_contains_key(void *dict, const char *key) {
	struct cfg_object_dict_entry *entry = cfg_dict_entry_for_key((cfg_obj_dict_t *)dict, key);
	return entry != NULL;
}

cfg_obj_t* cfg_dict_get(void *d, const char *key) {
	cfg_obj_dict_t *dict = (cfg_obj_dict_t *)d;
	struct cfg_object_dict_entry *entry = cfg_dict_entry_for_key(dict, key);
	return entry ? entry->value : NULL;
}

int cfg_dict_set(cfg_pool_t *pool, void *d, const char *key, cfg_obj_t *value) {
	cfg_obj_dict_t *dict = (cfg_obj_dict_t *)d;
	
	struct cfg_object_dict_entry *entry = cfg_dict_entry_for_key(dict, key);
	if (entry) {
		entry->value = value;
		return 1;
	} else {
		struct cfg_object_dict_entry *entry = cfg_pool_alloc(pool, sizeof(struct cfg_object_dict_entry));
		if (entry == NULL) {
			return 0;
		}
		
		entry->key = key;
		entry->value = value;
		entry->next = NULL;
		
		if (!dict->head) {
			dict->head = entry;
		} else {
			dict->tail->next = entry;
		}
		
		dict->length += 1;
		dict->tail = entry;
		
		return 1;
	}
}