#include "SimpleCCP.h"
#include <stdio.h>
#include <string.h>
__unimplemented void SimpleCCPRecvCallback(byte *data,int len){
    __unimplemented
    printf("SimpleCCPRecvCallback: ");
    // print data
    size_t i = 0;
    for (i; i < len; i++)
    {
        putchar(data[i]);
    }
    
}
__unimplemented byte SimpleCCP_putchar(byte c){
    __unimplemented
    putchar(c);
    SimpleCCP_parse(c);
}

int main(void)
{
    #define BUFFER_SIZE 1024
    byte send_data[BUFFER_SIZE];
    byte recv_data[BUFFER_SIZE];

    SimpleCCP_init(send_data,BUFFER_SIZE,recv_data,BUFFER_SIZE);
    printf("Test SimpleCCP:");
    char *msg = "hello worl@\\00000d";
    if (SimpleCCP_send(msg,strlen(msg)) == SIMPLECCP_SENDFAILED)
    {
        printf("send error\n");
    }
    return 0;
}