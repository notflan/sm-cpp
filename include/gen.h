#pragma once

#include "state.h"

typedef sm_yield (*sm_gen_fun)(sm_state* state); //NOTE: Same as sm_yield

// A generator method
struct sm_generator;

/// Create a generator method with this function.
sm_generator* sm_generate(sm_gen_fun function);
/// Free a generator method
void sm_free_generator(sm_generator* generator);
/// Run this generator until the next yield.
/// Returns false if the generator ends.
bool sm_next(sm_generator** gen, sm_state* state);
