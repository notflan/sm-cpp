# libsm

Contextual state machine (generator methods) implementation in basic C++, using mostly C constructs.

(TODO: Explain what is and isn't supported)

## Example

Output values of any type
``` c++
#include <sm/state.h>
#include <sm/gen.h>

#include <cstdio>

SM_GENERATOR(powers_of_two)
{
  int* a = SM_VAR(1);
  SM_BEGIN;
  {
    while( (*a) > 0 ) {
      SM_YIELD_VALUE(*a);
      (*a) <<= 1;
    }
  }
  SM_END;
}

int main()
{
  auto state = sm_new_state();
  auto gen = sm_generate(&powers_of_two);

  sm_output output;
  int out_int;
  while(sm_next(&gen, state, &output)) {
    if(!sm_output_value(output, &out_int))
      continue;
    printf("Power of two: %d\n", out_int);
  }

  sm_free_output(&output);

  sm_free_generator(gen);
  sm_free_state(state);

  return 0;
}
```
