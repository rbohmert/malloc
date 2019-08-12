#include "./malloc.h"

void	free_block(t_block *block, t_type type)
{
	t_block	**list;

	list = (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space);
	ordered_add_to_list(block, list);

	if (merge_block(&block) == WHAT_SIZE(type) - 1 && !(*list == block && !block->next))
	{
		if (block->prev)
			block->prev->next = block->next;
		if (block->next)
			block->next->prev = block->prev;
		if (*list == block)
			*list = block->next ? block->next : NULL;

		munmap((char *)block - 1, WHAT_SIZE(type));
	}
}

t_block		*is_free(t_block *test, t_type type)
{
	t_block	*block;

	block = (type == TINY) ? DATA->tiny_space : DATA->small_space;

	while (block) {
		if (block == test)
			return test;
		block = block->next;
	}
	return NULL;
}

void	free(void *ptr)
{
	t_type	type;
	t_block	*block;

	if (!ptr)
		return;

	block = get_from_malloced_list(ptr);

	if (!block)
		return;

	type = determine_type(block->size);

	if (type == BIG)
		munmap((void *)block, BLOCK_SIZE(block->size));
	else
		free_block(block, type);
}
