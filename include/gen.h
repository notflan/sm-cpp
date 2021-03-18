#pragma once

#include "state.h"

typedef sm_yield (*sm_gen_fun)(sm_state* state); //NOTE: Same as sm_yield

typedef _sm_user sm_output;

void sm_free_output(sm_output* data);

template<typename T>
inline const T* sm_output_ptr(const sm_output& data)
{
	if(data.set)
		return _sm_get<T>(const_cast<_sm_user*>(&data));
	else return nullptr;
}

template<typename T>
inline bool sm_output_value(const sm_output& data, T* output)
{
	auto ptr = sm_output_ptr<T>(data);
	if(ptr) {
		*output = *ptr;
		return true;
	} else return false;
}

template<typename T>
inline T sm_output_assume_value(const sm_output& data)
{
	return *sm_output_ptr<T>(data);
}

// A generator method
struct sm_generator;

/// Create a generator method with this function.
sm_generator* sm_generate(sm_gen_fun function);
/// Free a generator method
void sm_free_generator(sm_generator* generator);
/// Run this generator until the next yield.
/// Returns false if the generator ends.
/// Any output value yielded from the generator is returned in the output pointer, unless the pointer is NULL, in which case the output is discarded.
bool sm_next(sm_generator** gen, sm_state* state, sm_output* output);
