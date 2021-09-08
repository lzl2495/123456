#include "sqlite_link_list.h"

slinklist slinkHead;

slinklist sqlite_linkCreate(void)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	slinklist head = (slinklist)malloc(sizeof(Slinklist));
	if(head == NULL)
		return NULL;
	head->next = NULL;

	return head;
}

void sqlite_InsertLinknode(unsigned char type,struct storage_env_info storage_RT,int sto_no, int num)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	printf("%s:  %s: __%d__ shead :%p\n",__FILE__,__func__,__LINE__,slinkHead);
	slinklist node = NULL,shead = slinkHead;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	node = sqlite_linkCreate();
	if(node == NULL)
		return ;

	node->type = type;
	node->env_buf = storage_RT;
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	while(shead->next != NULL)
		shead = shead->next;

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	node->next = shead->next;
	shead->next = node;
}

slinklist sqlite_GetLinknode(slinklist slinkHead)
{
	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	printf("%s:  %s: __%d__ shead :%p\n",__FILE__,__func__,__LINE__,slinkHead);
	slinklist node = NULL;
	if(slinkHead->next ==NULL)
		return NULL;
	node = slinkHead->next;
	slinkHead->next = node->next;
	printf("%s:  %s: __%d__ node :%p\n",__FILE__,__func__,__LINE__,node);

	printf("%s:  %s: _____%d__\n",__FILE__,__func__,__LINE__);
	return node;
}


