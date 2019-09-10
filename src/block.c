/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbohmert <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/10 17:59:17 by rbohmert          #+#    #+#             */
/*   Updated: 2019/09/10 17:59:20 by rbohmert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./malloc.h"

t_block	*make_and_copy_block(void *ptr, size_t size)
{
	t_block block;

	block.size = size;
	block.prev = NULL;
	block.next = NULL;
	ft_memcpy(ptr, &block, sizeof(t_block));
	return (ptr);
}

t_block	*resize_and_move_block(t_block *block, size_t size)
{
	t_block *new_block;

	if (block->size <= BLOCK_SIZE(size))
		return (block);
	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size,\
	ALLOC_SIZE(block->size - size));
	replace_block_in_list(new_block, block);
	block->size = size;
	ordered_add_to_list(block, &(DATA->malloced_space));
	return (block);
}

t_block	*get_from_malloced_list(void *ptr)
{
	t_block *block;

	block = DATA->malloced_space;
	while (block)
	{
		if (GET_DATA_ADDRESS(block) == ptr)
			break ;
		block = block->next;
	}
	if (!block)
		return (NULL);
	del_block_from_list(block, &(DATA->malloced_space));
	return (block);
}

size_t	merge_block(t_block **block)
{
	t_block *bl;

	bl = *block;
	if (bl->prev &&\
	bl == (void *)bl->prev + BLOCK_SIZE(bl->prev->size))
	{
		if (bl->next)
			bl->next->prev = bl->prev;
		bl->prev->next = bl->next;
		bl->prev->size = bl->prev->size + BLOCK_SIZE(bl->size);
		bl = bl->prev;
	}
	if (bl->next &&\
	bl->next == (void *)bl + BLOCK_SIZE(bl->size))
	{
		if (bl->next->next)
			bl->next->next->prev = bl;
		bl->size = bl->size + BLOCK_SIZE(bl->next->size);
		bl->next = bl->next->next;
	}
	return (BLOCK_SIZE(bl->size));
}

void	reduct_block(t_block *block, t_type type, size_t size)
{
	t_block *new_block;

	if (block->size <= BLOCK_SIZE(size) || type == BIG)
	{
		if (type == BIG)
			block->size = size;
		return ;
	}
	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size,\
	ALLOC_SIZE(block->size - size));
	block->size = size;
	free_block(new_block, type);
}
