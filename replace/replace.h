#ifndef REPLACE_H
#define REPLACE_H

#if !HAVE_MALLOC

extern
void *
rpl_malloc (size_t n);

#endif

#if !HAVE_STRTOF

extern
float strtof(const char* s, char** endptr);

#endif

#endif
