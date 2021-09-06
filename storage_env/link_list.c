#include "link_list.h"


extern linklist linkHead; //头结点

linklist CreateEmptyLinklist(void)
{
	linklist head = (void *)malloc(sizeof(Linklist));
	if(head == NULL)
		return NULL;
	head->next = NULL;
	
	return head;
}

static linklist Create_DataNode(void)
{
	linklist node = (void *)malloc(sizeof(Linklist));
	if(node == NULL)
		return NULL;
	node->next = NULL;
	
	return node;
	
}
int InsertLinknode(link_datatype buf)
{
	linklist head = linkHead;
	linklist node = Create_DataNode();
	if(node == NULL)
		return -1;
	node->buf = buf;

	while(head->next != NULL)
		head = head->next;

	head->next = node;
	node->next = head->next;

	return 0;
}
