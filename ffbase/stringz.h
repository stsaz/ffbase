/** ffbase: NULL-terminated string functions
2020, Simon Zolin
*/

#pragma once

#ifndef _FFBASE_BASE_H
#include <ffbase/base.h>
#endif
#include <ffbase/string.h>

/*
ffsz_copyz ffsz_copyn
ffsz_dupn ffsz_dup
ffsz_findchar
ffsz_match ffsz_matchz
ffszarr_find ffszarr_ifind
ffszarr_findsorted ffszarr_ifindsorted
ffcharr_findsorted
*/

/** Copy string
Return number of bytes written (including NULL byte)
  ==cap: usually means that there was not enough space */
static inline ffsize ffsz_copyz(char *dst, ffsize cap, const char *sz)
{
	if (cap == 0)
		return 0;

	ffsize i;
	for (i = 0;  i != cap - 1;  i++) {
		if (sz[i] == '\0')
			break;
		dst[i] = sz[i];
	}
	dst[i] = '\0';
	return i + 1;
}

static inline ffsize ffsz_copyn(char *dst, ffsize cap, const char *src, ffsize n)
{
	if (cap == 0)
		return 0;
	n = ffmin(n, cap - 1);
	ffmem_copy(dst, src, n);
	dst[n] = '\0';
	return n + 1;
}

/** Allocate memory and copy string */
static inline char* ffsz_dupn(const char *src, ffsize n)
{
	char *s = (char*)ffmem_alloc(n + 1);
	if (s == NULL)
		return NULL;
	ffmem_copy(s, src, n);
	s[n] = '\0';
	return s;
}

/** Allocate memory and copy string */
static inline char* ffsz_dup(const char *sz)
{
	return ffsz_dupn(sz, ffsz_len(sz));
}

static inline int ffsz_cmp(const char *sz, const char *cmpz)
{
	ffsize i = 0;
	do {
		if (sz[i] != cmpz[i])
			return sz[i] - cmpz[i];
	} while (sz[i++] != '\0');
	return 0;
}

static inline int ffsz_eq(const char *sz, const char *cmpz)
{
	return !ffsz_cmp(sz, cmpz);
}

/** Find a position of byte in a NULL-terminated string
Return <0 if not found */
static inline ffssize ffsz_findchar(const char *sz, int ch)
{
	for (ffsize i = 0;  sz[i] != '\0';  i++) {
		if (sz[i] == ch)
			return i;
	}
	return -1;
}

/** Return TRUE if 'n' characters are equal in both strings */
static inline int ffsz_match(const char *sz, const char *s, ffsize n)
{
	ffsize i;
	for (i = 0;  sz[i] != '\0';  i++) {
		if (i == n)
			return 1; // sz begins with s
		if (sz[i] != s[i])
			return 0; // s != sz
	}
	if (i != n)
		return 0; // sz < s
	return 1; // s == sz
}

static inline int ffsz_matchz(const char *sz, const char *sz2)
{
	for (ffsize i = 0;  ;  i++) {
		if (sz2[i] == '\0')
			return 1; // sz begins with sz2
		if (sz[i] != sz2[i])
			return 0; // sz != sz2
	}
}

/** Search a string in array of pointers to NULL-terminated strings.
Return -1 if not found */
static inline ffssize ffszarr_find(const char *const *ar, ffsize n, const char *search, ffsize search_len)
{
	for (ffsize i = 0;  i < n;  i++) {
		if (0 == ffs_cmpz(search, search_len, ar[i]))
			return i;
	}
	return -1;
}

/** Search (case-insensitive) a string in array of pointers to NULL-terminated strings.
Return -1 if not found */
static inline ffssize ffszarr_ifind(const char *const *ar, ffsize n, const char *search, ffsize search_len)
{
	for (ffsize i = 0;  i != n;  i++) {
		if (0 == ffs_icmpz(search, search_len, ar[i]))
			return i;
	}
	return -1;
}

/** Search a string in array of pointers to NULL-terminated strings.
Return -1 if not found */
static inline ffssize ffszarr_findsorted(const char *const *ar, ffsize n, const char *search, ffsize search_len)
{
	ffsize start = 0, end = n;
	while (start != end) {
		ffsize i = start + (end - start) / 2;
		int r = ffs_cmpz(search, search_len, ar[i]);
		if (r == 0)
			return i;
		else if (r < 0)
			end = i;
		else
			start = i + 1;
	}
	return -1;
}

/** Search (case-insensitive) a string in array of pointers to NULL-terminated strings.
Return -1 if not found */
static inline ffssize ffszarr_ifindsorted(const char *const *ar, ffsize n, const char *search, ffsize search_len)
{
	ffsize start = 0, end = n;
	while (start != end) {
		ffsize i = start + (end - start) / 2;
		int r = ffs_icmpz(search, search_len, ar[i]);
		if (r == 0)
			return i;
		else if (r < 0)
			end = i;
		else
			start = i + 1;
	}
	return -1;
}


/** Search a string in char[n][elsize] array.
Return -1 if not found */
static inline ffssize ffcharr_findsorted(const void *ar, ffsize n, ffsize elsize, const char *search, ffsize search_len)
{
	if (search_len > elsize)
		return -1; // the string's too large for this array

	ffsize start = 0;
	while (start != n) {
		ffsize i = start + (n - start) / 2;
		int r = ffmem_cmp(search, (char*)ar + i * elsize, search_len);

		if (r == 0
			&& search_len != elsize
			&& ((char*)ar)[i * elsize + search_len] != '\0')
			r = -1; // found "01" in {0,1,2}

		if (r == 0)
			return i;
		else if (r < 0)
			n = i;
		else
			start = i + 1;
	}
	return -1;
}
