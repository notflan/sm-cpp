#include <state.h>
#include <gen.h>

#include <cstdio>

struct _test {
	int a, b;
};

SM_GENERATOR(sm_test_2)
{
	int* a = SM_VAR(-10);

	SM_BEGIN;
		while( (*a) < 0 ) {
			printf("(2) a = %d\n", *a);
			SM_YIELD_VALUE(*a);
			(*a)+=1;
		}
		printf("Done!\n");
	SM_END;
}

SM_GENERATOR(sm_test)
{
	int* a = SM_VAR(10);

	auto c = SM_SLOT(_test);
	float* d = SM_SLOT(float);

	*c = { 200, 300 };
	*d = 10.f;

	SM_BEGIN;
		*a = 5;
		SM_YIELD_VALUE(*a);
		printf("IN a = %d\n", *a);
		*a = 1;
		SM_YIELD_VALUE(*a);
		printf("IN a = %d\n", *a);
		//printf("Starting function 2\n");
		SM_YIELD(sm_test_2);
		//printf("2 done\n");

	SM_END;
}

int main()
{
	auto state = sm_new_state();
	auto gen = sm_generate(&sm_test);

	sm_output output;
	while(sm_next(&gen, state, &output)) {
		int out_int;
		if(!sm_output_value(output, &out_int))
			continue;
		printf("OUT \t\ta = %d\n", out_int);
	}

	sm_free_output(&output);
	sm_free_generator(gen);
	sm_free_state(state);

	/*
	//TODO: `sm_state` creation/initialisation & freeing functions
	//TODO: Test `sm_test`

	_test hi = { 0, 0 };
	auto _a = _sm_init(nullptr, hi);
	*/
	return 0;
}
