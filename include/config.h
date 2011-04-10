#ifndef __CONFIG__SYS_H__
#define __CONFIG__SYS_H__

#define CONFIG__DEBUG
#define CONFIG__USE_STDLIB

#ifdef CONFIG__USE_STDLIB
    #include <stdint.h>
    #include <stdlib.h>
#endif

#ifndef NULL
    #define NULL 0
#endif

#ifdef CONFIG__USE_STDLIB
    typedef size_t      cfg_size_t;
#else
    typedef uint32_t    cfg_size_t;
#endif

// Types used to store object data
typedef int32_t     cfg_int_t;
typedef uint32_t    cfg_uint_t;
typedef float       cfg_float_t;

// Length of arrays, dictionaries and strings
typedef uint16_t    cfg_len_t;

#endif