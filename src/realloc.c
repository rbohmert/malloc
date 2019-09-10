/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbohmert <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/10 18:06:39 by rbohmert          #+#    #+#             */
/*   Updated: 2019/09/10 18:21:54 by rbohmert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./malloc.h"

/*
** si new<old et necessite moins de page => hard realloc
** si new et old necessite le meme nombre de page et BIG => chqnge block->size
** si !BIG BIZAR, A RECHECK
*/

int		try_to_resize(t_block *block, t_type type, size_t size)
{
	t_block *nxt_blck_free;
	t_block	**list;

	list = (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space);
	if (block->size >= size)
	{
		if (type == BIG && NEEDED_PAGE(block->size) > NEEDED_PAGE(size))
			return (0);
		reduct_block(block, type, size);
		return (1);
	}
	else if (type == BIG && (NEEDED_PAGE(block->size) == NEEDED_PAGE(size)))
	{
		block->size = size;
		return (1);
	}
	if ((nxt_blck_free = is_free(GET_DATA_ADDRESS(block) + block->size, type))
		&& nxt_blck_free->size + block->size > size && type != BIG)
	{
		del_block_from_list(nxt_blck_free, list);
		nxt_blck_free = make_and_copy_block(GET_DATA_ADDRESS(block) + size,\
		nxt_blck_free->size + block->size - size);
		ordered_add_to_list(nxt_blck_free, list);
		block->size = size;
		return (1);
	}
	return (0);
}

void	*hard_realloc(void *ptr, size_t size, size_t old_size)
{
	void *new_ptr;

	new_ptr = malloc(size);
	ft_memcpy(new_ptr, ptr, size > old_size ? old_size : size);
	free(ptr);
	return (new_ptr);
}

void	*realloc(void *ptr, size_t size)
{
	t_type	src;
	t_type	dst;
	t_block *block;

	block = get_from_malloced_list(ptr);
	if (!block)
		return (NULL);
	dst = determine_type(size);
	src = determine_type(block->size);
	if (dst == src && try_to_resize(block, dst, size))
	{
		ordered_add_to_list(block, &(DATA->malloced_space));
		return (ptr);
	}
	ordered_add_to_list(block, &(DATA->malloced_space));
	return (hard_realloc(ptr, size, block->size));
}
