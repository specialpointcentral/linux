// SPDX-License-Identifier: GPL-2.0
/*
 * arch/loongarch/boot/string.c
 *
 * Very small subset of simple string routines
 */

#include <linux/types.h>

void __weak *memset(void *s, int c, size_t n)
{
	int i;
	char *ss = s;

	for (i = 0; i < n; i++)
		ss[i] = c;
	return s;
}

void __weak *memcpy(void *dest, const void *src, size_t n)
{
	int i;
	const char *s = src;
	char *d = dest;

	for (i = 0; i < n; i++)
		d[i] = s[i];
	return dest;
}

void __weak *memmove(void *dest, const void *src, size_t n)
{
	int i;
	const char *s = src;
	char *d = dest;

	if (d < s) {
		for (i = 0; i < n; i++)
			d[i] = s[i];
	} else if (d > s) {
		for (i = n - 1; i >= 0; i--)
			d[i] = s[i];
	}

	return dest;
}

int __weak memcmp(const void *cs, const void *ct, size_t count)
{
	int res = 0;
	const unsigned char *su1, *su2;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--) {
		res = *su1 - *su2;
		if (res != 0)
			break;
	}
	return res;
}

void * __weak memchr(const void *s, int c, size_t n)
{
	const unsigned char *p = s;
	while (n-- != 0) {
		if ((unsigned char)c == *p++) {
			return (void *)(p - 1);
		}
	}
	return NULL;
}

int __weak strcmp(const char *str1, const char *str2)
{
	int delta = 0;
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;

	while (*s1 || *s2) {
		delta = *s1 - *s2;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}

size_t __weak strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

size_t __weak strnlen(const char *s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

char * __weak strnstr(const char *s1, const char *s2, size_t len)
{
	size_t l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;
	while (len >= l2) {
		len--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

#undef strcat
char * __weak strcat(char *dest, const char *src)
{
	char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;
	return tmp;
}

char * __weak strncat(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != 0) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}
	return tmp;
}

char * __weak strrchr(const char *s, int c)
{
	const char *last = NULL;
	do {
		if (*s == (char)c)
			last = s;
	} while (*s++);
	return (char *)last;
}

#ifdef CONFIG_KASAN
extern void *__memset(void *s, int c, size_t n) __alias(memset);
extern void *__memcpy(void *dest, const void *src, size_t n) __alias(memcpy);
extern void *__memmove(void *dest, const void *src, size_t n) __alias(memmove);
#endif
