#include <stdio.h>
#include "cStyleList.h"

int main()
{
	printf("\nC-Style List Test Program\n");

	// creating the list
	List list;

	printf("\nList Created\n");

	init(&list);

	printf("\nList Initialized\n");

	if(empty(&list))
	{
		printf("\nEmpty List\n");
	}
	else
	{
		printf("\nNot Empty List\n");
	}

	printf("\nInserting values\n");

	insert(&list, "The");

	print(&list);

	insert(&list, "Cat");

	insert(&list, "Meowed");

	print(&list);

	begin(&list);

	next(&list);

	replace(&list, "Dog");

	print(&list);

	begin(&list);

	removeAtCursor(&list);

	print(&list);

	clear(&list);

	if(empty(&list))
	{
		printf("Empty List\n");
	}
	else
	{
		printf("Not Empty List\n");
	}

	printf("\nEnd Of Test Program\n");

	return 0;
}
