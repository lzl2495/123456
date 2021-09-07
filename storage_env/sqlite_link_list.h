#ifndef __SQLITE_LINK_LIST__
#define __SQLITE_LINK_LIST__

#include "global.h"

#define COLLECT_INSERTER 1
#define STORAGE_UPDATE 0
#define STORAGE_GET 4
#define COLLECT_GET 5



typedef struct slinklist_st
{
	unsigned char type;
	struct storage_env_info env_buf;
	struct slinklist_st *next;
}Slinklist, *slinklist;


slinklist sqlite_linkCreate(void);
void sqlite_InsertLinknode(unsigned char type, struct storage_env_info storage_RT,int sto_no,int );
slinklist sqlite_GetLinknode(slinklist slinkHead);






#endif  /*__SQLITE_LINK_LIST__*/
