#include <state.h>

sm_yield _sm_noop(sm_state*)
{
	return (sm_yield)nullptr;
}
