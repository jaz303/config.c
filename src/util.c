#include "util.h"

#ifdef CONFIG__USE_STDLIB
	#include <string.h>
#endif

int cfg_streq(const char *l, const char *r) {
	#ifdef CONFIG__USE_STDLIB
		return strcmp(l, r) == 0;
	#else
		while (*l != '\0' && *l == *r) {
			l++; r++;
		}
		return *l == *r;
	#endif
}

int cfg_strlen(const char *str) {
	#ifdef CONFIG__USE_STDLIB
		return strlen(str);
	#else
		int len = 0;
		while (*(str++)) len++;
		return len;
	#endif
}

void cfg_strcpy(char *dst, const char *src) {
	#ifdef CONFIG__USE_STDLIB
		strcpy(dst, src);
	#else
		while (*(dst++) = *(src++));
	#endif
}
