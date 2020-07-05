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

	const char **hh;
	FFARR_WALK(httphdrs, hh) {
		x(0 == ffmap_add(&m, *hh, ffsz_len(*hh), (void*)*hh));
	}

	FFARR_WALK(httphdrs, hh) {
		it = ffmap_find(&m, *hh, ffsz_len(*hh), NULL);
		x(it != NULL);
		x(it == (void*)*hh);
	}

	_ffmap_stats(&m);
	ffmap_free(&m);
}
