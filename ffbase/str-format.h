/** ffbase: string format functions
2020, Simon Zolin
*/

/*
ffs_format ffs_formatv ffs_format_r0
ffsz_format ffsz_formatv
ffsz_allocfmt ffsz_allocfmtv
ffstr_matchfmt ffstr_matchfmtv
*/

#pragma once
#define _FFBASE_STRFORMAT_H

#ifdef FF_UNIX
#include <stdarg.h>
#endif

#include <ffbase/unicode.h> // optional

/** Formatted output into string.

Standard-compliant:
% [[0]width] d                 int
% [[0]width] u                 uint
%s                             char*
%%                             '%'
%c                             int
% [[0]width] [.prec] f         double

Non-standard:

Unlike standard format string where flags may change the size of an input argument
 (e.g. "%d" is int32 but "%lld" is int64)
 this format string has a consistent type which defines the *size* taken from stack,
 and flags change the *behaviour* - not the size.

Integer:
% [[0]width] [x|X|,] d         int
% [[0]width] [x|X|,] u         ffuint
% [[0]width] [x|X|,] D         ffint64
% [[0]width] [x|X|,] U         ffuint64
% [[0]width] [x|X|,] L         ffsize

Floating-point:
% [[0]width] [.prec] f         double (default precision:6)

Binary data:
% width x|X b                  byte*
% * x|X b                      ffsize, byte*

String:
%s                             char* (NULL-terminated string)
% width s                      char*
%*s                            ffsize, char*
% [width] S                    ffstr*
%*S                            ffsize, ffstr*

System string:
%q                             char* on UNIX, wchar_t* on Windows (NULL-terminated string)
% width q                      char*|wchar_t*
%*q                            ffsize, char*|wchar_t*

Char:
% [width] c                    int
%*c                            ffsize, int

%p                             void*  (Pointer)
%Z                             '\0'  (NULL byte)
%%                             '%'  (Percent sign)

%E                             int  (System error message) (build with -DFFBASE_HAVE_FFERR_STR and provide fferr_str())

Examples:
("%s", NULL)  =>  "(null)"
("%2xb", "AB")  =>  "4142"
("%*xb", (ffsize)2, "AB")  =>  "4142"
("%*c", (ffsize)3, 'A')  =>  "AAA"
("%E", errno)  =>  "(22) Invalid argument"

Algorithm:

Note: if there's not enough space, we just count the total size needed
 and then return it as a negative value.
This allows the caller to effectively get the N of bytes he needs to allocate,
 while also do the job (in 1 call) if the space is enough.

. Process flags:
 . '0'
 . width: [0-9]*
 . 'x|X|,'
 . '.':
  . precision: [0-9]*
 . '*':
  . pop ffsize from va
. Process type:
 . pop TYPE from va
 . process
 . if not integer: done
. Convert from integer


Return N of bytes written;
 0 on error (bad format string);
 <0 if not enough space: "-RESULT" is the total number of bytes needed */
static inline ffssize ffs_formatv(char *dst, ffsize cap, const char *fmt, va_list va)
{
	ffsize len = 0;

	for (;  *fmt != '\0';  fmt++) {
		if (*fmt != '%') {
			if (len < cap)
				dst[len] = *fmt;
			len++;
			continue;
		}
		fmt++;

		int have_width = 0;
		ffuint width = 0;
		ffuint prec = 6;
		ffssize r = 0;
		ffuint64 n;
		ffuint iflags = 0;

		if (*fmt == '0') {
			iflags |= FFS_INTZERO;
			fmt++;
		}

		for (;  (*fmt >= '0' && *fmt <= '9');  fmt++) {
			width = width * 10 + *fmt - '0';
			have_width = 1;
		}

		for (;;) {
			switch (*fmt) {
			case 'x':
				FF_ASSERT(!(iflags & FFS_INTHEX));
				fmt++;
				iflags |= FFS_INTHEX;
				break;

			case 'X':
				FF_ASSERT(!(iflags & FFS_INTHEX));
				fmt++;
				iflags |= FFS_INTHEX | FFS_INTHEXUP;
				break;

			case ',':
				FF_ASSERT(!(iflags & FFS_INTSEP1000));
				fmt++;
				iflags |= FFS_INTSEP1000;
				break;

			case '.':
				fmt++;
				prec = 0;
				for (;  (*fmt >= '0' && *fmt <= '9');  fmt++) {
					prec = prec * 10 + *fmt - '0';
				}
				break;

			case '*':
				FF_ASSERT(!have_width);
				if (have_width)
					return 0; // "%5*s"
				fmt++;
				width = va_arg(va, ffsize);
				have_width = 1;
				continue;
			}

			break;
		}

		switch (*fmt) {

		case 'U':
			n = va_arg(va, ffuint64);
			break;
		case 'D':
			n = va_arg(va, ffint64);
			iflags |= FFS_INTSIGN;
			break;

		case 'u':
			n = va_arg(va, ffuint);
			break;
		case 'd':
			n = va_arg(va, int);
			iflags |= FFS_INTSIGN;
			break;

		case 'L':
			n = va_arg(va, ffsize);
			break;

		case 'p':
			n = va_arg(va, ffsize);
			iflags |= FFS_INTHEX | FFS_INTZERO;
			width = sizeof(void*) * 2;
			break;

		case 'F':
		case 'f': {
			double d = va_arg(va, double);
			if (len < cap) {
				ffuint fzero = (iflags & FFS_INTZERO) ? FFS_FLTZERO : 0;
				r = ffs_fromfloat(d, dst + len, cap - len, FFS_FLTWIDTH(width) | fzero | prec);
			}
			len += (r != 0) ? r : FFS_FLTCAP;
			continue;
		}


#if !defined FF_WIN && defined _FFBASE_UNICODE_H
		case 'q':
#endif
		case 's': {
			const char *sz = va_arg(va, char*);

			if (have_width) {
				if (len + width < cap)
					ffmem_copy(dst + len, sz, width);
				r = width;

			} else {
				if (sz == NULL)
					sz = "(null)";

				if (len < cap) {
					r = _ffs_copyz(dst + len, cap - len, sz);
					if (len + r >= cap)
						r = ffsz_len(sz);

				} else {
					r = ffsz_len(sz);
				}
			}

			len += r;
			continue;
		}

		case 'S': {
			const ffstr *s = va_arg(va, ffstr*);

			if (len + s->len < cap)
				ffmem_copy(dst + len, s->ptr, s->len);
			len += s->len;

			r = width - s->len;
			if (r > 0) {
				if (len + r < cap)
					ffmem_fill(dst + len, ' ', r);
				len += r;
			}
			continue;
		}

#if defined FF_WIN && defined _FFBASE_UNICODE_H
		case 'q': {
			const wchar_t *sz = va_arg(va, wchar_t*);

			r = -1;
			if (have_width) {
				if (len < cap)
					r = ffs_wtou(dst + len, cap - len, sz, width);
				if (r < 0)
					r = ffs_wtou(NULL, 0, sz, width);

			} else {
				if (sz == NULL)
					sz = L"(null)";

				if (len < cap)
					r = ffs_wtouz(dst + len, cap - len, sz);
				if (r < 0)
					r = ffs_wtouz(NULL, 0, sz);
			}

			if (r > 0)
				len += r;
			continue;
		}
#endif


		case 'b': {
			FF_ASSERT(have_width);
			FF_ASSERT(iflags & FFS_INTHEX);
			if (!have_width)
				return 0; // width must be specified
			if (!(iflags & FFS_INTHEX))
				return 0; // 'x|X' must be specified

			const void *d = va_arg(va, void*);
			if (len + width * 2 < cap)
				ffs_fromhex(dst + len, cap - len, d, width, iflags & FFS_INTHEXUP);
			len += width * 2;
			continue;
		}


		case 'c': {
			int ch = va_arg(va, int);
			if (have_width) {
				if (len + width < cap)
					ffmem_fill(dst + len, ch, width);
				len += width;
			} else {
				if (len < cap)
					dst[len] = ch;
				len++;
			}
			continue;
		}

		case '%':
			if (len < cap)
				dst[len] = '%';
			len++;
			continue;

		case 'Z':
			if (len < cap)
				dst[len] = '\0';
			len++;
			continue;

#ifdef FFBASE_HAVE_FFERR_STR
		case 'E': {
			// "(%u) %s", errno, strerror(errno)
			int e = va_arg(va, int);
			if (len + 3 < cap) {
				r = ffs_fromint(e, dst + len + 1, cap - (len + 1), 0);
				if (r != 0) {
					dst[len++] = '(';
					len += r;
					dst[len++] = ')';
					dst[len++] = ' ';

					if (0 == fferr_str(e, dst + len, cap - len))
						r = ffsz_len(dst + len);
					else
						r = 255;
				} else {
					r = 3 + FFS_INTCAP + 255;
				}
			} else {
				r = 3 + FFS_INTCAP + 255;
			}
			len += r;
			continue;
		}
#endif

		default:
			FF_ASSERT(0);
			return 0; // bad format string
		}

		if (len < cap)
			r = ffs_fromint(n, dst + len, cap - len, iflags | FFS_INTWIDTH(width));
		len += (r != 0) ? r : FFS_INTCAP;
	}

	if (len >= cap)
		return -(ffssize)(len + 1); // not enough space
	return len;
}

static inline ffssize ffs_format(char *buf, ffsize cap, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ffssize r = ffs_formatv(buf, cap, fmt, args);
	va_end(args);
	return r;
}

/**
More convenient variant in case we don't need the exact data size if there was not enough space.
Return 0 on error */
static inline ffsize ffs_format_r0(char *buf, ffsize cap, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ffssize r = ffs_formatv(buf, cap, fmt, args);
	va_end(args);
	return (r >= 0) ? r : 0;
}

static inline ffssize ffsz_formatv(char *buf, ffsize cap, const char *fmt, va_list va)
{
	va_list args;
	va_copy(args, va);
	ffssize r = ffs_formatv(buf, (cap) ? cap - 1 : 0, fmt, args);
	va_end(args);
	if (cap != 0) {
		if (r >= 0)
			buf[r] = '\0';
		else
			buf[0] = '\0';
	}
	return r;
}

static inline ffssize ffsz_format(char *buf, ffsize cap, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ffssize r = ffsz_formatv(buf, cap, fmt, args);
	va_end(args);
	return r;
}

/** Allocate buffer and add %-formatted data to a NULL-terminated string.
Return a newly allocated string.  Must free with ffmem_free() */
static inline char* ffsz_allocfmtv(const char *fmt, va_list va)
{
	ffsize cap = 80;
	char *d;
	if (NULL == (d = (char*)ffmem_alloc(cap)))
		return NULL;

	va_list args;
	va_copy(args, va);
	ffssize n = ffs_formatv(d, cap - 1, fmt, args);
	va_end(args);

	if (n < 0) {
		FF_ASSERT(cap - 1 < (ffsize)-n);
		cap = -n + 1;
		ffmem_free(d);
		if (NULL == (d = (char*)ffmem_alloc(cap)))
			goto fail;

		va_copy(args, va);
		n = ffs_formatv(d, cap - 1, fmt, args);
		va_end(args);

		if (n < 0)
			goto fail;
	}

	d[n] = '\0';
	return d;

fail:
	ffmem_free(d);
	return NULL;
}

static inline char* ffsz_allocfmt(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *r = ffsz_allocfmtv(fmt, args);
	va_end(args);
	return r;
}


/** Match string by a format and extract data to output variables

Format:
% [width] S      ffstr*
% [width] [x] u  ffuint*
% [width] [x] U  ffuint64*
% [width] [x] d  int*
% [width] [x] D  ffint64*
%%               %

Algorithm:
. compare bytes until '%'
. get optional width from '%width...'
. get string chunk
. convert string to integer if needed

Return 0 if matched;
 >0: match: return stop index +1 within input string;
 <0: no match or format error */
static inline ffssize ffstr_matchfmtv(const ffstr *s, const char *fmt, va_list args)
{
	ffsize is = 0;
	ffuint width;
	ffuint intflags;
	for (ffsize i = 0;  fmt[i] != '\0';) {
		int ch = fmt[i++];
		if (ch != '%') {
			if (ch != s->ptr[is])
				return -1; // mismatch
			is++;
			continue;
		}

		width = 0;
		for (;;) {
			ch = fmt[i++];
			if (!(ch >= '0' && ch <= '9'))
				break;
			width = width*10 + (ch-'0');
		}
		if (is + width > s->len)
			return -1; // too small input

		intflags = 0;
		if (ch == 'x') {
			ch = fmt[i++];
			intflags = FFS_INTHEX;
		}

		ffstr chunk;
		switch (ch) {
		case 'S':
		case 'd':
		case 'D':
		case 'u':
		case 'U': {
			if (width != 0) {
				chunk.ptr = &s->ptr[is];  chunk.len = width;
				is += width;
				break;
			}

			if (fmt[i] == '\0') {
				width = s->len - is;
				chunk.ptr = &s->ptr[is];  chunk.len = width;
				is += width;
				break;

			} else if (fmt[i] == '%') {
				if (fmt[i+1] == '%') { // "%S%%"
					i++;
				} else {
					return -1; // "%S%?" - bad format string
				}
			}
			int stop_char = fmt[i++];

			chunk.ptr = &s->ptr[is];  chunk.len = 0;
			for (;;) {
				if (is == s->len) {
					return -1; // mismatch
				}
				if (s->ptr[is] == stop_char) {
					// match text until next %-var or EOS
					ffsize is2 = is, i2 = i;
					is++;
					for (;; i++, is++) {

						if (fmt[i] == '\0') {
							goto delim_ok;
						} else if (fmt[i] == '%') {
							if (fmt[i+1] != '%')
								goto delim_ok;
							// "%%"
							i++;
						}

						if (s->ptr[is] != fmt[i])
							break; // continue search
					}

					is = is2;
					i = i2;
				}
				is++;
				chunk.len++;
			}
delim_ok:
			break;
		}
		}

		switch (ch) {
		case 'S': {
			ffstr *pstr = va_arg(args, ffstr*);
			*pstr = chunk;
			break;
		}

		case 'd':
		case 'D':
		case 'u':
		case 'U': {
			void *pint;
			if (ch == 'd' || ch == 'u') {
				intflags |= FFS_INT32;
				pint = va_arg(args, ffuint*);
			} else {
				intflags |= FFS_INT64;
				pint = va_arg(args, ffuint64*);
			}

			if (ch == 'd' || ch == 'D')
				intflags |= FFS_INTSIGN;

			if (chunk.len == 0
				|| chunk.len != ffs_toint(chunk.ptr, chunk.len, pint, intflags))
				return -1; // bad integer
			break;
		}

		case '%':
			if (s->ptr[is] != '%')
				return -1; // mismatch
			is++;
			continue;

		default:
			return -1; // bad format string
		}
	}

	if (is == s->len)
		return 0;

	return is + 1;
}

static inline ffssize ffstr_matchfmt(const ffstr *s, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ffssize r = ffstr_matchfmtv(s, fmt, args);
	va_end(args);
	return r;
}
