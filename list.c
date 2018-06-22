#include "./malloc.h"

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

t_block	*find_block(size_t size, t_type type)
{
	t_block *block;

	block = (type == TINY) ? DATA->tiny_space : DATA->small_space;

	while (block && block->size < size) 
		block = block->next;
	
	if (block == NULL)
		block = new_page_alloc(type);

	return resize_and_move_block(block, size);
}


