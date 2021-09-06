#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#include <stdlib.h>
#define TEXT_SIZE 128

typedef struct 
{
	char msg_type;
	char text[TEXT_SIZE];
}link_datatype;

typedef struct linklist_st
{
	link_datatype buf;
	struct linklist_st *next;
	
}Linklist, *linklist;

linklist CreateEmptyLinklist(void);
int InsertLinknode(link_datatype buf);


#endif  /*__LINK_LIST_H__*/
