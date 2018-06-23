#include "./malloc.h"

void	print_address(void *ptr)
{
	char base_digits[16] = "0123456789ABCDEF";
	int converted_number[128];
	int i;
	char result[12];
	unsigned long nbr = (unsigned long)ptr;

	i = -1;
	while (nbr != 0)
	{
		converted_number[++i] = nbr % 16;
		nbr = nbr / 16;
	}
	nbr = 0;
	while (i >= 0)
		result[nbr++] = base_digits[converted_number[i--]];
	write(1, "0x", 2);
	write(1, result + 7, 5);
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

void	putsize(size_t size)
{
	char c;

		if (size > 9)
		{
			putsize(size / 10);
			putsize(size % 10);
		}
		else
		{
			c = size + 48;
			write(1, &c, 1);
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
	print_address((void *)((b < list || type == BIG) ? b : list) - ((type == BIG) ? 0 : 1));
	write(1, "\n", 1);

	while (i <= len)
	{
		b = get_alloced_block(i++, type);
		print_address(GET_DATA_ADDRESS(b));
		write(1, " - ", 3);
		print_address(GET_DATA_ADDRESS(b) + b->size);
		write(1, " : ", 3);
		putsize(b->size);
		write(1, " octets\n", 8);
	}
}
//void d() {
//	t_block *b;
//
//	b = DATA->tiny_space;
//	printf("*****************************************\n* TINY : %p\n*****************************************\n* ", DATA->tiny_space);
//	while (b) {
//		printf("[%ld, %p] => ", b->size, b);
//		b = b->next;
//	}
//	printf("NULL\n*****************************************\n\n");
//	b = DATA->small_space;
//	printf("\n*****************************************\n* SMALL : %p\n*****************************************\n* ", DATA->small_space);
//	while (b) {
//		printf("[%ld, %p] => ", b->size, b);
//		b = b->next;
//	}
//	printf("NULL\n*****************************************\n\n");
//	b = DATA->malloced_space;
//	printf("\n*****************************************\n* ALLOCED %p\n*****************************************\n* ", DATA->malloced_space);
//	while (b) {
//		printf("[%ld, %p] => ", b->size, b);
//		b = b->next;
//	}
//	printf("NULL\n*****************************************\n");
//	printf("===================================================================================================================================\n\n\n");
//
//
//}
//
void	show_alloc_mem()
{
	size_t	len;

	if (len = get_len_of_malloced_type(TINY))
		print_alloced_list(TINY, len);
	if (len = get_len_of_malloced_type(SMALL))
		print_alloced_list(SMALL, len);
	if (len = get_len_of_malloced_type(BIG))
		print_alloced_list(BIG, len);
	write(1, "\n\n", 2);
}	
