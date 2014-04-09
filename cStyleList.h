/*
	list.h
	Date Modified: 	April 2, 2014
	Author:		Vance
	Description:	A simple list struct for C.
*/

#ifndef LIST_H
#define LIST_H

#include<stdlib.h>
#include<stdio.h>

// forward declaring to allow for the list pointer within the struct
typedef struct List List;
typedef struct ListNode ListNode;

struct ListNode
{
	ListNode* next;
	char dataItem [100];
};

struct List
{
	ListNode* head;
	ListNode* cursor;
};

//	cStyleCopy
void cStyleCopy(char original[], char copy[])
{
	int index;

	for(index = 0; original[index] != '\0' && index < 100; index++)
	{
		copy[index] = original[index];
	}

	// for the NULL char
	copy[index] = '\0';
}

//	INIT
//	Initializes the list by setting values to NULL. Only call once. 
//	If it is called after data is added to the list, parts of the list will be lost (BAD).
void init(List *list)
{
	//list = malloc(2 * sizeof(ListNode));
	list->head = malloc(sizeof(ListNode));
	list->cursor = list->head;

	list->head->dataItem[0] = '\0';
}

//	EMPTY
//	Returns 1 if the list is empty.
//	Returns 0 if the list is not empty.
int empty(List* list)
{
	if(list->head->dataItem[0] == '\0')
	{
		return 1;
	}

	return 0;
}

//	INSERT
//	Inserts the value at the current index.
//	Pushes current value to the right.
void insert(List* list, char value [100])
{
	ListNode *tempNode;

	if(empty(list))
	{
		list->head = realloc(list->head, sizeof(ListNode));	
		list->cursor = list->head;

		cStyleCopy(value, list->cursor->dataItem);
		list->head->next = 0;
	}
	else if(list->head->next == 0)
	{
		list->cursor = malloc(sizeof(ListNode));
		list->head->next = list->cursor;
		cStyleCopy(value, list->cursor->dataItem);
		list->cursor->next = 0;
	}
	else
	{
		tempNode = malloc(sizeof(ListNode));
		cStyleCopy(value, tempNode->dataItem);
		tempNode->next = list->cursor->next;
		list->cursor->next = tempNode;
		list->cursor = list->cursor->next;
	}
}

//	REMOVEATCURSOR
void removeAtCursor(List* list)
{
	ListNode *tempNode;
	if(!empty(list))
	{
		// last element in list
		if(list->head->next == 0)
		{
			free(list->head);
			init(list);
		}
		// cursor is at head but is not the last element
		else if(list->head == list->cursor)
		{
			list->head = list->head->next;
			free(list->cursor);
			list->cursor = list->head;
		}
		// does part of the same thing as prior().
		// modify at some point to use prior().
		else 
		{
			tempNode = list->cursor;
			list->cursor = list->head;
			
			while(list->cursor->next != tempNode)
			{
				list->cursor = list->cursor->next;
			}

			list->cursor->next = tempNode->next;

			free(tempNode);
		}
	}
}

//	REPLACE
void replace(List* list, char value [100])
{
	if(!empty(list))
	{
		cStyleCopy(value, list->cursor->dataItem);
	}
	else
	{
		insert(list, value);
	}
}

//	BEGIN
//	Moves cursor to head.
void begin(List* list)
{
	list->cursor = list->head;
}

//	END
//	Moves cursor to end of list.
void end(List* list)
{
	if(!empty(list))
	{
		while(list->cursor->next != 0)
		{
			list->cursor = list->cursor->next;
		}
	}
}

//	CLEAR
void clear(List* list)
{
	end(list);

	while(!empty(list))
	{
		removeAtCursor(list);
	}
}

//	NEXT
//	Moves cursor to next position in list.
void next(List* list)
{
	if(!empty(list))
	{
		if(list->cursor->next != 0)
		{
			list->cursor = list->cursor->next;
		}
	}
}

//	PRIOR
//	Moves cursor to the previous position in list
void prior(List* list)
{
	ListNode* tempNode;

	if(!empty(list))
	{
		if(list->cursor != list->head)
		{
			tempNode = list->cursor;
			list->cursor = list->head;
			
			while(list->cursor->next != tempNode)
			{
				list->cursor = list->cursor->next;
			}
		}
	}
}

//	GETCURSOR
void getCursor(List* list, char array [100])
{
	if(!empty(list))
	{
		cStyleCopy(list->cursor->dataItem, array);
	}
	else
	{
		// if empty list the NULL char is returned
		array[0] = '\0';
	}
}

//	PRINT
//	Prints each cstyle string in list.
//	Only works for cstyle strings unless it is modified.
//	For testing purposes.
void print(List* list)
{
	ListNode* tempNode;
	int index;

	if(!empty(list))
	{
		tempNode = list->head;

		for(index = 0; tempNode->dataItem[index] != '\0' && index < 100; index++)
		{
			printf("%c", tempNode->dataItem[index]);
		}
		
		printf("\n");

		while(tempNode->next != 0)
		{
			tempNode = tempNode->next;

			for(index = 0; tempNode->dataItem[index] != '\0' && index < 100; index++)
			{
				printf("%c", tempNode->dataItem[index]);
			}

			printf("\n");
		}
	}
	else
	{
		printf("Nothing to Print\n");
	}

	printf("\n");
}

#endif




