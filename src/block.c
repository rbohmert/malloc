#include "./malloc.h"

t_block *make_and_copy_block(void *ptr, size_t size)
{
	t_block block;

	block.size = size;
	block.prev = NULL;
	block.next = NULL;

	ft_memcpy(ptr, &block, sizeof(t_block));
	return ptr;
}

t_block *resize_and_move_block(t_block *block, size_t size)
{
	t_block *new_block;

	if (block->size <= BLOCK_SIZE(size))
		return block;

	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size, ALLOC_SIZE(block->size - size));

	replace_block_in_list(new_block, block);

	block->size = size;

	ordered_add_to_list(block, &(DATA->malloced_space));

	return block;
}

t_block	*get_from_malloced_list(void *ptr)
{
	t_block *block;

	block = DATA->malloced_space;

	while(block)
	{
		if (GET_DATA_ADDRESS(block) == ptr)
			break;
		block = block->next;
	}

	if (!block)
		return NULL;

	del_block_from_list(block, &(DATA->malloced_space));

	return block;
}

size_t	merge_block(t_block **block)
{
	if ((*block)->prev && (*block) == (void *)(*block)->prev + BLOCK_SIZE((*block)->prev->size))
	{
		if ((*block)->next)
			(*block)->next->prev = (*block)->prev;
		(*block)->prev->next = (*block)->next;
		(*block)->prev->size = (*block)->prev->size + BLOCK_SIZE((*block)->size);
		(*block) = (*block)->prev;
	}

	if ((*block)->next && (*block)->next == (void *)(*block) + BLOCK_SIZE((*block)->size))
	{
		if ((*block)->next->next)
			(*block)->next->next->prev = (*block);
		(*block)->size = (*block)->size + BLOCK_SIZE((*block)->next->size);
		(*block)->next = (*block)->next->next;
	}

	return BLOCK_SIZE((*block)->size);
}

void	reduct_block(t_block *block, t_type type, size_t size)
{
	t_block *new_block;

	// BIG new<old mais necessite autant de page
	if (block->size <= BLOCK_SIZE(size) || type == BIG)
	{
		if (type == BIG)
			block->size = size;
		return;
	}

	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size, ALLOC_SIZE(block->size - size));

	block->size = size;

	free_block(new_block, type);
}
