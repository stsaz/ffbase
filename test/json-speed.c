/** ffbase: JSON parser performance test
2023, Simon Zolin */

#include <ffsys/file.h>
#include <ffsys/signal.h>
#include <ffsys/std.h>
#include <ffsys/globals.h>
#include <ffbase/atomic.h>
#include <ffbase/json.h>

int stop;

void onsig(struct ffsig_info*)
{
	FFINT_WRITEONCE(stop, 1);
}

int main()
{
	ffuint sigs[] = { SIGINT };
	ffsig_subscribe(onsig, sigs, FF_COUNT(sigs));

	ffuint cap = 1*1024*1024;
	char *buf = ffmem_alloc(cap);
	ffuint64 total = 0;
	ffjson js = {};
	ffjson_init(&js);

	while (!FFINT_READONCE(stop)) {
		int r = fffile_read(ffstdin, buf, cap);
		if (r <= 0)
			break;
		total += r;

		ffstr in = FFSTR_INITN(buf, r);
		while (in.len) {
			r = ffjson_parse(&js, &in);
			if (r < 0) {
				ffstderr_fmt("err: %s\n", ffjson_errstr(r));
				_exit(1);
			}
			if (ffstr_eqz(&js.val, "stop"))
				_exit(1);
		}
	}

	ffstderr_fmt("total: %,U\n", total);
	return 0;
}
