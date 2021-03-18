#include <state.h>
#include "dev.h"

/// A generator that just returns `nullptr` (end). Used for yielding without calling in to another generator. 
///
/// NOTE: To avoid uselessly creating a stack frame for calls to this generator, its address is (TODO) directly compared to when switching into a deeper generator, and then skipped entirely.
sm_yield _sm_noop(sm_state*) 
{
	return (sm_yield)nullptr;
}

sm_state* sm_new_state()
{
	auto state = box<sm_state>();
	state->current = box<_sm_frame, true>();
	return state;
}

inline static void sm_free_user(_sm_user* data)
{
	if (data->free) {
		free(data->_ptr);
	}
}

inline static void _sm_free_page(_sm_user_page* page)
{
	for (int i=0;i<_SM_STACK_SIZE;i++) sm_free_user(&page->data[i]);
}

inline static void _sm_free_all_pages(_sm_user_page* page)
{
	while(page)
	{
		_sm_free_page(page);
		auto old = page;
		page = page->next;
		free(old);
	}
}

void sm_free_state(sm_state* state)
{
	_sm_frame* frame = unbox(state).current;

	while(frame)
	{
		_sm_free_all_pages(frame->user.next);
		_sm_free_page(&frame->user);

		frame = unbox(frame).prev;
	}
}

void _sm_pop_stack(sm_state* state)
{
	if(!state->current) return;

	auto last = unbox(state->current);
	state->current = last.prev;

	_sm_free_all_pages(last.user.next);
	_sm_free_page(&last.user);
}

void _sm_push_stack(sm_state* state)
{
	auto next = box<_sm_frame, true>();
	next->prev = state->current;
	state->current = next;
}
