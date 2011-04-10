#ifndef __CONFIG__TYPES_H__
#define __CONFIG__TYPES_H__

#include "config.h"

//
// Alloc/free

typedef void* (*cfg_alloc_f)(cfg_size_t);
typedef void (*cfg_free_f)(void*);

//
// Type IDs

typedef enum cfg_type {
    NIL,
    INTEGER,
    FLOAT,
    STRING,
    DATE,
    DATETIME,
    INTEGER_VECTOR,
    FLOAT_VECTOR,
    ARRAY,
    DICT
} cfg_type_t;

//
// Values

typedef struct cfg_object {
    cfg_type_t              type;
} cfg_obj_t;

typedef struct cfg_object_integer {
    cfg_obj_t               type;
    cfg_int_t               value;
} cfg_obj_integer_t;

typedef struct cfg_object_float {
    cfg_obj_t               type;
    cfg_float_t             value;
} cfg_obj_float_t;

typedef struct cfg_object_string {
    cfg_obj_t               type;
    cfg_len_t               length;
    const char              *value;
} cfg_obj_string_t;

typedef struct cfg_object_date {
    cfg_obj_t               type;
    unsigned short          year;
    unsigned char           month;
    unsigned char           day;
} cfg_obj_date_t;

typedef struct cfg_object_datetime {
    cfg_obj_date_t          date;
    unsigned char           hour;
    unsigned char           minute;
    unsigned char           second;
    unsigned short          millisecond;
} cfg_obj_datetime_t;

typedef struct cfg_object_integer_vector {
    cfg_obj_t               type;
    cfg_int_t               x, y, z;
} cfg_obj_integer_vector_t;

typedef struct cfg_object_float_vector {
    cfg_obj_t               type;
    cfg_float_t             x, y, z;
} cfg_obj_float_vector_t;

struct cfg_object_array_entry;
struct cfg_object_array_entry {
    cfg_obj_t                       *value;
    struct cfg_object_array_entry   *next;
};

typedef struct cfg_object_array {
    cfg_obj_t                       type;
    cfg_len_t                       length;
    struct cfg_object_array_entry   *head;
    struct cfg_object_array_entry   *tail;
} cfg_obj_array_t;

struct cfg_object_dict_entry;
struct cfg_object_dict_entry {
    const char                      *key;
    cfg_obj_t                       *value;
    struct cfg_object_dict_entry    *next;
};

typedef struct cfg_object_dict {
    cfg_obj_t                       type;
    cfg_len_t                       length;
    struct cfg_object_dict_entry    *head;
    struct cfg_object_dict_entry    *tail;
} cfg_obj_dict_t;

//
// Memory Pool

typedef struct cfg_pool {
    cfg_size_t              size;
    cfg_size_t              allocated;
    char                    *pool;
    cfg_alloc_f             alloc;
    cfg_free_f              free;
} cfg_pool_t;

#endif