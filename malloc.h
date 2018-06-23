#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <stddef.h>
# include <unistd.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #include <stdio.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

# define TINY_SIZE			(size_t)getpagesize()
# define TINY_MAX		    16
# define SMALL_SIZE			(size_t)(33 * getpagesize())
# define SMALL_MAX			1024
# define DATA				(get_data())
# define WHAT_SIZE(type)	(type == TINY ? TINY_SIZE : SMALL_SIZE)
# define GET_DATA_ADDRESS(block) ((void *)((t_block *)block + 1))
# define BLOCK_SIZE(alloc_size) (alloc_size + sizeof(t_block))
# define ALLOC_SIZE(block_size) (block_size - sizeof(t_block))
# define NEEDED_PAGE(alloc_size) ((BLOCK_SIZE(alloc_size) / getpagesize()) + ((BLOCK_SIZE(alloc_size) % getpagesize()) ? 1 : 0))

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

void*	malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem();

t_data				*get_data();
t_block				*make_and_copy_block(void *ptr, size_t size);
t_block				*replace_block_in_list(t_block *new_block, t_block *block);
t_block				*resize_and_move_block(t_block *block, size_t size);
void				del_block_from_list(t_block *block, t_block **list);
void				ordered_add_to_list(t_block *block, t_block **list);
t_block             *new_page_alloc(t_type type);
t_block             *get_from_malloced_list(void *ptr);
size_t              merge_block(t_block **block);
void                free_block(t_block *block, t_type type);
void                reduct_block(t_block *block, t_type type, size_t size);
t_block             *find_block(size_t size, t_type type);
void                add_block_to_list(t_block *block, t_block **list);
t_block             *is_free(t_block *test, t_type type);
void                *hard_realloc(void *ptr, size_t size, size_t old_size);
int                 try_to_resize(t_block *block, t_type type, size_t size);
t_type              determine_type(size_t size);
t_block				*get_alloced_block(int nb, t_type type);
void				*ft_memcpy(void *dst, const void *src, size_t n);
void				print_address(void *ptr);
size_t				get_len_of_malloced_type(t_type type);
t_block				*get_alloced_block(int nb, t_type type);
void				putsize(size_t size);
void				print_alloced_list(t_type type, size_t len);
void d();
#endif
