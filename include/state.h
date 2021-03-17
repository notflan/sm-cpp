#pragma once

#include "frame.h"

typedef void* (*sm_yield)(sm_state* state); //NOTE: This `void*` is a stand-in for this function returning itself.

template<typename T, typename U>
inline T* _sm_var(sm_state* state, U name, T init)
{
	auto val = &state->current->user.data[name];
	bool set = val->set;

	val->set = true;
	

	return ( ! set ) ?
		(val->free = false, _sm_init<T>(val, init)) :
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

/// --- For accessing generator-local variables stored in the `state` structure.

#define SM_VAR_EX(name, init) _sm_var(state, (name), (init)) // Same as `SM_VAR`, except the slot is explicitly named with a runtime integer value. Must be lower than `_SM_STACK_SIZE`. (TODO: Allow wrapping around this max value with a hashtable that adds new pages with the `next' pointer in the `_sm_user_page' struct.)
#define SM_VAR(init) _sm_var<__LINE__, decltype(init)>(state, (init)) // Return a pointer to a unique variable on the current state machine's stack. Initialising it with `init' if it doesn't already have a set value.
#define SM_SLOT(T) _sm_var<__LINE__, T>(state, {}) /// Allocate an uninitialised slot for type `T` on the current state machine's stack frame, returning a pointer to the value. If the value has already been allocated, return a pointer to that value saved in the current frame.

/// --- 

#define SM_BEGIN switch(state->current->pc) { case 0:
#define SM_END } _sm_pop_stack(state); return sm_end()

#define SM_YIELD(v) do { state->current->pc = __LINE__; return (sm_yield)(v); case __LINE__:; } while(0)

sm_state* sm_new();
void sm_free(sm_state* state);
