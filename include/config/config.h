#ifndef CONFIG__CONFIG_H
#define CONFIG__CONFIG_H

// #define CONFIG__DEBUG
// #define CONFIG__USE_STDLIB

#ifdef CONFIG__USE_STDLIB
    #include <stdint.h>
    #include <stdlib.h>
#endif

#ifndef NULL
    #define NULL 0
#endif

typedef int                 cfg_int_t;
typedef float               cfg_float_t;
typedef unsigned char[4]    cfg_ip_t;

typedef struct {
    unsigned int            length;
    const char              *string;
}                           cfg_string_t;

typedef struct {
    unsigned short          year;
    unsigned char           month;
    unsigned char           day;
}                           cfg_date_t;

typedef struct {
    unsigned short          year;
    unsigned char           month;
    unsigned char           day;
    unsigned char           hour, minute, second;
}                           cfg_datetime_t;

typedef struct {
    int                     depth;
    int                     state;
    unsigned char           negate;
    int                     accumi, accumf;
    unsigned short          token_len;
}                           config_t;

#endif