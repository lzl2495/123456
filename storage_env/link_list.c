#include "link_list.h"


linklist linkHead; //头结点

linklist CreateEmptyLinklist(void)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	linklist head =(linklist)malloc(sizeof(Linklist));
	if(head == NULL)
		return NULL;
	head->next = NULL;
	 
	return head;
}

static linklist Create_DataNode(void)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	linklist node = (linklist)malloc(sizeof(Linklist));
	if(node == NULL)
		return NULL;
	node->next = NULL;
	
	return node;
	
}
int InsertLinknode(link_datatype buf)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	linklist head = linkHead;
	linklist node = Create_DataNode();
	if(node == NULL)
		return -1;
	node->buf = buf;

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	while(head->next != NULL)
		head = head->next;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);

	node->next = head->next;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	head->next = node;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);

	return 0;
}


linklist GetLinknode(linklist linkHead)
{
	printf("linkHead = %p __%d__\n ",linkHead,__LINE__);
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	linklist node = NULL;
	if(linkHead->next == NULL)
		return NULL;
	node = linkHead->next;
	linkHead->next = node->next;

	return node;
}

