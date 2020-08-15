/** ffbase: date/time functions
2020, Simon Zolin
*/

#pragma once

#ifndef _FFBASE_BASE_H
#include <ffbase/base.h>
#endif
#include <ffbase/string.h> // optional

/*
fftime_add fftime_sub
fftime_valid
fftime_join
fftime_split
fftime_tostr
*/

/** Time value */
typedef struct fftime {
	ffint64 sec;
	ffuint nsec;
} fftime;

/** Add time value */
static inline void fftime_add(fftime *t, const fftime *add)
{
	t->sec += add->sec;
	t->nsec += add->nsec;
	if (t->nsec >= 1000000000) {
		t->nsec -= 1000000000;
		t->sec++;
	}
}

/** Subtract time value */
static inline void fftime_sub(fftime *t, const fftime *sub)
{
	t->sec -= sub->sec;
	t->nsec -= sub->nsec;
	if ((int)t->nsec < 0) {
		t->nsec += 1000000000;
		t->sec--;
	}
}

/** Date/time parts */
typedef struct ffdatetime {
	int year;
	ffuint month; //1..12
	ffuint day; //1..31

	ffuint hour; //0..23
	ffuint minute;  //0..59
	ffuint second;  //0..59
	ffuint nanosecond; //0..999,999,999

	ffuint weekday; //0..6 (0:Sunday)
	ffuint yearday; //1..366
} ffdatetime;

/** Return TRUE if date and time values are in allowed range
Note: 'weekday' and 'yearday' are not checked */
static inline int fftime_valid(const ffdatetime *dt)
{
	if (!(dt->hour <= 23
		&& dt->minute <= 59
		&& dt->second <= 59
		&& dt->nanosecond <= 999999999))
		return 0;

	static const ffbyte month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int leap = dt->year%4 == 0 && (dt->year%100 != 0 || dt->year%400 == 0);
	if (!(dt->year != 0
		&& dt->month-1 < 12
		&& (dt->day-1 < month_days[dt->month - 1]
			|| (dt->day == 29 && dt->month == 2 && leap))))
		return 0;

	return 1;
}

/** Join time parts into a time value
Note: the time values in 'dt' are allowed to overflow
'weekday' and 'yearday' values are not read
If either 'year', 'month' or 'day' is 0, only time values are read and the date values are skipped
'year' < 0 isn't supported */
static inline void fftime_join(fftime *t, const ffdatetime *dt)
{
	t->sec = (ffint64)dt->hour*60*60 + dt->minute*60 + dt->second;
	t->nsec = dt->nanosecond;
	if (t->nsec >= 1000000000) {
		t->sec += t->nsec % 1000000000;
		t->nsec /= 1000000000;
	}

	if (dt->year <= 0 || dt->month == 0 || dt->day == 0)
		return;

	ffuint year = dt->year;
	ffuint mon = dt->month;
	if (mon > 12) {
		mon--;
		year += mon / 12;
		mon = (mon % 12) + 1;
	}
	mon -= 2; // Jan -> Mar
	if ((int)mon <= 0) {
		mon += 12;
		year--;
	}

	ffuint64 days = year*365 + year/4 - year/100 + year/400 - 365 // number of days passed since 1 AD
		+ (367 * mon / 12 - 30) + 31+28 // number of days since new year (Mar 1)
		+ dt->day-1;
	t->sec += days*60*60*24;
}

/** Split time value into date/time parts

Algorithm:
. Get day of week (1/1/1 was Monday).
. Get year and the days passed since its Mar 1:
  . Get days passed since Mar 1, 1 AD
  . Get approximate year (days / ~365.25).
  . Get days passed during the year.
. Get month and its day:
  . Get month by year day
  . Get year days passed before this month
  . Get day of month
. Shift New Year from Mar 1 to Jan 1
  . If year day is within Mar..Dec, add 2 months
  . If year day is within Jan..Feb, add 2 months and increment year
*/
static inline void fftime_split(ffdatetime *dt, const fftime *t)
{
	ffuint64 sec = t->sec;
	dt->nanosecond = t->nsec;

	ffuint days = sec / (60*60*24);
	dt->weekday = (1 + days) % 7;

	sec = sec % (60*60*24);
	dt->hour = sec / (60*60);
	dt->minute = (sec % (60*60)) / 60;
	dt->second = sec % 60;

	days += 306; // 306: days from Mar before Jan
	ffuint year = 1 + days * 400 / (400*365 + 400/4 - 400/100 + 400/400);
	ffuint yday = days - (year*365 + year/4 - year/100 + year/400);
	if ((int)yday < 0) {
		int leap = year%4 == 0 && (year%100 != 0 || year%400 == 0);
		yday += 365 + leap;
		year--;
	}

	ffuint mon = (yday + 31) * 10 / 306; // get month by year day (1: March)
	ffuint mday = 367 * mon / 12 - 30; // get year days passed before this month (1: March)
	mday = yday - mday;
	dt->day = mday + 1;

	if (yday >= 306) {
		year++;
		mon -= 10;
		yday -= 306;
	} else {
		mon += 2;
		int leap = year%4 == 0 && (year%100 != 0 || year%400 == 0);
		yday += 31 + 28 + leap;
	}

	dt->year = year;
	dt->month = mon;
	dt->yearday = yday + 1;
}

enum FFTIME_FMT {
	// date
	FFTIME_DATE_YMD = 1, // yyyy-MM-dd
	FFTIME_DATE_WDMY, // Wed, dd Sep yyyy
	FFTIME_DATE_DMY, // dd-MM-yyyy
	FFTIME_DATE_MDY, // MM/dd/yyyy

	// time
	FFTIME_HMS = 0x10, // hh:mm:ss
	FFTIME_HMS_MSEC = 0x20, // hh:mm:ss.msc
	FFTIME_HMS_GMT = 0x30, // hh:mm:ss GMT
	FFTIME_HMS_MSEC_VAR = 0x40, // [[h:]m:]s[.ms] (optional hour, minute and millisecond)

	// date & time:
	FFTIME_YMD = FFTIME_DATE_YMD | FFTIME_HMS, // yyyy-MM-dd hh:mm:ss (ISO 8601)
	FFTIME_WDMY = FFTIME_DATE_WDMY | FFTIME_HMS_GMT, // Wed, dd Sep yyyy hh:mm:ss GMT (RFC1123)
};

#ifdef _FFBASE_STRFORMAT_H

/** Convert date/time to string
flags: enum FFTIME_FMT
Return N of bytes written;  0 on error */
static inline ffsize fftime_tostr(const ffdatetime *dt, char *dst, ffsize cap, ffuint flags)
{
	ffsize i = 0;
	static const char week_days[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	switch (flags & 0x0f) {
	case FFTIME_DATE_YMD:
		i += ffs_format(dst, cap, "%04u-%02u-%02u"
			, dt->year, dt->month, dt->day);
		break;

	case FFTIME_DATE_WDMY:
		i += ffs_format(dst, cap, "%s, %02u %s %04u"
			, week_days[dt->weekday], dt->day, months[dt->month - 1], dt->year);
		break;

	case FFTIME_DATE_MDY:
		i += ffs_format(dst, cap, "%02u/%02u/%04u"
			, dt->month, dt->day, dt->year);
		break;

	case FFTIME_DATE_DMY:
		i += ffs_format(dst, cap, "%02u.%02u.%04u"
			, dt->day, dt->month, dt->year);
		break;

	case 0:
		break; // no date

	default:
		return 0; // unknown date format
	}

	if (!!(flags & 0x0f) && !!(flags & 0xf0) && i < cap)
		dst[i++] = ' ';

	switch (flags & 0xf0) {
	case FFTIME_HMS:
		i += ffs_format(dst + i, cap - i, "%02u:%02u:%02u"
			, dt->hour, dt->minute, dt->second);
		break;

	case FFTIME_HMS_MSEC:
		i += ffs_format(dst + i, cap - i, "%02u:%02u:%02u.%03u"
			, dt->hour, dt->minute, dt->second, dt->nanosecond / 1000000);
		break;

	case FFTIME_HMS_GMT:
		i += ffs_format(dst + i, cap - i, "%02u:%02u:%02u GMT"
			, dt->hour, dt->minute, dt->second);
		break;

	case 0:
		break; // no time

	default:
		return 0; // unknown time format
	}

	if (i == cap)
		return 0;
	return i;
}
#endif // _FFBASE_STRFORMAT_H