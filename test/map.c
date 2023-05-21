/** ffbase/map.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/map.h>
#include <ffbase/string.h>
#include <test/test.h>
#define FFARRAY_FOREACH FF_FOREACH


static inline int keyeq(void *opaque, const void *key, ffsize keylen, void *val)
{
	(void) opaque;
	return !ffs_cmpz(key, keylen, val);
}

static const char *httphdrs[] = {
	"Connection",
	"Keep-Alive",
	"Date",
	"Transfer-Encoding",
	"Via",
	"Cache-Control",
	"Content-Encoding",
	"Content-Length",
	"Content-Range",
	"Content-Type",
	"Host",
	"Referer",
	"User-Agent",
	"Accept",
	"Accept-Encoding",
	"TE",
	"If-Match",
	"If-Modified-Since",
	"If-None-Match",
	"If-Range",
	"If-Unmodified-Since",
	"Range",
	"Authorization",
	"Cookie",
	"Cookie2",
	"Proxy-Authorization",
	"Proxy-Connection",
	"Age",
	"Server",
	"Location",
	"ETag",
	"Expires",
	"Last-Modified",
	"Accept-Ranges",
	"Vary",
	"Proxy-Authenticate",
	"Set-Cookie",
	"Set-Cookie2",
	"WWW-Authenticate",
	"Upgrade",
	"X-Forwarded-For",
};

void test_map_rm()
{
	ffmap m = {};
	ffmap_init(&m, &keyeq);

	/*
	RM #0			COLL==RM #2
	OCC1 #1			OCC1 #1
	COLL==RM #2		COLL==OCC1 #4
	OCC2 #3			OCC2 #3
	COLL==OCC1 #4	FREE
	FREE			FREE
	*/

	ffmap_add_hash(&m, 0, (void*)0);
	ffmap_add_hash(&m, 1, (void*)1);
	ffmap_add_hash(&m, 0, (void*)2);
	ffmap_add_hash(&m, 3, (void*)3);
	ffmap_add_hash(&m, 1, (void*)4);
	ffmap_rm_hash(&m, 0, (void*)0);
	x(m.data[0].val == (void*)2);
	x(m.data[1].val == (void*)1);
	x(m.data[2].val == (void*)4);
	x(m.data[3].val == (void*)3);
	ffmap_free(&m);

	ffmap_init(&m, &keyeq);

	/*
	OCC1 #1			OCC1 #1
	COLL==RM #2		COLL==OCC1 #4
	OCC2 #3			OCC2 #3
	COLL==OCC1 #4	FREE
	FREE			FREE
	RM #0			COLL==RM #2
	*/

	ffmap_add_hash(&m, 15, (void*)0);
	ffmap_add_hash(&m, 0, (void*)1);
	ffmap_add_hash(&m, 15, (void*)2);
	ffmap_add_hash(&m, 2, (void*)3);
	ffmap_add_hash(&m, 0, (void*)4);
	ffmap_rm_hash(&m, 15, (void*)0);
	x(m.data[15].val == (void*)2);
	x(m.data[0].val == (void*)1);
	x(m.data[1].val == (void*)4);
	x(m.data[2].val == (void*)3);
	ffmap_free(&m);
}

void test_map()
{
	ffmap m = {};
	void *it;

	ffmap_init(&m, &keyeq);

	const char *s = "This is a test string!";
	xieq(128329757, ffmap_hash(s, ffsz_len(s)));

	ffmap_grow(&m, 8);
	ffmap_grow(&m, 24);

	const char **hh;
	FFARRAY_FOREACH(httphdrs, hh) {
		x(0 == ffmap_add(&m, *hh, ffsz_len(*hh), (void*)*hh));
	}

	FFARRAY_FOREACH(httphdrs, hh) {
		it = ffmap_find(&m, *hh, ffsz_len(*hh), NULL);
		x(it != NULL);
		x(it == (void*)*hh);
	}

	_ffmap_stats(&m, 1);

	{
		struct _ffmap_item *it;
		FFMAP_WALK(&m, it) {
			if (_ffmap_item_occupied(it)) {
			}
		}
	}

	FFARRAY_FOREACH(httphdrs, hh) {
		xieq(0, ffmap_rm(&m, *hh, ffsz_len(*hh), NULL));
		xieq(-1, ffmap_rm(&m, *hh, ffsz_len(*hh), NULL));
	}

	FFARRAY_FOREACH(httphdrs, hh) {
		xieq(-1, ffmap_rm(&m, *hh, ffsz_len(*hh), NULL));
	}

	_ffmap_stats(&m, 1);

	ffmap_free(&m);

	test_map_rm();
}
