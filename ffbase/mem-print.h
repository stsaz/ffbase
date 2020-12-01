/** ffbase: printing memory contents
2020, Simon Zolin
*/

#pragma once

#include <ffbase/string.h>


enum FFMEM_PRINT {
	FFMEM_PRINT_NO_TEXT = 0x0100, // don't print text
	FFMEM_PRINT_ZEROSPACE = 0x0200, // print space instead of dot for 0x00 byte
};

/** Represent data as hex string and text
Format:
  OFFSET  XX XX XX XX  XX XX XX XX  .... ....
flags: line_width | enum FFMEM_PRINT
Return newly allocated buffer, free with ffstr_free() */
static inline ffstr ffmem_print(const void *data, ffsize len, ffuint flags)
{
	ffuint width = flags & 0xff;
	if (width == 0)
		width = 16;
	ffuint blocks = width / 4;
	ffstr s = {};
	if (NULL == ffstr_alloc(&s, (8 + FFS_LEN("  XX XX XX XX")*blocks + 1 + FFS_LEN(" ....")*blocks + 1) * (len/width + 1)))
		return s;

	const ffbyte *d = (ffbyte*)data;
	for (ffsize i = 0;  i != len; ) {

		// print offset
		ffsize off = i;
		for (int k = 0;  k != 8;  k++) {
			s.ptr[s.len++] = ffhex[(off & 0xf0000000) >> 28];
			off <<= 4;
		}

		ffsize itext = s.len + FFS_LEN("  XX XX XX XX")*blocks, itext_begin = itext;
		s.ptr[itext++] = ' ';

		ffsize hi = ffmin(i + width, len);
		for (; i < hi;  i++) {

			if (i % 4 == 0) {
				s.ptr[s.len++] = ' ';
				s.ptr[itext++] = ' ';
			}

			s.ptr[s.len++] = ' ';
			s.ptr[s.len++] = ffhex[(d[i] & 0xf0) >> 4];
			s.ptr[s.len++] = ffhex[d[i] & 0x0f];

			if (!(flags & FFMEM_PRINT_NO_TEXT)) {
				if (d[i] >= 0x20 && d[i] < 0x7f) // if printable
					s.ptr[itext] = d[i];
				else if (d[i] == 0x00 && (flags & FFMEM_PRINT_ZEROSPACE))
					s.ptr[itext] = ' ';
				else
					s.ptr[itext] = '.';
				itext++;
			}
		}

		// padding between hexdata and text
		while (s.len != itext_begin) {
			s.ptr[s.len++] = ' ';
		}

		if (!(flags & FFMEM_PRINT_NO_TEXT))
			s.len += itext - s.len;

		s.ptr[s.len++] = '\n';
	}
	return s;
}
