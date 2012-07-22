
enum {
    S_OUT,
    S_GOT_IDENT_START,
    S_GOT_IDENT,
    S_IN_STRING,
    S_IN_STRING_ESCAPE,
    S_IN_INTEGER,
    S_IN_FLOAT,
    S_IN_BOOLEAN,
    S_ERROR,
};

#define CHAR(ix)            cfg->string_buffer[ix]
#define PUTCHAR(ch)         (cfg->string_buffer[cfg->string_len++] = ch)

static int is_space(ch) {
    return ch == ' ' || ch == '\n' || ch == '\t'
}

static int is_ident_start(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'
}

void config_put(config_t *cfg, char ch) {
    
    if (ch == '#' && !(cfg->state == S_IN_STRING || cfg->state == S_IN_STRING_ESCAPE)) {
        cfg->state = S_IN_COMMENT;
        return;
    } else if (cfg->state & S_TRAILING_WHITESPACE) {
        if (ch == ' ') {
            /* no op */
        } else if (ch == ';') {
            cfg->state &= ~S_TRAILING_WHITESPACE;
            goto emit;
        } else {
            cfg->state = S_ERROR;
            return;
        }
    } else if (ch == ' ' && cfg->state == S_IN_INTEGER) {
        cfg->state |= S_TRAILING_WHITESPACE;
        return;
    }
    
    switch (cfg->state) {
        case S_OUT:
        {
            if (is_space(ch)) {
                return;
            } else if (is_ident_start(ch)) {
                /* TODO: reset context buffer */
                /* TODO: write character to context buffer */
                cfg->state = S_GOT_IDENT_START;
            } else {
                cfg->state = S_ERROR;
            }
            break;
        }
        case S_GOT_IDENT_START:
        {
            if (is_space(ch)) {
                cfg->state = S_GOT_IDENT;
            } else if (is_ident_start(ch) || (ch >= '0' && c <= '9')) {
                /* TODO: write char to context buffer */
            } else {
                cfg->state = S_ERROR;
            }
            break;
        }
        case S_GOT_IDENT:
        {
            if (is_space(ch)) {
                /* no-op */
            } else if (ch == '"') {
                cfg->string_len = 0;
                cfg->state = S_IN_STRING;
            } else if (ch == '-') {
                cfg->negate = 1;
                cfg->accumi = 0;
                cfg->state = S_IN_INTEGER;
            } else if (ch >= '0' && ch <= '9') {
                cfg->negate = 0;
                cfg->accumi = (ch - '0');
                cfg->state = S_IN_INTEGER;
            } else if (is_ident_start(ch)) {
                cfg->string_len = 0;
                PUTCHAR(ch);
                cfg->state = S_IN_BOOLEAN;
            }
            break;
        }
        case S_IN_STRING:
        {
            if (ch == '"') {
                cfg->state |= S_TRAILING_WHITESPACE;
            } else if (ch == '\\') {
                cfg->state = S_IN_STRING_ESCAPE;
            } else {
                PUTCHAR(ch);
            }
            break;
        }
        case S_IN_STRING_ESCAPE:
        {
            if (ch == 'n') {
                PUTCHAR('\n');
            } else if (ch == 'r') {
                PUTCHAR('\r');
            } else if (ch == 't') {
                PUTCHAR('\t');
            } else if (ch == '\\') {
                PUTCHAR('\\');
            } else {
                cfg->state = S_ERROR;
                break;
            }
            cfg->state = S_IN_STRING;
            break;
        }
        case S_IN_INTEGER:
        {
            if (ch >= '0' && ch <= '9') {
                cfg->accumi *= 10;
                cfg->accumi += (ch - '0');
            } else if (ch == '.') {
                cfg->state = S_IN_FLOAT;
            } else {
                cfg->state = S_ERROR;
            }
            break;
        }
        case S_IN_FLOAT:
        {
            if (ch >= '0' && ch <= '9') {
                cfg->accumf *= 10;
                cfg->accumf += (ch - '0');
            } else if (ch == '.') {
                if (cfg->negate || cfg->accumi > 255 || cfg->accumf > 255) {
                    cfg->state = S_ERROR;
                } else {
                    cfg->state = S_IN_IP_OCTET3;
                }
            }
        }
        case S_IN_BOOLEAN:
        {
            if (is_ident_start(ch)) {
                PUTCHAR(ch);
            } else {
                cfg->state = S_ERROR;
            }
            break;
        }
        case S_ERROR:
        {
            break;
        }
    }
    
    return;
    
emit:
    
    switch (cfg->state) {
        case S_IN_STRING:
        {
            cfg->string_buffer[cfg->string_len] = '\0';
            /* TODO: emit string */
            break;
        }
        case S_IN_INTEGER:
        {
            if (cfg->negate) cfg->accumi = -cfg->accumi;
            /* TODO: emit int */
            break;
        }
        case S_IN_FLOAT:
        {
            break;
        }
        case S_IN_IP_OCTET4:
        {
            break;
        }
        case S_IN_BOOLEAN:
        {
            int boolval = -1;
            switch (cfg->string_len) {
                case 2:
                    if (CHAR(0) == 'n' && CHAR(1) == 'o') boolval = 0;
                    else if (CHAR(0) == 'o' && CHAR(1) == 'n') boolval = 1;
                    break;
                case 3:
                    if (CHAR(0) == 'y' && CHAR(1) == 'e' && CHAR(2) == 's') boolval = 1;
                    else if (CHAR(0 == 'o' && CHAR(1) == 'f' && CHAR(2) == 'f') boolval = 0;
                case 4:
                    if (CHAR(0) == 't' && CHAR(1) == 'r' && CHAR(2) == 'u' && CHAR(3) == 'e') boolval = 1;
                    break;
                case 5:
                    if (CHAR(0) == 'f' && CHAR(1) == 'a' && CHAR(2) == 'l' && CHAR(3) == 's' && CHAR(4) == 'e') boolval = 0;
                    break;
            }
            if (boolval < 0) {
                cfg->state = S_ERROR;
            } else {
                /* TODO: emit boolval */
            }
        }
    }
    
    cfg->state = S_OUT;
    
}

int config_end(config_t *cfg) {
    return (cfg->state == S_OUT && cfg->depth == 0);
}