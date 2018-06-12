#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <stddef.h>
# include <unistd.h>

# define TINY_SIZE			(size_t)getpagesize()
# define TINY_MAX			232
# define SMALL_SIZE			(size_t)(16 * getpagesize())
# define SMALL_MAX			4072
# define DATA				(get_data())
# define WHAT_SIZE(type)	(type == TINY ? TINY_SIZE : SMALL_SIZE)
# define GET_DATA_ADDRESS(block) ((void *)((t_block *)block + 1))
# define BLOCK_SIZE(alloc_size) (alloc_size + sizeof(t_block))
# define ALLOC_SIZE(block_size) (block_size - sizeof(t_block))

typedef	enum		e_type
{
	TINY,
	SMALL,
	BIG,
}					t_type;

typedef struct		s_block
{
	size_t			size;
	struct s_block	*prev;
	struct s_block	*next;
}			t_block;

typedef struct		s_data
{
	t_block			*tiny_space;
	t_block			*small_space;
	t_block			*malloced_space;
}					t_data;

void*	ft_malloc(size_t size);
void	free(void *ptr);

t_data				*get_data();
t_block				*make_and_copy_block(void *ptr, size_t size);
void				add_block_to_list(t_block *block, t_block **list);
t_block				*replace_block_in_list(t_block *new_block, t_block *block);
t_block				*resize_and_move_block(t_block *block, size_t size);
void				del_block_from_list(t_block *block, t_block **list);
void				ordered_add_to_list(t_block *block, t_block **list);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void d();
#endif
