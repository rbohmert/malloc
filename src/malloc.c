/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbohmert <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/10 17:37:02 by rbohmert          #+#    #+#             */
/*   Updated: 2019/09/10 17:49:29 by rbohmert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./malloc.h"

t_type	determine_type(size_t size)
{
	if (size > SMALL_MAX)
		return (BIG);
	else
		return (size > TINY_MAX ? SMALL : TINY);
}

t_data	*get_data(void)
{
	static t_data data = {NULL, NULL, NULL};

	return (&data);
}

t_block	*new_page_alloc(t_type type)
{
	void	*ptr_page;
	t_block	*block;

	ptr_page = mmap(NULL, WHAT_SIZE(type),\
	PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	block = \
	make_and_copy_block((char*)ptr_page + 1, ALLOC_SIZE(WHAT_SIZE(type)) - 1);
	ordered_add_to_list(block, (type == TINY) ?\
	&(DATA->tiny_space) : &(DATA->small_space));
	return (block);
}

t_block	*direct_mmap(size_t size, t_block **malloced_space)
{
	void	*ptr_page;
	size_t	needed_size;
	t_block	*block;

	needed_size = NEEDED_PAGE(size) * getpagesize();
	ptr_page = mmap(NULL, needed_size, \
	PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	block = make_and_copy_block(ptr_page, size);
	ordered_add_to_list(block, malloced_space);
	return (block);
}

void	*malloc(size_t size)
{
	t_type	type;
	void	*ptr;

	if (size <= 0)
		return (NULL);
	type = determine_type(size);
	if (type == BIG)
		ptr = GET_DATA_ADDRESS(direct_mmap(size, &(DATA->malloced_space)));
	else
		ptr = GET_DATA_ADDRESS(find_block(size, type));
	return (ptr);
}
