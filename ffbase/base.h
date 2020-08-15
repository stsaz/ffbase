/** ffbase: base types & memory functions
2020, Simon Zolin
*/

#pragma once
#define _FFBASE_BASE_H

/*
Detect CPU
	FF_AMD64 FF_X86 FF_ARM FF_64
	FF_LITTLE_ENDIAN FF_BIG_ENDIAN
Detect OS
	FF_UNIX FF_WIN
Base types
	ffbyte ffushort ffint ffuint ffint64 ffuint64 ffsize ffssize
FF_ASSERT
ff_printf
ffmin ffmax ffint_abs
FF_COUNT FFS_LEN
FF_OFF FF_PTR
FF_STRUCTPTR
ffint_be_cpu16 ffint_be_cpu32 ffint_be_cpu64
ffint_le_cpu16 ffint_le_cpu32 ffint_le_cpu64
ffbit_find32 ffbit_find64
ffint_ispower2 ffint_align_power2
ffsz_len ffwsz_len
Heap allocation
	ffmem_alloc ffmem_zalloc ffmem_new ffmem_realloc ffmem_free
ffmem_stack
ffmem_cmp ffmem_fill ffmem_findbyte
ffmem_zero ffmem_zero_obj
ffmem_copy ffmem_move
*/

/* Detect CPU */
#if defined FF_AMD64 || defined FF_X86 || defined FF_ARM
	// already defined

#elif defined __amd64__ || defined _M_AMD64
	#define FF_AMD64
	#define FF_LITTLE_ENDIAN
	#define FF_64

#elif defined __i386__ || defined _M_IX86
	#define FF_X86
	#define FF_LITTLE_ENDIAN

#elif defined __arm__ || defined _M_ARM || defined __aarch64__
	#define FF_ARM
	#define FF_BIG_ENDIAN
	#ifdef __LP64__
		#define FF_64
	#endif

#else
	#warning "This CPU is not supported"
#endif


/* Detect OS */
#if defined FF_WIN || defined FF_UNIX
	// already defined

#elif defined _WIN32 || defined _WIN64 || defined __CYGWIN__
	#define FF_WIN
	#include <windows.h>
	#include <stdlib.h>

#else
	#define FF_UNIX
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
#endif


/* Base types */
#define ffbyte  unsigned char
#define ffushort  unsigned short
#define ffint  int
#define ffuint  unsigned int
#define ffint64  long long
#define ffuint64  unsigned long long
#define ffsize  size_t
#define ffssize  ssize_t


#ifdef FF_DEBUG
	#include <assert.h>

	/** Debug-mode assertion */
	#define FF_ASSERT(expr)  assert(expr)
#else
	#define FF_ASSERT(expr)
#endif


#include <stdio.h>
/** Formatted print to stdout
Supported: %p, %u, %s */
#define ff_printf(fmt, ...)  printf(fmt, ##__VA_ARGS__)


/** Get minimum value */
static inline ffsize ffmin(ffsize a, ffsize b)
{
	return (a < b) ? a : b;
}

/** Get maximum value */
#define ffmax(i0, i1) \
	(((i0) < (i1)) ? (i1) : (i0))

/** Get absolute value */
#define ffint_abs(n) \
({ \
	__typeof__(n) _n = (n); \
	(_n >= 0) ? _n : -_n; \
})


/** Get N of elements in a static C array */
#define FF_COUNT(ar)  (sizeof(ar) / sizeof(ar[0]))

/** Get N of characters in a static C string */
#define FFS_LEN(s)  (FF_COUNT(s) - 1)

/** Get offset of a field in structure */
#define FF_OFF(struct_type, field) \
	(((ffsize)&((struct_type *)0)->field))

/** Get struct field pointer by struct pointer and field offset */
#define FF_PTR(struct_ptr, field_off)  ((void*)((char*)(struct_ptr) + (field_off)))

/** Get struct pointer by its field pointer */
#define FF_STRUCTPTR(struct_type, field_name, field_ptr) \
	((struct_type*)((char*)field_ptr - FF_OFF(struct_type, field_name)))


/** Swap bytes
e.g. "0x11223344" <-> "0x44332211" */
#define ffint_bswap16(i)  __builtin_bswap16(i)
#define ffint_bswap32(i)  __builtin_bswap32(i)
#define ffint_bswap64(i)  __builtin_bswap64(i)

#ifdef FF_LITTLE_ENDIAN
	/** Swap bytes: BE <-> CPU */
	#define ffint_be_cpu16(i)  __builtin_bswap16(i)
	#define ffint_be_cpu32(i)  __builtin_bswap32(i)
	#define ffint_be_cpu64(i)  __builtin_bswap64(i)

	/** Swap bytes: LE <-> CPU */
	#define ffint_le_cpu16(i)  (i)
	#define ffint_le_cpu32(i)  (i)
	#define ffint_le_cpu64(i)  (i)

#else // FF_BIG_ENDIAN:
	/** Swap bytes: BE <-> CPU */
	#define ffint_be_cpu16(i)  (i)
	#define ffint_be_cpu32(i)  (i)
	#define ffint_be_cpu64(i)  (i)

	/** Swap bytes: LE <-> CPU */
	#define ffint_le_cpu16(i)  __builtin_bswap16(i)
	#define ffint_le_cpu32(i)  __builtin_bswap32(i)
	#define ffint_le_cpu64(i)  __builtin_bswap64(i)
#endif


/** Find the most significant 1-bit
--> 0xABCD
Return bit position +1;
 0 if not found */
static inline ffuint ffbit_find32(ffuint n)
{
	return (n != 0) ? __builtin_clz(n) + 1 : 0;
}

static inline ffuint ffbit_find64(ffuint64 n)
{
	return (n != 0) ? __builtin_clzll(n) + 1 : 0;
}

/** Find the least significant 1-bit
0xABCD <--
Return position +1
  0 if not found */
static inline ffuint ffbit_rfind32(ffuint n)
{
	return __builtin_ffs(n);
}

static inline ffuint ffbit_rfind64(ffuint64 n)
{
	return __builtin_ffsll(n);
}


/** Return TRUE if number is a power of 2 */
#define ffint_ispower2(n)  ((n) >= 2 && (((n) - 1) & (n)) == 0)

/** Align number to the next power of 2
Note: value n > 2^63 is not supported */
static inline ffuint64 ffint_align_power2(ffuint64 n)
{
	if (n <= 2)
		return 2;
	ffuint one = ffbit_find64(n - 1);
	return 1ULL << (64 - one + 1);
}


#define ffsz_len(sz)  strlen(sz)
#define ffwsz_len(sz)  wcslen(sz)


/* Heap allocation */
#if !defined ffmem_alloc

/** Allocate heap buffer */
#define ffmem_alloc(size)  malloc(size)

/** Allocate heap buffer and zero it */
#define ffmem_zalloc(size)  calloc(1, size)

/** Allocate object and zero it */
#define ffmem_new(T)  ((T*)calloc(1, sizeof(T)))

/** Change heap buffer size */
#define ffmem_realloc(ptr, newsize)  realloc(ptr, newsize)

/** Free heap buffer */
#define ffmem_free(ptr)  free(ptr)

#endif


/** Reserve stack buffer */
#define ffmem_stack(size)  alloca(size)

#define FFMEM_STACK_THRESHOLD  4096

#define _ffmem_alloc_stackorheap(size) \
	((size) < FFMEM_STACK_THRESHOLD) ? alloca(size) : ffmem_alloc(size)

#define _ffmem_free_stackorheap(ptr, size) \
({ \
	if ((size) >= FFMEM_STACK_THRESHOLD) \
		ffmem_free(ptr); \
})


/** Compare buffers */
#define ffmem_cmp(a, b, len)  memcmp(a, b, len)

/** Find byte in buffer
Return pointer to the byte or NULL */
#define ffmem_findbyte(p, len, ch)  memchr(p, ch, len)

/** Fill the buffer with copies of byte */
#define ffmem_fill(p, ch, len)  memset(p, ch, len)

/** Fill the buffer with zeros */
#define ffmem_zero(ptr, len)  memset(ptr, 0, len)

/** Fill the buffer with zeros */
#define ffmem_zero_obj(ptr)  memset(ptr, 0, sizeof(*ptr))

/** Copy data
Return tail pointer */
static inline void* ffmem_copy(void *dst, const void *src, ffsize len)
{
	memcpy(dst, src, len);
	return (char*)dst + len;
}

/** Safely copy data (overlapping regions) */
#define ffmem_move(dst, src, len)  (void) memmove(dst, src, len)
