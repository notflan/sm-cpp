#pragma once

#include "box.h"

#include <cstdint>
#define _SM_STACK_SIZE 15

#define _SM_KEY_SIZE (UINT64_MAX >> 2)

struct _sm_user {
	uint64_t set : 1;  // bool
	uint64_t free : 1; // bool

	uint64_t key : 62; // Use the rest of the 7 byte hole here for the key.

	union {
		uint8_t _8;
		uint16_t _16;
		uint32_t _32;
		uint64_t _64;

		float _f32;
		double _f64;		

		void* _ptr;
	};
};

struct _sm_user_page {
	_sm_user data[_SM_STACK_SIZE];

	_sm_user_page* next;
};

struct _sm_frame {
	uint64_t pc;
	_sm_user_page user;

	_sm_frame* prev;
};

struct sm_state {
	

	_sm_frame* current;
};

template<typename T>
inline T* _sm_init(_sm_user* frame, T init)
{
	frame->free = true;

	T* ptr = box<T>();
	*ptr = init;
	return (T*) (frame->_ptr = (void*)ptr);
}

template<typename T>
inline T** _sm_init(_sm_user* frame, T* init)
{
	frame->free = false;
	frame->_ptr = (void*)init;

	return (T**) &frame->_ptr;
}

template<typename T>
inline T* _sm_get(_sm_user* frame)
{
	return (T*) frame->_ptr;
}

inline void _sm_free(_sm_user* frame)
{
	if (frame->free)
		free(frame->_ptr);
	frame->free = frame->set = false;
}

#define _SM_DEF(T) template<> T* _sm_init<T>(_sm_user* frame, T init); \
		template<> T* _sm_get<T>(_sm_user* frame)

_SM_DEF(char);
_SM_DEF(unsigned char);
_SM_DEF(short);
_SM_DEF(unsigned short);
_SM_DEF(int);
_SM_DEF(unsigned int);
_SM_DEF(long);
_SM_DEF(unsigned long);
_SM_DEF(float);
_SM_DEF(double);

#undef _SM_DEF
