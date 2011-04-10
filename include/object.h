#ifndef __CONFIG__OBJECT_H__
#define __CONFIG__OBJECT_H__

#include "config.h"
#include "types.h"
#include "pool.h"

//
// Type checking

#define cfg_is_nil(o)               (((cfg_obj_t *)o)->type == NIL)
#define cfg_is_integer(o)           (((cfg_obj_t *)o)->type == INTEGER)
#define cfg_is_float(o)             (((cfg_obj_t *)o)->type == FLOAT)
#define cfg_is_string(o)            (((cfg_obj_t *)o)->type == STRING)
#define cfg_is_date(o)              (((cfg_obj_t *)o)->type == DATE)
#define cfg_is_datetime(o)          (((cfg_obj_t *)o)->type == DATETIME)
#define cfg_is_integer_vector(o)    (((cfg_obj_t *)o)->type == INTEGER_VECTOR)
#define cfg_is_float_vector(o)      (((cfg_obj_t *)o)->type == FLOAT_VECTOR)
#define cfg_is_array(o)             (((cfg_obj_t *)o)->type == ARRAY)
#define cfg_is_dict(o)              (((cfg_obj_t *)o)->type == DICT)

//
// Values

#define cfg_get_integer(o)          (((cfg_obj_integer_t *)o)->value)
#define cfg_get_float(o)            (((cfg_obj_float_t *)o)->value)
#define cfg_get_string(o)           (((cfg_obj_string_t *)o)->value)
                                    
#define cfg_get_year(o)             (((cfg_obj_date_t *)o)->year)
#define cfg_get_month(o)            (((cfg_obj_date_t *)o)->month)
#define cfg_get_day(o)              (((cfg_obj_date_t *)o)->day)
#define cfg_get_hour(o)             (((cfg_obj_datetime_t *)o)->hour)
#define cfg_get_minute(o)           (((cfg_obj_datetime_t *)o)->minute)
#define cfg_get_second(o)           (((cfg_obj_datetime_t *)o)->second)
#define cfg_get_millisecond(o)      (((cfg_obj_datetime_t *)o)->millisecond)
                                    
#define cfg_get_integer_x(o)        (((cfg_obj_integer_vector_t *)o)->x)
#define cfg_get_integer_y(o)        (((cfg_obj_integer_vector_t *)o)->y)
#define cfg_get_integer_z(o)        (((cfg_obj_integer_vector_t *)o)->z)
                                    
#define cfg_get_float_x(o)          (((cfg_obj_float_vector_t *)o)->x)
#define cfg_get_float_y(o)          (((cfg_obj_float_vector_t *)o)->y)
#define cfg_get_float_z(o)          (((cfg_obj_float_vector_t *)o)->z)

//
// Constructors

void cfg_init_nil(cfg_obj_t *o);

void cfg_init_integer(cfg_obj_integer_t *o, cfg_int_t v);
void cfg_init_float(cfg_obj_float_t *o, cfg_float_t v);
void cfg_init_string(cfg_obj_string_t *o, const char *v);

void cfg_init_date(cfg_obj_date_t *o, int year, int month, int day);
void cfg_init_datetime(cfg_obj_datetime_t *o, int year,
                                                 int month,
                                                 int day,
                                                 int hour,
                                                 int minute,
                                                 int second,
                                                 int millisecond);

void cfg_init_integer_vector(cfg_obj_integer_vector_t *o, cfg_int_t x, cfg_int_t y, cfg_int_t z);
void cfg_init_float_vector(cfg_obj_float_vector_t *o, cfg_float_t x, cfg_float_t y, cfg_float_t z);
                                            
void cfg_init_array(cfg_obj_array_t *o);
void cfg_init_dict(cfg_obj_dict_t *o);

//
// Free

void cfg_object_free(cfg_pool_t *pool, cfg_obj_t *obj);

//
// Array

#define         cfg_array_length(o) (((cfg_obj_array_t *)o)->length)
int             cfg_array_push(cfg_pool_t *pool, void *ary, cfg_obj_t *obj);
void            cfg_array_concat(cfg_pool_t *pool, void *ary1, void *ary2);

#define cfg_array_foreach(array, i, v) \
    struct cfg_object_array_entry *i; \
    cfg_obj_t *v; \
    for (i = ((cfg_obj_array_t *)array)->head, v = (i ? i->value : NULL); i; i = i->next, v = (i ? i->value : NULL))

//
// Dictionary

#define         cfg_dict_length(o) (((cfg_obj_dict_t *)o)->length)
int             cfg_dict_contains_key(void *dict, const char *key);
cfg_obj_t*      cfg_dict_get(void *d, const char *key);
int             cfg_dict_set(cfg_pool_t *pool, void *d, const char *key, cfg_obj_t *value);

#define cfg_dict_foreach_pair(dict, i, k, v) \
    struct cfg_object_dict_entry *i; \
    const char *k; \
    cfg_obj_t *v; \
    for (i = ((cfg_obj_dict_t *)dict)->head, k = (i ? i->key : NULL), v = (i ? i->value : NULL); i; i = i->next, k = (i ? i->key : NULL), v = (i ? i->value : NULL))

#define cfg_dict_foreach_key(dict, i, k) \
    struct cfg_object_dict_entry *i; \
    const char *k; \
    for (i = ((cfg_obj_dict_t *)dict)->head, k = (i ? i->key : NULL); i; i = i->next, k = (i ? i->key : NULL))

#define cfg_dict_foreach_value(dict, i, v) \
    struct cfg_object_dict_entry *i; \
    cfg_obj_t *v; \
    for (i = ((cfg_obj_dict_t *)dict)->head, v = (i ? i->value : NULL); i; i = i->next, v = (i ? i->value : NULL))

#endif