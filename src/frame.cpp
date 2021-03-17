#include <frame.h>

#include <cstdio>

#define BODY(T, U, nm) \
template<> T* _sm_init<T>(_sm_user* frame, T init) \
{ \
	frame->free = false; \
	frame-> nm = (U)init; \
	return (T*)&frame-> nm; \
} \
template<> T* _sm_get<T>(_sm_user* frame) \
{ \
	return (T*)&frame-> nm; \
}

#define BODY_T(T, S) BODY(T, uint ## S ## _t, _ ## S)

#define BODY_INT(T, S) BODY_T(T, S) \
			BODY_T(unsigned T, S)

BODY_INT(char, 8)
BODY_INT(short, 16)
BODY_INT(int, 32)
BODY_INT(long, 64)

BODY(float, float, _f32)
BODY(double, double, _f64)
