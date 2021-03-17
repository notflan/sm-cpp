#include <frame.h>
#include <cstdio>

struct _test {
	int a, b;
};

int main()
{
	_test hi = { 0, 0 };
	auto _a = _sm_init(nullptr, hi);

	return 0;
}
