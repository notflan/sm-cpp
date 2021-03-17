#ifndef _TODO_H
#define _TODO_H


#ifdef __cplusplus
#include <cstdio>
#define restrict __restrict__

[[noreturn]]
#else
#include <stdio.h>

__attribute__((noreturn))
#endif
inline static void _dev__unimplemented(const char* msg)
{
		fputs("unimplemented", stderr);
	if(msg) fputs(msg, stderr);
	fputc('\n', stderr);

	abort();
}

#define TODO(msg) _dev__unimplemented(": TODO: " msg)

#endif /* _TODO_H */
