#ifndef STDARG_H_
#define STDARG_H_

#include "types.h"

typedef uint8_t * va_list;

#define va_size(TYPE) (((sizeof(TYPE) -1)/sizeof(int)+1) * sizeof(int))

#define va_start(AP, FIRST)									\
		(AP = ((va_list)&FIRST + va_size(FIRST)))

#define va_end(AP) (AP=(uint8_t)NULL)

#define va_arg(AP, TYPE)									\
		(AP += va_size(TYPE),								\
		 *((TYPE *)(AP - va_size(TYPE))))

#endif

