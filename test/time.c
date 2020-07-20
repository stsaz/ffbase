/** ffbase/time.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/time.h>
#include <test/test.h>


void test_time()
{
	fftime t = {};
	ffdatetime dt = {};

	// Jan 1, 1 AD
	dt.year = 1;
	dt.month = 1;
	dt.day = 1;
	fftime_join(&t, &dt);
	xieq(0, t.sec);

	// (no date) 01:02:03
	ffmem_zero_obj(&dt);
	dt.hour = 1;
	dt.minute = 2;
	dt.second = 3;
	fftime_join(&t, &dt);
	xieq(1*60*60 + 2*60 + 3, t.sec);

	// Dec 31, 1 AD, 23:59:59
	ffmem_zero_obj(&dt);
	dt.year = 1;
	dt.month = 12;
	dt.day = 31;
	dt.hour = 23;
	dt.minute = 59;
	dt.second = 59;
	fftime_join(&t, &dt);
	xieq(365*24*60*60-1, t.sec);

	// Jan 1, 1970
	ffmem_zero_obj(&dt);
	dt.year = 1970;
	dt.month = 1;
	dt.day = 1;
	fftime_join(&t, &dt);
	ffuint64 sec1970 = t.sec;
	fftime_split(&dt, &t);
	xieq(1970, dt.year);
	xieq(1, dt.month);
	xieq(1, dt.day);

	char buf[64];
	ffstr s;
	s.ptr = buf;
	s.len = fftime_tostr(&dt, buf, sizeof(buf), FFTIME_YMD);
	xseq(&s, "1970-01-01 00:00:00");

	s.len = fftime_tostr(&dt, buf, sizeof(buf), FFTIME_WDMY);
	xseq(&s, "Thu, 01 Jan 1970 00:00:00 GMT");

	// test each day since 1970
	ffuint64 cursec = sec1970;
	ffuint wd = 4;
	for (ffuint y = 1970;  y <= 2020;  y++) {
		dt.year = y;

		for (ffuint m = 1;  m <= 12;  m++) {
			dt.month = m;

			for (ffuint d = 1; ; d++) {

				static const ffuint month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
				if (d > month_days[m-1]) {
					int leap = y%4 == 0 && (y%100 != 0 || y%400 == 0);
					if (d == 29 && m == 2 && leap)
					{}
					else
						break;
				}

				dt.day = d;
				fftime_join(&t, &dt);
				xieq(cursec, t.sec);

				fftime_split(&dt, &t);
				xieq(y, dt.year);
				xieq(m, dt.month);
				xieq(d, dt.day);
				xieq(wd, dt.weekday);

				wd = (wd + 1) % 7;
				cursec += 24*60*60;

				// char buf[64];
				// ffsize n = fftime_tostr(&dt, buf, sizeof(buf), FFTIME_DATE_YMD);
				// printf("%.*s\n", (int)n, buf);
			}
		}
	}
}
