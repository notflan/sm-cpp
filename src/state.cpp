#include <state.h>
#include "dev.h"

void _sm_pop_stack(sm_state* state)
{
	TODO("Free the current stack frame in `state` and replace it with the previous one");
}

sm_yield _sm_noop(sm_state*)
{
	return (sm_yield)nullptr;
}

