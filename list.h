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
	char dataItem;
	ListNode* next;
};

struct List
{
	ListNode* head;
	ListNode* cursor;
};

/*
	INIT
	Initializes the list by setting values to NULL. Only call once. 
	If it is called after data is added to the list, parts of the list will be lost (BAD).
*/
void init(List *list)
{
	//list = malloc(2 * sizeof(ListNode));
	list->head = malloc(sizeof(ListNode));
	list->cursor = list->head;

	list->head->dataItem = 0;
}

/*
	EMPTY
	Returns 1 if the list is empty.
	Returns 0 if the list is not empty.
*/
int empty(List* list)
{
	if(list->head->dataItem == 0)
	{
		return 1;
	}

	return 0;
}

/*
	INSERT
	Inserts the value at the current index.
	Pushes current value to the right.
*/
void insert(List* list, char value)
{
	ListNode *tempNode;

	if(empty(list))
	{
		list->head = realloc(list->head, sizeof(ListNode));	
		list->cursor = list->head;

		list->head->dataItem = value;
		list->head->next = 0;
	}
	else if(list->head->next == 0)
	{
		list->cursor = malloc(sizeof(ListNode));
		list->head->next = list->cursor;
		list->cursor->dataItem = value;
		list->cursor->next = 0;
	}
	else
	{
		tempNode = malloc(sizeof(ListNode));
		tempNode->dataItem = value;
		tempNode->next = list->cursor->next;
		list->cursor->next = tempNode;
		list->cursor = list->cursor->next;
	}
}

/*
	REMOVEATCURSOR
*/
void removeAtCursor(List* list)
{
	ListNode *tempNode;
	if(!empty(list))
	{
		// last element in list
		if(list->head = list->cursor)
		{
			free(list->head);
			init(list);
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
	else
	{
printf("\nAlready Empty\n");
	}
}

/*
	REPLACE
*/
void replace(List* list, char value)
{
	if(!empty(list))
	{
		list->cursor->dataItem = value;
	}
	else
	{
		insert(list, value);
	}
}

/*
	CLEAR
*/
void clear(List* list)
{
	while(!empty(list))
	{
		removeAtCursor(list);
	}
}

/*
	BEGIN
	Moves cursor to head.
*/
void begin(List* list)
{
	list->cursor = list->head;
}

/*
	END
	Moves cursor to end of list.
*/
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

/*
	NEXT
	Moves cursor to next position in list.
*/
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

/*
	PRIOR
	Moves cursor to the previous position in list
*/
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

/*
	GETCURSOR
*/
char getCursor(List* list)
{
	if(!empty(list))
	{
		return list->cursor->dataItem;
	}
	
	return 0;
}

/*
	PRINT
	Prints each character in the list.
	Only works for characters unless it is modified.
*/
void print(List* list)
{
	ListNode* tempNode;

	if(!empty(list))
	{
		tempNode = list->head;
		printf("%c", tempNode->dataItem);
		
		while(tempNode->next != 0)
		{
			tempNode = tempNode->next;
			printf("\n%c", tempNode->dataItem);
		}
	}
}
#endif




