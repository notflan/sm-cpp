#include <gen.h>

#define EX_C extern "C"

struct sm_generator
{
	sm_generator* prev;
	sm_gen_fun current;
};

EX_C sm_generator* sm_generate(sm_gen_fun function)
{
	auto gen = box<sm_generator>();
	gen->prev = nullptr;
	gen->current = function;
	return gen;
}

EX_C void sm_free_generator(sm_generator* gen)
{
	auto g = unbox(gen);
	if(g.prev)
		sm_free_generator(g.prev);
}

// Returns false if the generator ends.
EX_C bool sm_next(sm_generator** gen, sm_state* state, _sm_user* output)
{
	if( !(*gen)->current) return false;

	sm_yield next = (*gen)->current(state);

	if(sm_state_getopt(state, INTERMEDIATE_RETURNS) || next == nullptr) {
		// Set the return value to `output`.
		if(output && state->current->rval) *output = *state->current->rval;
		else if(output) output->set = false;
	}

	switch((uintptr_t)next)
	{
		case 0:
			// Pop the current
			(*gen)->current = nullptr;

			if ((*gen)->prev) {

				// Pop back to previous stack
				_sm_pop_stack(state);

				*gen = unbox(*gen).prev;
				return true;
			} else return false;
		default:
			if (next == (sm_yield)_sm_noop) return true;

			auto prev = *gen;
			*gen = sm_generate((sm_gen_fun) next);
			(*gen)->prev = prev;

			// Set up new stack frame
			_sm_push_stack(state);

			break;
	}
	return true;
}
