#include "SimpleCCP.h"

static bool SimpleCCP_escape_char_contain(byte c);
static void SimpleCCP_send_buffer();
static void SimpleCCPRecvInnerCallback(byte *data,int len);

static Buffer send_buffer;
static Buffer recv_buffer;
static int SimpleCCPErrorCnt;
static int SimpleCCPRevState;
static char SimpleCCP_escape_char[] = {'\\','@'};

void SimpleCCP_init(byte *send_data,int send_capacity,byte *recv_data,int recv_capacity){
    buffser_setmemory(&send_buffer,send_data,send_capacity);
    buffser_setmemory(&recv_buffer,recv_data,recv_capacity);
    SimpleCCPErrorCnt = 0;
    SimpleCCPRevState = SIMPLECCP_REV_WAIT_START;
}

static bool SimpleCCP_escape_char_contain(byte c){
    int i;
    for(i=0;i<sizeof(SimpleCCP_escape_char);i++){
        if(c == SimpleCCP_escape_char[i]){
            return true;
        }
    }
    return false;
}

static void SimpleCCP_send_buffer(){
    int i;
    for(i=0;i<send_buffer.size;i++){
        SimpleCCP_putchar(send_buffer.data[i]);
    }
}


static void SimpleCCPRecvInnerCallback(byte *data,int len){
    SimpleCCPRecvCallback(recv_buffer.data,recv_buffer.size);
    buffer_clear(&recv_buffer);
}
/*
Return:
    success: send data length
    fail: SAMPLE_ERROR
*/
int SimpleCCP_send(byte *data,int len){
    int i;
    //1. empty buffer
    buffer_clear(&send_buffer);
    //2. push message head : "@@"
    buffer_push_str(&send_buffer,CAST_CHAR_PTR_TO_BYTE_PTR("@@"));
    for(i=0;i<len;i++){
        //3. push message body,when encounter '@',replace it with "\\@";when encounter '\',
        //replace it with "\\".'\r' and '\n' are not allowed in message body.
        if(data[i]=='@'){
            if (buffer_push_str(&send_buffer,CAST_CHAR_PTR_TO_BYTE_PTR("\\@"))==OVER_CAPACITY_ERROR)
            {
                return SIMPLECCP_SENDFAILED;
            }
        }else if(data[i]=='\\'){
            if (buffer_push_str(&send_buffer,CAST_CHAR_PTR_TO_BYTE_PTR("\\\\"))==OVER_CAPACITY_ERROR)
            {
                return SIMPLECCP_SENDFAILED;
            }
        }else if(data[i]=='\r' || data[i]=='\n'){
            return SIMPLECCP_SENDFAILED;
        }else{
            if (buffer_push(&send_buffer,data[i])==OVER_CAPACITY_ERROR)
            {
                return SIMPLECCP_SENDFAILED;
            }
        }
    }
    //4. push message tail : "\r\n"
    buffer_push_str(&send_buffer,CAST_CHAR_PTR_TO_BYTE_PTR("\r\n"));
    //5. send message
    SimpleCCP_send_buffer();
    return len;
}
// SimpleCCP receive state machine's states
void SimpleCCP_parse(byte c){
    switch (SimpleCCPRevState)
    {
    case SIMPLECCP_REV_WAIT_START:
        if(c=='@'){
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_NEXT_AT;
        }
        break;
    case SIMPLECCP_REV_WAIT_NEXT_AT:
        if(c=='@'){
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_END;
        }else{
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_START;
        }
        break;
    case SIMPLECCP_REV_WAIT_END:
        if(c=='\r'){
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_LF;
        }else if(c == '\\'){
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_ESC_END;
        }else{
            buffer_push(&recv_buffer,c);
        }
        break;
    case SIMPLECCP_REV_WAIT_LF:
        if(c=='\n'){
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_START;
            SimpleCCPRecvInnerCallback(recv_buffer.data,recv_buffer.size);
        }else{
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_START;
        }
        break;
    case SIMPLECCP_REV_WAIT_ESC_END:
        if(SimpleCCP_escape_char_contain(c)){
            buffer_push(&recv_buffer,c);
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_END;
        }else{
            SimpleCCPErrorCnt++;
            SimpleCCPRevState = SIMPLECCP_REV_WAIT_START;
        }
        break;
    default:
        break;
    }

}
