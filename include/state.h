#pragma once

#include "frame.h"
#include <cassert>

typedef void* (*sm_yield)(sm_state* state); //NOTE: This `void*` is a stand-in for this function returning itself.

inline uint64_t _sm_confine_key(uint64_t ui)
{
	return ui & _SM_KEY_SIZE;
}

inline uint64_t _sm_compute_key(uint64_t name)
{
	return _sm_confine_key(name);
}

template<typename T>
inline T* _sm_replace(_sm_user* frame, T value)
{
	if(frame->set) _sm_free(frame);

	frame->set = true;
	frame->free = false;

	// Key is left the same

	return _sm_init(frame, value);
}

template<typename T>
inline T* _sm_replace_ip(_sm_user** frame, T value)
{

	if(! (*frame)) {
		*frame = box<_sm_user, true>();
	}
	return _sm_replace(*frame, value);	
}

template<typename T, typename U>
inline T* _sm_var(_sm_user_page* user, U name, T init)
{
	auto skey = name % _SM_STACK_SIZE;

	auto val = &user->data[skey];

	auto key = _sm_compute_key( (uint64_t)name );

	bool set = val->set;

	if(!set) {
		val->set = true;
		val->free = false;
		val->key = key;
		return _sm_init<T>(val, init);
	}
	else if(key == val->key) {
		//TODO: Check equality of value somehow
		return _sm_get<T>(val);
	} else {
		// Move to next page
		if(!user->next) user->next = box<_sm_user_page, true>();
		return _sm_var<T, U>(user->next, name, init);
	}
}
template<typename T, typename U>
inline T* _sm_var(sm_state* state, U name, T init)
{
	assert(state->current != nullptr);

	return _sm_var<T,U>(&state->current->user, name, init);
}
template<uint64_t name, typename T>
inline T* _sm_var(sm_state* state, T init)
{
	//static_assert(name < _SM_STACK_SIZE, "Line pseudo-hashmap not yet implemented");
	return _sm_var(state, name, init);
}

// Pop the current stack frame, and re-set the pointer to the previous one.
void _sm_pop_stack(sm_state* state);
void _sm_push_stack(sm_state* state);

sm_yield _sm_noop(sm_state*);

inline sm_yield sm_end() { return (sm_yield)nullptr; }
inline sm_yield sm_continue() { return (sm_yield)_sm_noop; }

/// --- For accessing generator-local variables stored in the `state` structure.

#define SM_VAR_EX(name, init) _sm_var(state, (name), (init)) // Same as `SM_VAR`, except the slot is explicitly named with a runtime integer value. Must be lower than `_SM_STACK_SIZE`. (TODO: Allow wrapping around this max value with a hashtable that adds new pages with the `next' pointer in the `_sm_user_page' struct.)
#define SM_VAR(init) _sm_var<__LINE__, decltype(init)>(state, (init)) // Return a pointer to a unique variable on the current state machine's stack. Initialising it with `init' if it doesn't already have a set value.
#define SM_SLOT(T) _sm_var<__LINE__, T>(state, {}) /// Allocate an uninitialised slot for type `T` on the current state machine's stack frame, returning a pointer to the value. If the value has already been allocated, return a pointer to that value saved in the current frame.

/// --- 

#define SM_BEGIN switch(state->current->pc) { case 0:
#define SM_END } return sm_end()

#define SM_YIELD(v) do { state->current->pc = __LINE__; return (sm_yield)(v); case __LINE__:; } while(0)
#define SM_YIELD_VALUE(v) do { _sm_replace_ip(&state->current->rval, (v)); SM_YIELD(sm_continue()); } while(0)

#define SM_RETURN() SM_YIELD(sm_end())
#define SM_RETURN_VALUE(v) do { _sm_replace_ip(&state->current->rval, (v)); SM_YIELD(sm_end()); } while(0)

#define SM_GENERATOR(name) sm_yield name(sm_state* state)

// ---

#define SM_GENERATOR(name) sm_yield name(sm_state* state)

// ---

sm_state* sm_new_state();
void sm_free_state(sm_state* state);
