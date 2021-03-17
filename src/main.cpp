#include <state.h>
#include <cstdio>

struct _test {
	int a, b;
};

sm_yield sm_test(sm_state* state)
{
	int* a = SM_VAR(10);

	auto c = SM_SLOT(_test);
	float* d = SM_SLOT(float);

	*c = { 200, 300 };
	*d = 10.f;

	SM_BEGIN;
		SM_YIELD(sm_continue());
		*a = 5;
		SM_YIELD(sm_continue());
		*a = 0;
	SM_END;
}

int main()
{

	//TODO: `sm_state` creation/initialisation & freeing functions
	//TODO: Test `sm_test`

	_test hi = { 0, 0 };
	auto _a = _sm_init(nullptr, hi);

	return 0;
}
