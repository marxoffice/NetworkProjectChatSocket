#ifndef MARX_SOCKET_TEST   // close the main function and som constants of totaltest.c
#define MARX_SOCKET_TEST
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* close */
#include "totaltest.c"

#define SUCCESS 0
#define ERROR 1

/**
 * receive message from sender
 */
void* receiver(void *arg)
{
    int ReceiverPort = *(short *)arg;
    printf("\nCreate Port%d",ReceiverPort);

    int sd, newSd, cliLen;

    struct sockaddr_in cliAddr, servAddr;
    char line[MAX_MSG];

    Byte data[MAX_MSG];
    Byte* frames[10];
    int FrameLength[10];
    int framecnt=1;

    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("cannot open socket ");
        exit(ERROR);
    }

    /* bind server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(ReceiverPort);

    if (bind(sd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("cannot bind port ");
        exit(ERROR);
    }

    listen(sd, 5);

    while (1)
    {

        printf("receiver: waiting for data on port TCP %d\n", ReceiverPort);

        cliLen = sizeof(cliAddr);
        newSd = accept(sd, (struct sockaddr *)&cliAddr, &cliLen);
        if (newSd < 0)
        {
            perror("cannot accept connection ");
            exit(ERROR);
        }

        /* init line */
        memset(line, 0x0, MAX_MSG);

        /* receive segments */
        if((FrameLength[0] = recv(newSd, line, MAX_MSG, 0)) != 0)
        {
            printf("get buffer:%d\n",FrameLength[0]);
            BeforeRecv(line,data);
            printf("receiver: received from %s:TCP%d :\n%s\n",
                   inet_ntoa(cliAddr.sin_addr),
                   ntohs(cliAddr.sin_port),data);
            /* init line */
            memset(line, 0x0, MAX_MSG);
            memset(data, 0x0, MAX_MSG);

        } /* while(read_line) */

    } /* while (1) */
}

#ifndef MARX_SOCK_APP

#define SERVER_PORT 3044
int main(int argc, char *argv[])
{
    int sd, newSd, cliLen;

    struct sockaddr_in cliAddr, servAddr;
    char line[MAX_MSG];

    Byte data[MAX_MSG];
    Byte* frames[10];
    int FrameLength[10];
    int framecnt=1;


    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("cannot open socket ");
        return ERROR;
    }

    /* bind server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERVER_PORT);

    if (bind(sd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("cannot bind port ");
        return ERROR;
    }

    listen(sd, 5);

    while (1)
    {

        printf("%s: waiting for data on port TCP %u\n", argv[0], SERVER_PORT);

        cliLen = sizeof(cliAddr);
        newSd = accept(sd, (struct sockaddr *)&cliAddr, &cliLen);
        if (newSd < 0)
        {
            perror("cannot accept connection ");
            return ERROR;
        }

        /* init line */
        memset(line, 0x0, MAX_MSG);

        /* receive segments */
        while ((FrameLength[0] = recv(newSd, line, MAX_MSG, 0)) != 0)
        {
            printf("get buffer:%d\n",FrameLength[0]);
            BeforeRecv(line,data);
            printf("%s: received from %s:TCP%d :\n%s\n", argv[0],
                   inet_ntoa(cliAddr.sin_addr),
                   ntohs(cliAddr.sin_port),data);
            /* init line */
            memset(line, 0x0, MAX_MSG);
            memset(data, 0x0, MAX_MSG);

        } /* while(read_line) */

    } /* while (1) */
}
#endif