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

	printf("mmap => %p\n", ptr_page);

	block = make_and_copy_block((char*)ptr_page + 1, ALLOC_SIZE(WHAT_SIZE(type)) - 1);

	ordered_add_to_list(block, (type == TINY) ? &(DATA->tiny_space) : &(DATA->small_space));

	return block;
}

t_block	*direct_mmap(size_t size, t_block **malloced_space)
{
	void	*ptr_page;
	int		needed_size;
	t_block	*block;

	needed_size = (size / getpagesize() + 1) * getpagesize();

	ptr_page = mmap(NULL, needed_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	block = make_and_copy_block(ptr_page, ALLOC_SIZE(needed_size));

	ordered_add_to_list(block, malloced_space);

	return block;
}

void*	ft_malloc(size_t size)
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

void	print_address(void *ptr)
{
	char base_digits[16] = "0123456789ABCDEF";
	int converted_number[128];
	int i;
	char result[14];
	unsigned long nbr = (unsigned long)ptr;

	i = -1;
	result[0] = '0';
	result[1] = 'x';
	while (nbr != 0)
	{
		converted_number[++i] = nbr % 16;
		nbr = nbr / 16;
	}
	nbr = 2;
	while (i >= 0)
		result[nbr++] = base_digits[converted_number[i--]];
	write(1, result, 14);
}

size_t		get_len_of_malloced_type(t_type type)
{
	int		count;
	t_block *b;

	b = DATA->malloced_space;
	count = 0;
	while (b)
	{
		if (type == determine_type(b->size))
			count++;
		b = b->next;
	}
	return count;
}

t_block		*get_alloced_block(int nb, t_type type)
{
	int		count;
	t_block *b;

	b = DATA->malloced_space;
	count = 0;
	while (b)
	{
		if (type == determine_type(b->size))
			count++;
		if (count == nb)
			return b;
		b = b->next;
	}
}

void		print_alloced_list(t_type type, size_t len)
{
	int 	i;
	t_block *b;
	t_block *list;

	i = 1;
	b = get_alloced_block(1, type);
	list = (type == TINY) ? DATA->tiny_space : DATA->small_space;

	if (type == TINY)
		write(1, "TINY : ", 7);
	else
		write(1, (type == SMALL) ? "SMALL : " : "LARGE : ", 8);
	print_address((void *)((b < list || type == BIG) ? b : list) - 1);
	write(1, "\n", 1);

	while (i <= len)
	{
		b = get_alloced_block(i++, type);
		print_address(GET_DATA_ADDRESS(b));
		write(1, " - ", 3);
		print_address(GET_DATA_ADDRESS(b) + b->size - 1);
		write(1, " : ", 3);
		printf("%d octets\n", b->size);
	}
}

void	show_alloc_mem()
{
	size_t	len;
	t_block *b;

	if (len = get_len_of_malloced_type(TINY))
		print_alloced_list(TINY, len);
	if (len = get_len_of_malloced_type(SMALL))
		print_alloced_list(SMALL, len);
	if (len = get_len_of_malloced_type(BIG))
		print_alloced_list(BIG, len);
	
	
				printf("\n\n\n\n\n\n");
			
				b = DATA->tiny_space;
				printf("\n*****************************************\n* tiny : %p\n*****************************************\n* ", DATA->small_space);
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
	show_alloc_mem();
	getchar();
	free(NULL);
	free((void *)89678);
	free(ptr);
	printf("free 1e petit\n");
	show_alloc_mem();
	getchar();
	free(p3);
	printf("free 3e petit\n");
	show_alloc_mem();
	getchar();
	free(p5);
	printf("free 5e petit\n");
	show_alloc_mem();
	getchar();
	free(p2);
	printf("free 2e petit\n");
	show_alloc_mem();
	getchar();
	free(p4);
	printf("free 4e petit\n");
	show_alloc_mem();
	getchar();
	free(p);
	free(p6);
	show_alloc_mem();
	getchar();
	ptr = ft_malloc(100);

	show_alloc_mem();
	ptr = realloc(ptr, 101);
	show_alloc_mem();
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
