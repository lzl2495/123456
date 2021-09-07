#include "sqlite_link_list.h"

slinklist slinkHead;

slinklist sqlite_linkCreate(void)
{
	slinkHead = (slinklist)malloc(sizeof(slinklist));
	if(slinkHead == NULL)
		return NULL;
	slinkHead->next = NULL;

	return slinkHead;
}

void sqlite_InsertLinknode(unsigned char type,struct storage_env_info storage_RT,int sto_no, int num)
{
	slinklist node = NULL,shead = slinkHead;
	node->type = type;
	node->env_buf = storage_RT;

	while(shead->next != NULL)
		shead = shead->next;

	node->next = shead->next;
	shead->next = node;
}

slinklist sqlite_GetLinknode(slinklist slinkHead)
{
	slinklist node = NULL;
	if(slinkHead->next ==NULL)
		return NULL;
	node = slinkHead->next;
	slinkHead->next = node->next;

	return node;
}


