/** ffbase/time.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/time.h>
#include <test/test.h>


#define STR2(s)  (char*)(s), FFS_LEN(s)

void test_time_tostr()
{
	ffdatetime dt = {};
	dt.year = 1;
	dt.month = 2;
	dt.day = 3;
	dt.hour = 4;
	dt.minute = 5;
	dt.second = 6;
	dt.nanosecond = 123456789;
	dt.weekday = 4;

	char buf[64];
	ffstr s;
	s.ptr = buf;
	s.len = fftime_tostr1(&dt, buf, sizeof(buf), FFTIME_YMD);
	xseq(&s, "0001-02-03 04:05:06");

	xieq(0, fftime_tostr1(&dt, buf, 19, FFTIME_YMD));
	x(0 != fftime_tostr1(&dt, buf, 20, FFTIME_YMD));

	s.len = fftime_tostr1(&dt, buf, sizeof(buf), FFTIME_WDMY);
	xseq(&s, "Thu, 03 Feb 0001 04:05:06 GMT");

	s.len = fftime_tostr1(&dt, buf, sizeof(buf), FFTIME_DATE_YMD | FFTIME_HMS_USEC);
	xseq(&s, "0001-02-03 04:05:06.123456");
}

void test_time_fromstr()
{
	ffdatetime dt;
	ffmem_zero_obj(&dt);
	xieq(19, fftime_fromstr1(&dt, STR2("1970-01-02 04:05:06xxx"), FFTIME_YMD));
	x(dt.year == 1970);
	x(dt.month == 1);
	xieq(dt.day, 2);
	x(dt.hour == 4);
	x(dt.minute == 5);
	x(dt.second == 6);
	x(dt.weekday == 0);

	xieq(10, fftime_fromstr1(&dt, STR2("1970-01-02xxx"), FFTIME_DATE_YMD));
	x(dt.year == 1970);
	x(dt.month == 1);
	xieq(dt.day, 2);

	ffmem_zero_obj(&dt);
	xieq(29, fftime_fromstr1(&dt, STR2("Thu, 01 Feb 1972 04:05:06 GMTxxx"), FFTIME_WDMY));
	x(dt.year == 1972);
	x(dt.month == 2);
	x(dt.day == 1);
	x(dt.hour == 4);
	x(dt.minute == 5);
	x(dt.second == 6);
	x(dt.weekday == 4);

	ffmem_zero_obj(&dt);
	xieq(12, fftime_fromstr1(&dt, STR2("01:02:03.456xxx"), FFTIME_HMS_MSEC));
	x(dt.hour == 1);
	x(dt.minute == 2);
	x(dt.second == 3);
	x(dt.nanosecond == 456000000);

	ffmem_zero_obj(&dt);
	xieq(12, fftime_fromstr1(&dt, STR2("01:02:03.456xxx"), FFTIME_HMS_MSEC_VAR));
	x(dt.hour == 1);
	x(dt.minute == 2);
	x(dt.second == 3);
	x(dt.nanosecond == 456000000);

	ffmem_zero_obj(&dt);
	xieq(6, fftime_fromstr1(&dt, STR2("03.456"), FFTIME_HMS_MSEC_VAR));
	x(dt.hour == 0);
	x(dt.minute == 0);
	x(dt.second == 3);
	x(dt.nanosecond == 456000000);

	ffmem_zero_obj(&dt);
	xieq(5, fftime_fromstr1(&dt, STR2("02:03"), FFTIME_HMS_MSEC_VAR));
	x(dt.hour == 0);
	x(dt.minute == 2);
	x(dt.second == 3);
	x(dt.nanosecond == 0);
}

void test_time()
{
	fftime t = {}, t2;
	ffdatetime dt = {};

	t.sec = 1;  t.nsec = 1;
	t2.sec = 2;  t2.nsec = 1;
	x(fftime_cmp(&t, &t2) < 0);

	t.sec = 2;  t.nsec = 1;
	t2.sec = 1;  t2.nsec = 1;
	x(fftime_cmp(&t, &t2) > 0);

	t.sec = 1;  t.nsec = 1;
	t2.sec = 1;  t2.nsec = 2;
	x(fftime_cmp(&t, &t2) < 0);

	t.sec = 1;  t.nsec = 2;
	t2.sec = 1;  t2.nsec = 1;
	x(fftime_cmp(&t, &t2) > 0);

	t.sec = 1;  t.nsec = 1;
	t2.sec = 1;  t2.nsec = 1;
	x(fftime_cmp(&t, &t2) == 0);

	// Jan 1, 1 AD
	dt.year = 1;
	dt.month = 1;
	dt.day = 1;
	fftime_join1(&t, &dt);
	xieq(0, t.sec);

	// (no date) 01:02:03
	ffmem_zero_obj(&dt);
	dt.hour = 1;
	dt.minute = 2;
	dt.second = 3;
	fftime_join1(&t, &dt);
	xieq(1*60*60 + 2*60 + 3, t.sec);

	// Dec 31, 1 AD, 23:59:59
	ffmem_zero_obj(&dt);
	dt.year = 1;
	dt.month = 12;
	dt.day = 31;
	dt.hour = 23;
	dt.minute = 59;
	dt.second = 59;
	fftime_join1(&t, &dt);
	xieq(365*24*60*60-1, t.sec);

	// Jan 1, 1970
	ffmem_zero_obj(&dt);
	dt.year = 1970;
	dt.month = 1;
	dt.day = 1;
	fftime_join1(&t, &dt);
	fftime_split1(&dt, &t);
	xieq(1970, dt.year);
	xieq(1, dt.month);
	xieq(1, dt.day);

	test_time_tostr();
	test_time_fromstr();

	// test each day since 1970
	ffuint64 cursec = FFTIME_1970_SECONDS;
	ffuint wd = 4;
	ffmem_zero_obj(&dt);
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
				fftime_join1(&t, &dt);
				xieq(cursec, t.sec);

				fftime_split1(&dt, &t);
				xieq(y, dt.year);
				xieq(m, dt.month);
				xieq(d, dt.day);
				xieq(wd, dt.weekday);

				wd = (wd + 1) % 7;
				cursec += 24*60*60;

				// char buf[64];
				// ffsize n = fftime_tostr1(&dt, buf, sizeof(buf), FFTIME_DATE_YMD);
				// printf("%.*s\n", (int)n, buf);
			}
		}
	}
}
