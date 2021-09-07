#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#include <stdio.h>
#include <stdlib.h>

#define TEXT_SIZE 128
#define MSG_LEDT 1
#define MSG_BEEP 2
#define MSG_FAN 4
#define MSG_LEDH 6
#define MSG_LEDI 7

#define MSG_M0_LEDT_ON 1
#define MSG_M0_LEDI_ON 1
#define MSG_M0_LEDH_ON 1
#define MSG_M0_LEDH_OFF 0
#define MSG_M0_LEDT_OFF 0
#define MSG_M0_LEDI_OFF 0
#define MSG_M0_BEEP_OFF 0
#define MSG_M0_BEEP_ON 1
#define MSG_M0_FAN_ON1 1
#define MSG_M0_FAN_ON2 2
#define MSG_M0_FAN_ON3 3
#define MSG_M0_FAN_OFF 0


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
linklist GetLinknode(linklist linkHead);
int sendMsgQueue(int witch,int do_what);


#endif  /*__LINK_LIST_H__*/
