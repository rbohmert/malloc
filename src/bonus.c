/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbohmert <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/10 17:26:50 by rbohmert          #+#    #+#             */
/*   Updated: 2019/09/10 17:28:49 by rbohmert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./malloc.h"

void	*calloc(size_t nmemb, size_t size)
{
	void	*ptr;
	size_t	total_size;

	total_size = nmemb * size;
	if ((ptr = malloc(total_size)))
		return (NULL);
	ft_bzero(ptr, total_size);
	return (ptr);
}
