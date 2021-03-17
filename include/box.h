//! Deals with heap allocating (`boxing') types and values. 
//!
//! Nameapace(s):
//!	API functions/macros start with (or are) `box`. `unbox`, `BOX`, and `UNBOX`.
//!	Internal impl. functions/macros start with (or are) `_box`, `_BOX`, `_unbox`, and `_UNBOX`.
//!
//! Provides macros for C and C++:
//! * BOX(T)		- Allocate a properly aligned, uninitliased `T*` pointer on the heap.
//! * BOX_EX(T)		- Allocate a properly aligned, zero-initialised `T*` pointer on the heap.
//! * BOX_WITH(T, val)	- Allocate a properly aligned `T*` pointer on the heap with the value of `val`.
//! * UNBOX(val)	- Free the memory of boxed pointer `val`, then return the value itself.
//! * UNBOX_EX(val)	- Explicitly zero and then free the memory of boxed pointer `val`, then return the value itself.
//!
//! Provides templates for C++:
//! * T* box<T>()			- Same as `BOX(T)`
//! * T* box<T, true>()			- Same as `BOX_EX(T)`
//! * T* box<T>(T value)		- Same as `BOX_WITH(T, value)`
//! * T unbox(T* value)			- Same as `UNBOX(value)`
//! * T unbox<T, true>(T* value)	- Same as `UNBOX_EX(value)`
//!
//! # Notes
//! `UNBOX_EX` and `unbox<T, true>` use `explicit_bzero` to blank the memory before free.
//! Whereas `BOX_EX` and `box<T, true>` just use `bzero`.
//!
//! This API is **not** designed to work with C++ classes, constructors, or destructors. It is designed for C types and structs. Constructors and destructors are not called or supported. Only use with C-like (so-called ``POD'') types without con/destructors. No default initialisation is done (besides zero-initialisation when requested).
//! To override this behaviour and use C++ new/delete instead of C malloc/free, you can define the _BOX_CPP preprocessor directive. However, C++ move semantics are not used here at all so using this API with non-POD types will cause lots of useless copy-constructors being called. Do not use this with non-trivially constructable types. Ideally, only ever use C ("POD") types with this.

#ifndef _BOX_H
#define _BOX_H

#ifdef __cplusplus

#ifdef _BOX_CPP
#warning "Using BOX with non-POD types is discouraged"
#endif

#include <cstdlib>
#include <cstring>
//#include <strings.h>

template<typename T, bool Zero = false>
inline T* box()
{
	void* ptr = 
#ifdef _BOX_CPP
	new T();
#else
	aligned_alloc(alignof(T), sizeof(T));
#endif
	if constexpr(Zero) bzero(ptr, sizeof(T));

	return (T*)ptr;
}

template<typename T>
inline T* box(T val)
{
	T* ptr = box<T>();
	*ptr = val;
	return ptr;
}

template<typename T, bool Zero = false>
inline T unbox(T* boxed)
{
	T val = *boxed;
	if constexpr(Zero) explicit_bzero((void*)boxed, sizeof(T));
#ifdef _BOX_CPP
	delete boxed;
#else
	free((void*)boxed);
#endif
	return val;
}

template<typename T>
inline static T _unbox_explicit(T* boxed)
{
	return unbox<T, true>(boxed);
}

#define BOX(T) box<T>()
#define BOX_EX(T) box<T, true>()
#define BOX_WITH(T, val) box<T>(val)

#define UNBOX_EX(val) _unbox_explicit(val)
#define UNBOX(val) unbox(val)

#else

#include <stdlib.h>
#include <string.h>

#define BOX(T) ({ void* _bx__ptr = aligned_alloc(_Alignof(T), sizeof(T)); \
	(T*)_bx__ptr; })

#define BOX_EX(T) ({ void* _bx__ptr = aligned_alloc(_Alignof(T), sizeof(T)); \
	bzero(_bx__ptr, sizeof(T)); \
	(T*)_bx__ptr; })

#define BOX_WITH(T, v) ({ T* _bx__ptr = aligned_alloc(_Alignof(T), sizeof(T)); \
	*_bx__ptr = (v); \
	_bx__ptr; })

#define UNBOX_EX(_val) ({ __auto_type _bx__ptr = (_val); \
	__auto_type _bx__val = *_bx__ptr; \
	explicit_bzero((void*)_bx__ptr, sizeof(_bx__val)); \
	free((void*)_bx__ptr); \
	_bx__val; })

#define UNBOX(val) ({ __auto_type _bx__val = *val; \
	free((void*)val); \
	_bx__val; })

#endif

#endif /* _BOX_H */
