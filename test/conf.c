/** ffbase/conf.h tester */

#include <ffbase/conf.h>
#include <test/test.h>

void test_conf()
{
	struct ffconf c = {};
	ffstr in, out;

	ffstr_setz(&in, "k");
	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "k");
	x(!(c.flags & FFCONF_FKEY));
	xieq(FFCONF_MORE, ffconf_read(&c, &in, &out));
	ffmem_zero_obj(&c);

	ffstr_setz(&in,
"  key  val1 val2 \"val w space\" \"val w\\\" quote\" \"\\\\val w\\x01 binary\" \"val\n\
w\n\
newline\" val4 \"\"  # comment \n\
  key1  val1\n");

	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xseq(&out, "key");
	xieq(0, c.line);

	xieq(FFCONF_VAL, ffconf_read(&c, &in, &out));
	xseq(&out, "val1");
	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, "val2");

	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, "val w space");
	x(!!(c.flags & FFCONF_FKEY));
	x(!!(c.flags & FFCONF_FQUOTED));

	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "val w");
	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "\"");
	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, " quote");

	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "\\");
	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "val w");
	xieq(FFCONF_CHUNK, ffconf_read(&c, &in, &out));
	xseq(&out, "\x01");
	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, " binary");

	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, "val\nw\nnewline");
	xieq(2, c.line);

	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, "val4");
	x(!(c.flags & FFCONF_FQUOTED));

	xieq(FFCONF_VAL_NEXT, ffconf_read(&c, &in, &out));
	xseq(&out, "");

	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xseq(&out, "key1");
	xieq(3, c.line);

	xieq(FFCONF_VAL, ffconf_read(&c, &in, &out));
	xseq(&out, "val1");

	xieq(FFCONF_MORE, ffconf_read(&c, &in, &out));

	ffstr_setz(&in, "\"key w space\" val\n");
	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xseq(&out, "key w space");
	x(in.ptr[-1] == '"');
	xieq(FFCONF_VAL, ffconf_read(&c, &in, &out));
	xseq(&out, "val");

	ffmem_zero_obj(&c);
	ffstr_setz(&in, "ke\"y "); // require whitespace before quoted text
	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xieq(FFCONF_ERROR, ffconf_read(&c, &in, &out));
	xieq(FFS_LEN("ke"), c.line_off);

	ffmem_zero_obj(&c);
	ffstr_setz(&in, "\"key\"1 "); // require whitespace after quoted text
	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xseq(&out, "key");
	xieq(FFCONF_ERROR, ffconf_read(&c, &in, &out));
	xieq(FFS_LEN("\"key\""), c.line_off);

	ffmem_zero_obj(&c);
	ffstr_setz(&in, "k\x01""ey"); // non-printable character
	xieq(FFCONF_KEY, ffconf_read(&c, &in, &out));
	xseq(&out, "k");
	xieq(FFCONF_ERROR, ffconf_read(&c, &in, &out));
	xieq(FFS_LEN("k"), c.line_off);

	ffmem_zero_obj(&c);
	ffstr_setz(&in, "\"k\x01""ey\""); // non-printable character within quotes
	xieq(FFCONF_ERROR, ffconf_read(&c, &in, &out));
	xieq(FFS_LEN("\"k"), c.line_off);
}
