#ifndef _SIMPLE_CCP_H_
#define _SIMPLE_CCP_H_
#include "Buffer.h"
#define SIMPLECCP_SENDFAILED -1
//cast char * to byte *
#define byte unsigned char
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (byte *)(ptr)

// SimpleCCP receive state machine's states
#define SIMPLECCP_REV_WAIT_START 0
#define SIMPLECCP_REV_WAIT_NEXT_AT 1
#define SIMPLECCP_REV_WAIT_END 2
#define SIMPLECCP_REV_WAIT_LF 3
#define SIMPLECCP_REV_WAIT_ESC_END 4

//escape character contains '\\','@', '\n' and '\r' are not allowed in message body.

#define __unimplemented

void SimpleCCP_init(byte *send_data,int send_capacity,byte *recv_data,int recv_capacity);
int SimpleCCP_send(byte *data,int len);
void SimpleCCP_parse(byte c);

__unimplemented void SimpleCCPRecvCallback(byte *data,int len);
__unimplemented byte SimpleCCP_putchar(byte c);

#define SIMPLECCP_MSG_HEAD "@@"
#define SIMPLECCP_MSG_TAIL "\r\n"
#endif // _SIMPLE_CCP_H_