#pragma once

#include "frame.h"

typedef void* (*sm_yield)(sm_state* state); //NOTE: This `void*` is a stand-in for this function returning itself.

template<typename T, typename U>
inline T* _sm_var(sm_state* state, U name, T init)
{
	auto val = &state->current->user.data[name];
	bool set = val->set;
	val->set = true;
	val->free = false;
	return ( ! set ) ?
		_sm_init<T>(val, init) :
		_sm_get<T>(val);
}
template<uint64_t name, typename T>
inline T* _sm_var(sm_state* state, T init)
{
	static_assert(name < _SM_STACK_SIZE, "Line pseudo-hashmap not yet implemented");
	return _sm_var(state, name, init);
}

// Pop the current stack frame, and re-set the pointer to the previous one.
void _sm_pop_stack(sm_state* state);

sm_yield _sm_noop(sm_state*);

inline sm_yield sm_end() { return (sm_yield)nullptr; }
inline sm_yield sm_continue() { return (sm_yield)_sm_noop; }

#define SM_VAR_EX(name, init) _sm_var(state, (name), (init)) //TODO: Replace `name' with (__LINE__ % _SM_STACK_SIZE) pseudo-hashmap
#define SM_VAR(init) _sm_var<__LINE__, decltype(init)>(state, (init))

#define SM_BEGIN switch(state->current->pc) { case 0:
#define SM_END } _sm_pop_stack(state); return sm_end()

#define SM_YIELD(v) do { state->current->pc = __LINE__; return (sm_yield)(v); case __LINE__:; } while(0)

