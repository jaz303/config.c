#ifndef CONFIG__UTIL_H
#define CONFIG__UTIL_H

int cfg_streq(const char *l, const char *r);
int cfg_strlen(const char *str);
void cfg_strcpy(char *dst, const char *src);

#endif