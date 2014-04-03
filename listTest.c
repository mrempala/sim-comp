/*
	listTest.c
	Data Modified: 	April 2, 2014
	Author:		Vance
	Description:	Used to test the list struct from list.h
*/

#include <stdio.h>
#include "list.h"

int main()
{
	printf("\nList Test Program\n");

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

	insert(&list, 'h');
	printf("\n%c", list.cursor->dataItem);

	insert(&list, 'a');
	printf("\n%c", list.cursor->dataItem);

	insert(&list, 'p');
	printf("\n%c", list.cursor->dataItem);

	insert(&list, 'p');
	printf("\n%c", list.cursor->dataItem);

	insert(&list, 'y');
	printf("\n%c", list.cursor->dataItem);

	printf("\n");
	printf("\nTesting Replace\n");
	printf("\n");

	begin(&list);
	
	next(&list);

	replace(&list, 'o');

	print(&list);

	printf("\n");
	printf("\nTesting getCursor\n");
	printf("\n");

	prior(&list);

	printf("%c", getCursor(&list));

	printf("\n");
	printf("\nTesting End\n");
	printf("\n");

	end(&list);

	printf("%c", getCursor(&list));

	printf("\n");
	printf("\nRemoving everything\n");

	clear(&list);

	if(empty(&list))
	{
		printf("\nEmpty List\n");
	}
	else
	{
		printf("\nNot Empty List\n");
	}


	printf("\nEnd Of Test Program\n");

	return 0;
}
