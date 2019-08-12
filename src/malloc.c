#include "./malloc.h"

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

t_block	*direct_mmap(size_t size, t_block **malloced_space)
{
	void	*ptr_page;
	size_t	needed_size;
	t_block	*block;

	needed_size = NEEDED_PAGE(size) * getpagesize();

	ptr_page = mmap(NULL, needed_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	block = make_and_copy_block(ptr_page, size);

	ordered_add_to_list(block, malloced_space);

	return block;
}

void*	malloc(size_t size)
{
	t_type	type;
	void*	ptr;

	if (size <= 0)
		return NULL;

	type = determine_type(size);

	if (type == BIG)
		ptr = GET_DATA_ADDRESS(direct_mmap(size, &(DATA->malloced_space)));
	else
		ptr = GET_DATA_ADDRESS(find_block(size, type));

	return ptr;
}

//int main() {
//	show_alloc_mem();
////how_alloc_mem();
//	//getchar();
//	show_alloc_mem();
//	void* ptr = malloc(10);
//	show_alloc_mem();
//	//getchar();
//
//	void *p;
//	void *p2;
//	void *p3;
//	void *p4;
//	void *p5;
//	void *p6;
//
//	p = malloc(2000);
//	p2 = malloc(10);
//	p3 = malloc(10);
//	p4 = malloc(10);
//	p5 = malloc(10);
//	p6 = malloc(100);
//	//show_alloc_mem();
//	//getchar();
//	free(NULL);
//	free((void *)89678);
//	//free(ptr);
//	//show_alloc_mem();
//	//getchar();
//	free(p3);
//	//show_alloc_mem();
//	free(p2);
//	show_alloc_mem();
//	//getchar();
//	free(p4);
//	show_alloc_mem();
//	//getchar();
//	free(p6);
//	show_alloc_mem();
//	//getchar();
//	ptr = malloc(100);
//
//	show_alloc_mem();
//	p = realloc(p, 4072);
//	show_alloc_mem();
//	p = realloc(p, 2500);
//	show_alloc_mem();
//	p = realloc(p, 67674);
//	show_alloc_mem();
//	p = realloc(p, 6767);
//
//	p5 = malloc(7);
//	free(p5);
//	show_alloc_mem();
//	//	char* ptr = (char *)mmap(NULL, TINY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//	//
//	t_block bp;
//	//
//	bp.size = 5;
//	bp.prev = NULL;
//	bp.next = NULL;
//	//
//	ft_memcpy(ptr, &bp, sizeof(t_block));
//	//	bp.type = 5;
//	//	ft_memcpy(ptr + 50, &bp, sizeof(t_block_page));
//	//	bp.type = BIG;
//	//	ft_memcpy(((void *)((char *)ptr + 800)), &bp, sizeof(t_block_page));
//	//
//	t_block bp2;
//	//	t_block_page bp50;
//	//	t_block_page bp100;
//	//
//	//
//	ft_memcpy(&bp2, ptr, sizeof(t_block));
//	//	ft_memcpy(&bp50, ((void *)((char *)ptr + 50)), sizeof(t_block_page));
//	//	ft_memcpy(&bp100, ptr + 800, sizeof(t_block_page));
//	//printf("%ld, %p, %p", bp2.size, bp2.prev, bp2.next);
//	//	printf("%d, %p, %p", bp50.type, bp50.prev, bp50.next);
//	//	printf("%d, %p, %p", bp100.type, bp100.prev, bp100.next);
//	//printf("\n%ld", sizeof(t_block));
//	//	printf("\n%p, %p, %p", (((void *)ptr)+ 1), ((void *)((char *)ptr + 1)),((void *)((char *)ptr + 2)));
//	return 0;}
