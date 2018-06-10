#include "./malloc.h"

void	ordered_add_to_list(t_block *block, t_block **list)
{
	t_block *curs;

	if(!*list)
	{
		*list = block;
		return;
	}
	
	curs = *list;
	while (curs->next && curs < block)
		curs = curs->next;

	if (!curs->next && curs < block)
	{
		curs->next = block;
		block->prev = curs;
	}
	else
	{
		block->prev = curs->prev;
		block->next = curs;
		if (curs->prev)
			curs->prev->next = block;
		else
			*list = block;
		curs->prev = block;
	}	

}

t_block *make_and_copy_block(void *ptr, size_t size)
{
	t_block block;

	block.size = size;
	block.prev = NULL;
	block.next = NULL;

	ft_memcpy(ptr, &block, sizeof(t_block));
	return ptr;
}

void	add_block_to_list(t_block *block, t_block **list)
{
	if (*list)
	{
		block->next = *list;
		(*list)->prev = block;	
	}
	
	*list = block;	
}

void	del_block_from_list(t_block *block, t_block **list)
{
	if (*list == block)
		*list = block->next;

	if (block->next)
		block->next->prev = block->prev;
	if (block->prev)
		block->prev->next = block->next;

	block->prev = NULL;
	block->next = NULL;
}

t_block	*replace_block_in_list(t_block *new_block, t_block *block)
{
	if (DATA->tiny_space == block)
			DATA->tiny_space = new_block;
	else if (DATA->small_space == block)
			DATA->small_space = new_block;

	new_block->next = block->next;
	if (block->next)
		block->next->prev = new_block;
	new_block->prev = block->prev;
	if (block->prev)
		block->prev->next = new_block;

	block->next = NULL;
	block->prev = NULL;
	return block;
	
}

t_block *resize_and_move_block(t_block *block, size_t size)
{
	t_block *new_block;

	if (block->size <= BLOCK_SIZE(size))
		return block;

	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size, ALLOC_SIZE(block->size - size));
	
	replace_block_in_list(new_block, block);

	block->size = size;
	
	add_block_to_list(block, &(DATA->malloced_space));

	return block;
}


