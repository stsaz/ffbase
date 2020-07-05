/** ffbase/str-convert.h tester
2020, Simon Zolin
*/

#include <ffbase/string.h>
#include <test/test.h>


#define STR(s)  (char*)(s), FFS_LEN(s)

void test_ffstr_toint()
{
	ffstr s = {};
	ffuint64 u64 = 0;
	ffint64 i64 = 0;
	ffuint u32 = 0;
	int i32 = 0;
	ffushort u16 = 0;
	short i16 = 0;
	ffbyte u8 = 0;
	char i8 = 0;

	ffstr_setz(&s, "");
	x(!ffstr_to_uint64(&s, &u64));

	ffstr_setz(&s, ":");
	x(!ffstr_to_uint64(&s, &u64));

	ffstr_setz(&s, "-");
	x(!ffstr_to_uint64(&s, &u64));

// leading zeros
	ffstr_setz(&s, "018446744073709551615");
	x(ffstr_to_uint64(&s, &u64));  x(u64 == 18446744073709551615ULL);

// unsigned 64-bit
	ffstr_setz(&s, "18446744073709551616");
	x(!ffstr_to_uint64(&s, &u64)); // OF unsigned 64-bit

	ffstr_setz(&s, "18446744073709551615");  u64 = 0;
	x(ffstr_to_uint64(&s, &u64));  x(u64 == 18446744073709551615ULL);

// signed 64-bit
	// ffstr_setz(&s, "-9223372036854775809");
	// x(!ffstr_to_int64(&s, &i64)); // OF signed 64-bit

	// ffstr_setz(&s, "-9223372036854775808");
	// x(ffstr_to_int64(&s, &i64));  x(i64 == (ffint64)0x8000000000000000ULL);

	// ffstr_setz(&s, "9223372036854775808");
	// x(!ffstr_to_int64(&s, &i64)); // OF signed 64-bit

	ffstr_setz(&s, "+9223372036854775807");
	x(ffstr_to_int64(&s, &i64));  x(i64 == 9223372036854775807LL);

// unsigned 32-bit
	ffstr_setz(&s, "4294967296");
	x(!ffstr_to_uint32(&s, &u32)); // OF unsigned 32-bit

	ffstr_setz(&s, "4294967295");  u32 = 0;
	x(ffstr_to_uint32(&s, &u32));  x(u32 == 4294967295);

// signed 32-bit
	// ffstr_setz(&s, "-2147483649");
	// x(!ffstr_to_int32(&s, &i32)); // OF signed 32-bit

	ffstr_setz(&s, "-2147483648");
	x(ffstr_to_int32(&s, &i32));  x(i32 == -2147483648);

	// ffstr_setz(&s, "2147483648");
	// x(!ffstr_to_int32(&s, &i32)); // OF signed 32-bit

	ffstr_setz(&s, "+2147483647");
	x(ffstr_to_int32(&s, &i32));  x(i32 == 2147483647);

// unsigned 16-bit
	ffstr_setz(&s, "65536");
	x(!ffstr_toint(&s, &u16, FFS_INT16)); // OF unsigned 16-bit

	ffstr_setz(&s, "65535");  u16 = 0;
	x(ffstr_toint(&s, &u16, FFS_INT16));  x(u16 == 65535);

// signed 16-bit
	// ffstr_setz(&s, "-2147483649");
	// x(!ffstr_to_int16(&s, &i16)); // OF signed 16-bit

	ffstr_setz(&s, "-32768");
	x(ffstr_toint(&s, &i16, FFS_INT16 | FFS_INTSIGN));  x(i16 == -32768);

	// ffstr_setz(&s, "2147483648");
	// x(!ffstr_to_int16(&s, &i16)); // OF signed 16-bit

	ffstr_setz(&s, "+32767");
	x(ffstr_toint(&s, &i16, FFS_INT16 | FFS_INTSIGN));  x(i16 == 32767);

// unsigned 8-bit
	ffstr_setz(&s, "256");
	x(!ffstr_toint(&s, &u8, FFS_INT8)); // OF unsigned 8-bit

	ffstr_setz(&s, "255");  u8 = 0;
	x(ffstr_toint(&s, &u8, FFS_INT8));  x(u8 == 255);

// signed 8-bit
	// ffstr_setz(&s, "-129");
	// x(!ffstr_toint(&s, &i8, FFS_INT8 | FFS_INTSIGN)); // OF signed 8-bit

	ffstr_setz(&s, "-128");
	x(ffstr_toint(&s, &i8, FFS_INT8 | FFS_INTSIGN));  x(i8 == -128);

	// ffstr_setz(&s, "128");
	// x(!ffstr_toint(&s, &i8, FFS_INT8 | FFS_INTSIGN)); // OF signed 8-bit

	ffstr_setz(&s, "+127");
	x(ffstr_toint(&s, &i8, FFS_INT8 | FFS_INTSIGN));  x(i8 == 127);
}

void test_ffstr_toint_hex()
{
	ffstr s = {};
	ffuint64 u64 = 0;

// leading zeros
	ffstr_setz(&s, "0ffffffffffffffff");
	x(ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTHEX));  x(u64 == 0xffffffffffffffffULL);

// unsigned 64-bit
	ffstr_setz(&s, "10000000000000000");
	x(!ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTHEX)); // OF 64-bit

	ffstr_setz(&s, "ffffffffffffffff");
	x(ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTHEX));  x(u64 == 0xffffffffffffffffULL);
}

void test_ffstr_toint_oct()
{
	ffstr s = {};
	ffuint64 u64 = 0;

// leading zeros
	ffstr_setz(&s, "01777777777777777777777");
	x(ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTOCTAL));  x(u64 == 0xffffffffffffffffULL);

// unsigned 64-bit
	ffstr_setz(&s, "2777777777777777777777");
	x(!ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTOCTAL)); // OF 64-bit

	ffstr_setz(&s, "1777777777777777777777");
	x(ffstr_toint(&s, &u64, FFS_INT64 | FFS_INTOCTAL));  x(u64 == 0xffffffffffffffffULL);
}

void test_ffstr_fromint()
{
	char buf[32];
	ffstr s;
	ffstr_set(&s, buf, 0);

	x(!ffstr_from_uint(&s, 19, 0xffffffffffffffffULL));
	x(ffstr_from_uint(&s, 20, 0xffffffffffffffffULL));	xseq(&s, "18446744073709551615");
	x(ffstr_from_int(&s, 2, 0xffffffffffffffffULL));	xseq(&s, "-1");
	x(ffstr_fromint(&s, 22, 0xffffffffffffffffULL, FFS_INTOCTAL));	xseq(&s, "1777777777777777777777");
	x(ffstr_fromint(&s, 16, 0xffffffffffffffffULL, FFS_INTHEX));	xseq(&s, "ffffffffffffffff");
	x(ffstr_fromint(&s, sizeof(buf), 0xffffffffffffffffULL, FFS_INTHEX | FFS_INTHEXUP));	xseq(&s, "FFFFFFFFFFFFFFFF");
	x(ffstr_fromint(&s, sizeof(buf), 0xffffffffffffffffULL, FFS_INTSEP1000));	xseq(&s, "18,446,744,073,709,551,615");

	x(ffstr_fromint(&s, sizeof(buf), 999, FFS_INTSEP1000));	xseq(&s, "999");

	x(ffstr_fromint(&s, sizeof(buf), 1234, FFS_INTWIDTH(8)));
	xseq(&s, "    1234");
	x(ffstr_fromint(&s, sizeof(buf), -1234, FFS_INTWIDTH(8) | FFS_INTSIGN));
	xseq(&s, "   -1234");
	x(ffstr_fromint(&s, sizeof(buf), 1234, FFS_INTWIDTH(2)));
	xseq(&s, "1234");
	x(ffstr_fromint(&s, sizeof(buf), -1234, FFS_INTWIDTH(2) | FFS_INTSIGN));
	xseq(&s, "-1234");
	x(ffstr_fromint(&s, sizeof(buf), 1234, FFS_INTWIDTH(8) | FFS_INTZERO));
	xseq(&s, "00001234");
	x(ffstr_fromint(&s, sizeof(buf), -1234, FFS_INTWIDTH(8) | FFS_INTSIGN | FFS_INTZERO));
	xseq(&s, "-0001234");
	x(ffstr_fromint(&s, sizeof(buf), 1234, FFS_INTWIDTH(2) | FFS_INTZERO));
	xseq(&s, "1234");
	x(ffstr_fromint(&s, sizeof(buf), -1234, FFS_INTWIDTH(2) | FFS_INTSIGN | FFS_INTZERO));
	xseq(&s, "-1234");
	x(ffstr_fromint(&s, sizeof(buf), -1234, FFS_INTWIDTH(6) | FFS_INTSIGN | FFS_INTKEEPSIGN));
	xseq(&s, " -1234");
	x(ffstr_fromint(&s, sizeof(buf), 1234, FFS_INTWIDTH(6) | FFS_INTSIGN | FFS_INTKEEPSIGN));
	xseq(&s, " +1234");
}

void test_ffs_tofloat()
{
	double d;

	xieq(1, ffs_tofloat(STR("1/"), &d, 0));	x(d == 1);
	xieq(1, ffs_tofloat(STR("1,"), &d, 0));	x(d == 1);
	xieq(2, ffs_tofloat(STR("30#"), &d, 0));	x(d == 30);
	x(0 != ffs_tofloat(STR("1."), &d, 0));	x(d == 1.);
	x(0 != ffs_tofloat(STR(".1"), &d, 0));	x(d == .1);
	x(0 != ffs_tofloat(STR("1.1"), &d, 0));	x(d == 1.1);
	x(0 != ffs_tofloat(STR("0.0"), &d, 0));	x(d == 0.0);
	x(0 != ffs_tofloat(STR("123.4567"), &d, 0));	x(d == 123.4567);
	x(0 != ffs_tofloat(STR("18446744073709551615999"), &d, 0));	xieq(18446744073709552ULL, (ffuint64)(d/1000000));

	x(0 != ffs_tofloat(STR("1e5"), &d, 0));	x(d == 1e5);
	x(0 != ffs_tofloat(STR("1e+6"), &d, 0));	x(d == 1e+6);
	x(0 != ffs_tofloat(STR("1e-7"), &d, 0));	x(d == 1e-7);
	x(0 != ffs_tofloat(STR("1.e-8"), &d, 0));	x(d == 1.e-8);
	x(0 != ffs_tofloat(STR("1.1e-9"), &d, 0));	x(d == 1.1e-9);
	x(0 != ffs_tofloat(STR(".1e-12"), &d, 0));	x(d == .1e-12);
	x(0 != ffs_tofloat(STR("-.1e-12"), &d, 0));	x(d == -.1e-12);
	x(0 != ffs_tofloat(STR("+.1e-12"), &d, 0));	x(d == +.1e-12);
	x(0 != ffs_tofloat(STR("1.123e+6"), &d, 0));	x(d == 1.123e+6);
	x(0 != ffs_tofloat(STR("123.456e-052"), &d, 0));	x(d == 123.456e-052);
	x(0 != ffs_tofloat(STR("1e-323"), &d, 0));	x(d == 1e-323);
	x(0 != ffs_tofloat(STR("1e50/"), &d, 0));	x(d == 1e50);

	xieq(0, ffs_tofloat(STR("-"), &d, 0));
	xieq(0, ffs_tofloat(STR("+"), &d, 0));
	xieq(0, ffs_tofloat(STR("1e"), &d, 0));
	xieq(0, ffs_tofloat(STR("e-1"), &d, 0));
	xieq(0, ffs_tofloat(STR(".e-1"), &d, 0));
}

void test_ffstr_fromfloat()
{
	char buf[32];
	ffstr s;
	ffstr_set(&s, buf, 0);
	ffuint prec = 17;

	x(ffstr_from_float(&s, sizeof(buf), 0.0, prec));	xseq(&s, "0");
	x(ffstr_from_float(&s, sizeof(buf), -0.0, prec));	xseq(&s, "0");
	x(ffstr_from_float(&s, sizeof(buf), 1.2, prec));	xseq(&s, "1.19999999999999996");
	x(ffstr_from_float(&s, sizeof(buf), 1.2345, 2));	xseq(&s, "1.23");

	x(!ffstr_from_float(&s, 16, 12345678912345678.0, prec));
	x(ffstr_from_float(&s, 17, 12345678912345678.0, prec));	xseq(&s, "12345678912345678");

	x(ffstr_from_float(&s, sizeof(buf), -12345678912345678.0, prec));	xseq(&s, "-12345678912345678");

	x(ffstr_from_float(&s, sizeof(buf), 0.123, 0));	xseq(&s, "0");
	x(ffstr_from_float(&s, sizeof(buf), 0.123, 1));	xseq(&s, "0.1");
	x(ffstr_from_float(&s, sizeof(buf), 0.123456789123456789, prec));	xseq(&s, "0.12345678912345678");

	x(ffstr_from_float(&s, 20, -0.123456789123456789, prec));	xseq(&s, "-0.12345678912345678");
	x(!ffstr_from_float(&s, 19, -0.123456789123456789, prec));

	x(ffstr_from_float(&s, sizeof(buf), 0.000123456789123456789, prec));	xseq(&s, "0.00012345678912346");

	x(ffstr_from_float(&s, sizeof(buf), INFINITY, prec));	xseq(&s, "inf");
	x(ffstr_from_float(&s, sizeof(buf), -INFINITY, prec));	xseq(&s, "-inf");
	x(ffstr_from_float(&s, sizeof(buf), NAN, prec));	xseq(&s, "nan");
}

void test_ffs_fromhex()
{
	char ss[8];
	ffstr s;
	ffstr_set(&s, ss, 0);
	s.len = ffs_fromhex(ss, 6, "0\x00\xff", 3, 0);
	xseq(&s, "3000ff");

	s.len = ffs_fromhex(ss, 6, "0\x00\xff", 3, FFS_INTHEXUP);
	xseq(&s, "3000FF");

	x(6 == ffs_fromhex(NULL, 0, "0\x00\xff", 3, 0));
	x(0 == ffs_fromhex(ss, 5, "0\x00\xff", 3, 0));

	xieq(3, ffs_tohex(ss, sizeof(ss), STR("3000FF")));
	x(!ffmem_cmp(ss, "0\x00\xff", 3));
}

void test_str_conv()
{
	test_ffstr_toint();
	test_ffstr_toint_hex();
	test_ffstr_toint_oct();
	test_ffstr_fromint();
	test_ffs_tofloat();
	test_ffstr_fromfloat();
	test_ffs_fromhex();
}
