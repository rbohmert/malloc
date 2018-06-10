#include "./malloc.h"
#include <stdio.h>

t_type	determine_type(size_t size)
{
	if (size > SMALL_MAX)
		return BIG;
	else
		return size > TINY_MAX ? SMALL : TINY;
}

t_data *get_data()
{
	static t_data data = {NULL, NULL, NULL};
	return &data;
}

t_block	*new_page_alloc(t_type type) 
{
	void	*ptr_page;
	t_block	*block;

	ptr_page = mmap(NULL, WHAT_SIZE(type), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	
	block = make_and_copy_block((char*)ptr_page + 1, ALLOC_SIZE(WHAT_SIZE(type)) - 1);

	ordered_add_to_list(block, (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space));
	
	return block;
}

t_block	*find_block(size_t size, t_type type)
{
	t_block *block;
	
	block = (type == TINY) ? DATA->tiny_space : DATA->small_space;

	// 2 lignes: while (block && block->size >= size)
	while (block) {
		if (block->size >= size)
			 break;
		block = block->next;
	}

	if (block == NULL)
		block = new_page_alloc(type);
	
	return resize_and_move_block(block, size);
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
	if ((*block)->prev && (*block) ==	(void *)(*block)->prev + BLOCK_SIZE((*block)->prev->size))
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

void	free_block(t_block *block, t_type type)
{
	t_block	**list;

	list = (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space);
	ordered_add_to_list(block, list);
	
	if (merge_block(&block) == WHAT_SIZE(type) - 1)
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

void	reduct_block(t_block *block, t_type type, size_t size)
{
	t_block *new_block;

	if (block->size <= BLOCK_SIZE(size) || type == BIG)
		return;

	new_block = make_and_copy_block(GET_DATA_ADDRESS(block) + size, ALLOC_SIZE(block->size - size));

	block->size = size;

	free_block(new_block, type);
}

int		try_to_resize(t_block *block, t_type type, size_t size)
{
	t_block *nxt_blck_free;
	t_block	**list;
	
	list = (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space);
	
	if (block->size >= size)
	{
		reduct_block(block, type, size);
		return 1;
	}
	else if (type == BIG && block->size >= size)
		return 1;

	//si block suivant (en memoire) libre et assez grd pour new malloc
	if ((nxt_blck_free = is_free(GET_DATA_ADDRESS(block) + block->size, type)) &&
		nxt_blck_free->size + block->size > size && type != BIG)
	{
		del_block_from_list(nxt_blck_free, list);
		nxt_blck_free = make_and_copy_block(GET_DATA_ADDRESS(block) + size, nxt_blck_free->size + block->size - size);
		ordered_add_to_list(nxt_blck_free, list);
		block->size = size;
		return 1;
	}
	return 0;
}

void	*hard_realloc(void *ptr, size_t size, size_t old_size)
{
	void	*new_ptr;

	new_ptr = ft_malloc(size);

	ft_memcpy(new_ptr, ptr, old_size);

	free(ptr);

	return new_ptr;
}

void	*realloc(void *ptr, size_t size)
{
	t_type	src;
	t_type	dst;
	t_block *block;

	block = get_from_malloced_list(ptr);
	if (!block) {printf("noallocedptrforrealloc");return NULL;}

	dst = determine_type(size);
	src = determine_type(block->size);

	if (dst == src && try_to_resize(block, dst, size))
	{
		add_block_to_list(block, &(DATA->malloced_space));
		return ptr;
	}

	add_block_to_list(block, &(DATA->malloced_space));
	return hard_realloc(ptr, size, block->size);
}

void	free(void *ptr)
{
	t_type	type;
	t_block	*block;

	if (!ptr) {printf("nonulltofree\n");return;}

	block = get_from_malloced_list(ptr);
	
	if (!block){printf("noblocktofree\n");return;}

	type = determine_type(block->size);

	if (type == BIG)
		munmap((void *)block, BLOCK_SIZE(block->size));
	else
		free_block(block, type);
}

t_block	*direct_mmap(size_t size, t_block **malloced_space)
{
	void	*ptr_page;
	int		needed_size;
	t_block	*block;

	needed_size = (size / getpagesize() + 1) * getpagesize();

	ptr_page = mmap(NULL, needed_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	block = make_and_copy_block(ptr_page, ALLOC_SIZE(needed_size));

	add_block_to_list(block, malloced_space);

	return block;
}

void*	allocate(size_t size, t_type type) 
{
	t_block *allocated_block;

	if (type == BIG)
		allocated_block = direct_mmap(size, &(DATA->malloced_space));
	else
		allocated_block = find_block(size, type);
	
	return (GET_DATA_ADDRESS(allocated_block));
}


void*	ft_malloc(size_t size)
{
	t_type	type;
	void*	ptr;

	if (size <= 0)
		return NULL;

	type = determine_type(size);

	ptr = allocate(size, type);
	return ptr;
}

void	d()
{
	t_block *b;

	printf("===================================================================================================================================\n");
	b = DATA->tiny_space;
	printf("*****************************************\n* TINY : %p\n*****************************************\n* ", DATA->tiny_space);
	while (b) {
		printf("[%ld, %p] => ", b->size, b);
		b = b->next;
	}
	printf("NULL\n*****************************************\n\n");
	b = DATA->small_space;
	printf("\n*****************************************\n* SMALL : %p\n*****************************************\n* ", DATA->small_space);
	while (b) {
		printf("[%ld, %p] => ", b->size, b);
		b = b->next;
	}
	printf("NULL\n*****************************************\n\n");
	b = DATA->malloced_space;
	printf("\n*****************************************\n* ALLOCED %p\n*****************************************\n* ", DATA->malloced_space);
	while (b) {
		printf("[%ld, %p] => ", b->size, b);
		b = b->next;
	}
	printf("NULL\n*****************************************\n");
	printf("===================================================================================================================================\n\n\n");
}

int main() {
	void* ptr = ft_malloc(10);
	
	void *p;
	void *p2;
	void *p3;
	void *p4;
	void *p5;
	void *p6;
	void *p7;
	void *p8;
	p = ft_malloc(2000);
	p2 = ft_malloc(10);
	p3 = ft_malloc(10);
	p4 = ft_malloc(10);
	p5 = ft_malloc(10);
	p6 = ft_malloc(100);
	d();
	getchar();
	free(NULL);
	free((void *)89678);
	free(ptr);
	printf("free 1e petit\n");
	d();
	getchar();
	free(p3);
	printf("free 3e petit\n");
	d();
	getchar();
	free(p5);
	printf("free 5e petit\n");
	d();
	getchar();
	free(p2);
	printf("free 2e petit\n");
	d();
	getchar();
	free(p4);
	printf("free 4e petit\n");
	d();
	getchar();
	free(p);
	free(p6);
	d();
	getchar();
	ptr = ft_malloc(100);

	d();
	ptr = realloc(ptr, 101);
	d();
//	char* ptr = (char *)mmap(NULL, TINY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//
	t_block bp;
//
	bp.size = 5;
	bp.prev = NULL;
	bp.next = NULL;
//
	ft_memcpy(ptr, &bp, sizeof(t_block));
//	bp.type = 5;
//	ft_memcpy(ptr + 50, &bp, sizeof(t_block_page));
//	bp.type = BIG;
//	ft_memcpy(((void *)((char *)ptr + 800)), &bp, sizeof(t_block_page));
//	
	t_block bp2;
//	t_block_page bp50;
//	t_block_page bp100;
//
//
	ft_memcpy(&bp2, ptr, sizeof(t_block));
//	ft_memcpy(&bp50, ((void *)((char *)ptr + 50)), sizeof(t_block_page));
//	ft_memcpy(&bp100, ptr + 800, sizeof(t_block_page));
	printf("%ld, %p, %p", bp2.size, bp2.prev, bp2.next);
//	printf("%d, %p, %p", bp50.type, bp50.prev, bp50.next);
//	printf("%d, %p, %p", bp100.type, bp100.prev, bp100.next);
	printf("\n%ld", sizeof(t_block));
//	printf("\n%p, %p, %p", (((void *)ptr)+ 1), ((void *)((char *)ptr + 1)),((void *)((char *)ptr + 2)));
	return 0;
}
