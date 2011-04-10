#include "parser.h"

#define PARSER_FAIL             0
#define PARSER_OK               1

#define PARSER_FUNCTION(return_type, name, ...) \
    static return_type name(cfg_parser_t *p, ##__VA_ARGS__)
    
#define PARSER_ALLOC(variable, type) \
    type* variable = cfg_pool_alloc(p->pool, sizeof(type)); \
    if (variable == NULL) { \
        ERROR("allocation error"); \
    } else {} \
    
#define curr() (*((p)->curr))
#define next() ((p)->curr++)
    
#define is_eof() (curr() == '\0')

#define is_alpha() ((curr() >= 'a' && curr() <= 'z') || (curr() >= 'A' && curr() <= 'Z'))
#define is_ident_start() (curr() == '$' || curr() == '_' || is_alpha())
#define is_ident_rest() (is_ident_start() || is_digit())

#define is_digit() (curr() >= '0' && curr() <= '9')
#define is_number_start() (curr() == '-' || is_digit())
#define is_whitespace() (curr() == ' ' || curr() == '\n' || curr() == '\r' || curr() == '\t')
        
#define skip_whitespace() parser_skip_whitespace(p)

#define SET_ERROR(msg) p->error = msg
#define ERROR(msg) \
    do { \
        SET_ERROR(msg); \
        return PARSER_FAIL; \
    } while(0)

#define PARSER_NUMBER_INT       1
#define PARSER_NUMBER_FLOAT     2

union parsed_number_value {
    cfg_int_t       v_int;
    cfg_float_t     v_float;
};

struct parsed_number {
    int                         type;
    union parsed_number_value   value;
};

PARSER_FUNCTION(void, parser_skip_whitespace);
PARSER_FUNCTION(char*, copy_fixed_string, const char* start, int len);
PARSER_FUNCTION(int, parse_number, struct parsed_number *out);
PARSER_FUNCTION(int, parse_fixed_int, int *out, int len);
PARSER_FUNCTION(int, parse_value, cfg_obj_t **obj);
PARSER_FUNCTION(int, parse_array_body, cfg_obj_array_t *ary);
PARSER_FUNCTION(int, parse_dict_body, cfg_obj_dict_t *dict, int is_outer);

void cfg_parser_init(cfg_parser_t *parser, cfg_pool_t *pool, const char *input) {
    parser->pool    = pool;
    parser->input   = input;
    parser->curr    = input;
}

const char *cfg_parser_get_error(cfg_parser_t *parser) {
    return parser->error;
}

int cfg_parse(cfg_parser_t *p, cfg_obj_dict_t **out) {
    PARSER_ALLOC(dict, cfg_obj_dict_t);
    cfg_init_dict(dict);
    if (!parse_dict_body(p, dict, 1)) {
        cfg_object_free(p->pool, (cfg_obj_t *)dict);
        return PARSER_FAIL;
    } else {
        *out = dict;
        return PARSER_OK;
    }
}

//
// End Public Interface

PARSER_FUNCTION(void, parser_skip_whitespace) {
	do {
		if (curr() == '#') {
			while (curr() != '\n' && curr() != '\r' && curr() != '\0') {
				next();
			}
		} else if (is_whitespace()) {
			next();
			while (is_whitespace()) {
				next();
			}
		} else {
			break;
		}
	} while(1);
}

PARSER_FUNCTION(char*, copy_fixed_string, const char* start, int len) {
    char *out = cfg_pool_alloc(p->pool, len + 1);
    if (out) {
        int i;
        for (i = 0; i < len; i++) out[i] = start[i];
        out[len] = '\0';
    }
    return out;
}

PARSER_FUNCTION(int, parse_number, struct parsed_number *out) {
    // TODO: this function needs to handle:
    // IP addresses
    // hex literals
    // exponents, maybe?
    
    int is_negative = 0, int_part = 0, float_part = 0, float_mul = 1;
    
    if (curr() == '-') {
        is_negative = 1;
        next();
    }
    if (!is_digit()) {
        ERROR("expecting digit");
    }
    
    while (is_digit()) {
        int_part = (int_part * 10) + (curr() - '0');
        next();
    }
    
    if (curr() == '.') {
        next();
        if (!is_digit()) {
            ERROR("expecting digit");
        }
        while (is_digit()) {
            float_part = (float_part * 10) + (curr() - '0');
            float_mul *= 10;
            next();
        }
        out->type = PARSER_NUMBER_FLOAT;
        out->value.v_float = (((float)int_part) + ((float)float_part/float_mul)) * (is_negative ? -1 : 1);
        return PARSER_OK;
    } else {
        out->type = PARSER_NUMBER_INT;
        out->value.v_int = int_part * (is_negative ? -1 : 1);
        return PARSER_OK;
    }
}

PARSER_FUNCTION(int, parse_fixed_int, int *out, int len) {
    *out = 0;
    while (len--) {
        if (!is_digit())
            ERROR("expecting digit");
        *out = (*out * 10) + (curr() - '0');
        next();
    }
    // FIXME: not sure if this belongs here.
    if (is_digit())
        ERROR("trailing digits on fixed-length int");
}

PARSER_FUNCTION(int, parse_array_body, cfg_obj_array_t *ary) {
    int count = 0;
    do {
        if (count > 0 && curr() == ',') next();
        skip_whitespace();
        cfg_obj_t *obj;
        if (!parse_value(p, &obj)) {
            return PARSER_FAIL;
        }
        cfg_array_push(p->pool, ary, obj);
        skip_whitespace();
        count++;
    } while (curr() == ',');
    return PARSER_OK;
}

PARSER_FUNCTION(int, parse_dict_body, cfg_obj_dict_t *dict, int is_outer) {
    int count = 0;
    
    do {
        skip_whitespace();
        
        if (is_outer && is_eof()) {
            return PARSER_OK;
        } else if (curr() == '}') {
            return PARSER_OK;
        } else if (!is_ident_start()) {
            ERROR("expecting start of identifier");
        }
        
        const char *start = p->curr;
        next();
        while (is_ident_rest()) {
            next();
        }
        
        if (!is_whitespace()) {
            ERROR("expecting whitespace after dictionary key");
        }
        
        char        *key    = copy_fixed_string(p, start, p->curr - start);
        cfg_obj_t   *obj;
        if (!parse_value(p, &obj)) {
            cfg_pool_free(p->pool, key);
            return PARSER_FAIL;
        }
        
        cfg_dict_set(p->pool, dict, key, obj);
        
        skip_whitespace();
        if (curr() != ';') {
            ERROR("expecting ';'");
        }
        
        next();
    } while (1);
}

PARSER_FUNCTION(int, parse_value, cfg_obj_t **obj) {
    skip_whitespace();
    switch (curr()) {
		case '"':
		{
			// TODO: parse string
		}
		case 'a'...'z':
		{
			
		}
		case '-':
        case '0'...'9':
        {
            struct parsed_number parsed;
            if (!parse_number(p, &parsed)) {
                return PARSER_FAIL;
            }
            if (parsed.type == PARSER_NUMBER_FLOAT) {
                PARSER_ALLOC(number, cfg_obj_float_t);
                cfg_init_float(number, parsed.value.v_float);
                *obj = (cfg_obj_t *)number;
            } else {
                PARSER_ALLOC(number, cfg_obj_integer_t);
                cfg_init_integer(number, parsed.value.v_int);
                *obj = (cfg_obj_t *)number;
            }
            return PARSER_OK;
        }
        case '%':
        case '(':
        {
            int vector_type = PARSER_NUMBER_FLOAT;
            if (curr() == '%') {
                vector_type = PARSER_NUMBER_INT;
                next();
                if (curr() != '(') {
                    ERROR("expecting '('");
                }
            }
            
            next();
            
            int component_count = 0;
            struct parsed_number components[3];
            
            do {
                if (component_count > 2) {
                    ERROR("vector has a maximum of 3 components");
                }
                
                skip_whitespace();
                if (!is_number_start()) {
                    ERROR("expecting start of number");
                }
                
                int result = parse_number(p, &components[component_count]);
                if (!result) {
                    return PARSER_FAIL;
                }
                
                if (vector_type == PARSER_NUMBER_INT) {
                    if (components[component_count].type == PARSER_NUMBER_FLOAT) {
                        ERROR("can't put floats into integer vector");
                    }
                } else if (components[component_count].type == PARSER_NUMBER_INT) {
                    const cfg_int_t old = components[component_count].value.v_int;
                    components[component_count].value.v_float = (float)old;
                }
                
                component_count++;
                
                skip_whitespace();
                if (curr() != ',') {
                    break;
                } else {
                    next();
                }
            } while (1);
            
            if (curr() != ')') {
                ERROR("expecting ')'");
            }
            
            next();
            
            int i;
            for (i = component_count; i < 3; i++) {
                components[i].type = vector_type;
                if (vector_type == PARSER_NUMBER_INT) {
                    components[i].value.v_int = 0;
                } else {
                    components[i].value.v_float = 0.0;
                }
            }
            
            if (vector_type == PARSER_NUMBER_INT) {
                PARSER_ALLOC(vector, cfg_obj_integer_vector_t);
                cfg_init_integer_vector(vector, components[0].value.v_int,
                                                components[1].value.v_int,
                                                components[2].value.v_int);
                *obj = (cfg_obj_t *)vector;
            } else {
                PARSER_ALLOC(vector, cfg_obj_float_vector_t);
                cfg_init_float_vector(vector, components[0].value.v_float,
                                              components[1].value.v_float,
                                              components[2].value.v_float);
                *obj = (cfg_obj_t *)vector;
            }
            
            return PARSER_OK;
        }
        case '@':
        {
            next();
            int year, month, day, hour, minute, second, millisecond;
            
            if (!parse_fixed_int(p, &year, 4)) { ERROR("couldn't parse year"); }
            if (curr() != '-') { ERROR("expecting '-'"); } else { next(); }
            if (!parse_fixed_int(p, &month, 2)) { ERROR("couldn't parse month"); }
            if (curr() != '-') { ERROR("expecting '-'"); } else { next(); }
            if (!parse_fixed_int(p, &day, 2)) { ERROR("couldn't parse day"); }
            if (curr() == 'T') {
                next();
                if (!parse_fixed_int(p, &hour, 2)) { ERROR("couldn't parse hour"); }
                if (curr() != ':') { ERROR("expecting '-'"); } else { next(); }
                if (!parse_fixed_int(p, &minute, 2)) { ERROR("couldn't parse minute"); }
                if (curr() != ':') { ERROR("expecting '-'"); } else { next(); }
                if (!parse_fixed_int(p, &second, 2)) { ERROR("couldn't parse second"); }
                if (curr() == '.') {
                    next();
                    if (!parse_fixed_int(p, &millisecond, 3)) { ERROR("couldn't parse millisecond"); }
                } else {
                    millisecond = 0;
                }
                PARSER_ALLOC(datetime, cfg_obj_datetime_t);
                cfg_init_datetime(datetime, year, month, day, hour, minute, second, millisecond);
                *obj = (cfg_obj_t*)datetime;
            } else {
                PARSER_ALLOC(date, cfg_obj_date_t);
                cfg_init_date(date, year, month, day);
                *obj = (cfg_obj_t*)date;
            }
            return PARSER_OK;
        }
        case '{':
        {
            next();
            PARSER_ALLOC(dict, cfg_obj_dict_t);
            cfg_init_dict(dict);
            if (!parse_dict_body(p, dict, 0)) {
                return PARSER_FAIL;
            }
            skip_whitespace();
            if (curr() != '}') {
                ERROR("expecting '}'");
            }
            next();
            *obj = (cfg_obj_t*)dict;
            return PARSER_OK;
        }
        case '[':
        {
            next();
            PARSER_ALLOC(ary, cfg_obj_array_t);
            cfg_init_array(ary);
            if (!parse_array_body(p, ary)) {
                return PARSER_FAIL;
            }
            skip_whitespace();
            if (curr() != ']') {
                ERROR("expecting ']'");
            }
            next();
            *obj = (cfg_obj_t*)ary;
            return PARSER_OK;
        }
            
    }
    return PARSER_FAIL;
}
