#ifndef MARX_SOCKET_TEST    // close the main function and som constants of totaltest.c
#define MARX_SOCKET_TEST
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include "totaltest.c"



int sender(char* ServerIp, int ServerPort,int MyPort){
    int sd, rc, i,j;
    struct sockaddr_in localAddr, servAddr;
    struct hostent *h;

    Byte* frames[10];
    int FrameLength[10];
    int frameCnt;
    Byte* toSend;
    char InputBuff[MAX_MSG];

    h = gethostbyname(ServerIp);
    if (h == NULL)
    {
        printf("sender: unknown Server, if send-recv in same pc, input 127.0.0.1 '%s'\n",ServerIp);
        exit(1);
    }

    servAddr.sin_family = h->h_addrtype;
    memcpy((char *)&servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(ServerPort);

    while(1){
        memset(InputBuff,0x0,MAX_MSG);
        fgets(InputBuff, 1024, stdin);
        printf("\nStart Send");
        frameCnt = BeforeSend(InputBuff,frames,FrameLength);
        printf("FrameCnt:%d\n",frameCnt);

        
        /* create socket */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0)
        {
            perror("cannot open socket ");
            exit(1);
        }

        /* bind any port number */
        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localAddr.sin_port = htons(0);

        // rc = bind(sd, (struct sockaddr *)&localAddr, sizeof(localAddr));
        // if (rc < 0)
        // {
        //     printf("Sender: cannot bind port TCP %u\n", MyPort);
        //     perror("error ");
        //     exit(1);
        // }

        /* connect to server */
        rc = connect(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));
        if (rc < 0)
        {
            perror("cannot connect ");
            exit(1);
        }

        for(j=0;j<frameCnt;j++){    // if msg ip frag
            toSend = frames[j];
            rc = send(sd, toSend, FrameLength[j] + 1, 0);
            if (rc < 0)
            {
                perror("cannot send data ");
                close(sd);
                exit(1);
            }

            printf("sender: data sent %s FrameLength:%d\n" , InputBuff, FrameLength[j]);
        }
    }

    return 0;
}

#ifndef MARX_SOCK_APP

#define SERVER_PORT 3044
int main(int argc, char *argv[])
{

    int sd, rc, i,j;
    struct sockaddr_in localAddr, servAddr;
    struct hostent *h;

    Byte* frames[10];
    int FrameLength[10];
    int frameCnt;
    Byte* toSend;

    if (argc < 3)
    {
        printf("usage: %s <server> <data>\n", argv[0]);
        // exit(1);
        argc = 3;
        argv[1] = "127.0.0.1";
        argv[2] = "Hello World";
    }

    h = gethostbyname(argv[1]);
    if (h == NULL)
    {
        printf("%s: unknown host '%s'\n", argv[0], argv[1]);
        exit(1);
    }

    servAddr.sin_family = h->h_addrtype;
    memcpy((char *)&servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(SERVER_PORT);

    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("cannot open socket ");
        exit(1);
    }

    /* bind any port number */
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (rc < 0)
    {
        printf("%s: cannot bind port TCP %u\n", argv[0], SERVER_PORT);
        perror("error ");
        exit(1);
    }

    /* connect to server */
    rc = connect(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if (rc < 0)
    {
        perror("cannot connect ");
        exit(1);
    }

    for (i = 2; i < argc; i++)
    {
        frameCnt = BeforeSend(argv[i],frames,FrameLength);
        printf("FrameCnt:%d\n",frameCnt);
        for(j=0;j<frameCnt;j++){
            toSend = frames[j];
            
            rc = send(sd, toSend, FrameLength[j] + 1, 0);

            if (rc < 0)
            {
                perror("cannot send data ");
                close(sd);
                exit(1);
            }

            printf("%s: data%u sent (%s), FrameLength:%d\n", argv[0], i - 1, argv[i],FrameLength[j]);
        }
    }

    return 0;
}
#endif