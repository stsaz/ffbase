/** ffbase: cpuid.h tester
2017, Simon Zolin
*/

#include <ffbase/cpuid.h>
#include <test/test.h>

void test_cpu()
{
	struct ffcpuid c = {};
	x(0 == ffcpuid(&c, FFCPUID_VENDOR | FFCPUID_FEATURES | FFCPUID_BRAND));
	xlog("CPU:%s\nBrand:%s\n"
		"SSE:%u\nSSE2:%u\nSSE3:%u\nSSSE3:%u\nSSE4.1:%u\nSSE4.2:%u\n"
		"POPCNT:%u\n",
		c.vendor, c.brand,
		ffcpuid_feat(&c, FFCPUID_SSE),
		ffcpuid_feat(&c, FFCPUID_SSE2),
		ffcpuid_feat(&c, FFCPUID_SSE3),
		ffcpuid_feat(&c, FFCPUID_SSSE3),
		ffcpuid_feat(&c, FFCPUID_SSE41),
		ffcpuid_feat(&c, FFCPUID_SSE42),
		ffcpuid_feat(&c, FFCPUID_POPCNT)
		);
}
