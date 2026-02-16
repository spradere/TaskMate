#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define PSRT(s) (s)

#define CONCAT2(a,b) a##b
#define CONCAT(a,b) CONCAT2(a,b)

#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

#define tm_printf(format, vargs) \
	tm_string_t UNIQUE_NAME(str) = { \
		.text = PSRT(format), \
		.len = 256, \
		.storage = STRING_RAM}; \
		UNIQUE_NAME(str).len = tm_strlen(&UNIQUE_NAME(str)); \
		tm_printf_P(UNIQUE_NAME(str), (vargs))


typedef struct
{
	const char *text;
	int len;
} tm_string_t;


int tm_printf_P(tm_string_t format, ...);
int tm_vprintf_P(tm_string_t format, va_list args);


int tm_printf_P(tm_string_t format, ...)
{
	int ret;
	va_list args;
	va_start(args, format.text);
	ret = tm_vprintf_P(format, args);
	va_end(args);
	return ret;
}

int tm_vprintf_P(tm_string_t format, va_list args)
{
	return vprintf(format.text, args);
}

int main(void)
{
	int i=10;
	tm_printf("hello %i\n",i);

	tm_printf("hello 2 %i\n",i+i);

	return 0;

}
