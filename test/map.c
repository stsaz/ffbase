/** ffbase/map.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/map.h>
#include <ffbase/string.h>
#include <test/test.h>


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
}
