/** ffbase/conf.h benchmark */

/*
Synthetic benchmark (i5-8250U, clang-16, 50MB "fmedia.gui"):

	                 MB/sec
	=======================
	new    -O3 sse42  405
	new    -O3/-O2    298
	old*   -O3        210
	new    -O0 sse42   99
	new    -O0         69
	old*   -O0         65

	* 'old' is an old straightforward implementation with a per-byte reading loop
*/

#include <ffsys/file.h>
#include <ffsys/signal.h>
#include <ffsys/std.h>
#include <ffsys/globals.h>
#include <ffbase/atomic.h>
#include <ffbase/conf.h>

int stop;

void onsig(struct ffsig_info*)
{
	FFINT_WRITEONCE(stop, 1);
}

void main()
{
	ffuint sigs[] = { SIGINT };
	ffsig_subscribe(onsig, sigs, 1);

	char buf[1*1024*1024];
	ffuint64 total = 0;
	struct ffconf ltc = {};
	while (!FFINT_READONCE(stop)) {
		int r = fffile_read(ffstdin, buf, sizeof(buf));
		if (r <= 0)
			break;
		total += r;

		ffstr in = FFSTR_INITN(buf, r);
		while (in.len) {
			ffstr out;
			r = ffconf_read(&ltc, &in, &out);
			if (r == FFCONF_ERROR) {
				ffstr_shift(&in, ltc.line_off);
				continue;
			}
			if (ffstr_eqz(&out, "stop"))
				break;
		}
	}

	ffstderr_fmt("total: %,U\n", total);
}
