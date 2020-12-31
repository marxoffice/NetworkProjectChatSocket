#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#ifndef MARX_SOCK_APP    // close the main function and som constants of sender.c receive.c
#define MARX_SOCK_APP
#endif

#include "receiver.c"
#include "sender.c"

pthread_t rcv_t;

int main(int argc, char *argv[])
{

	if (argc < 3)
    {
        printf("usage: %s <serverIP> <serverPort> <Myport>\n", argv[0]);
        exit(1);
    }
    //argv[1]  host ip
	short HostPort = atoi(argv[2]);
	short MyPort = atoi(argv[3]);

    printf("ServerPort:%d\nMyPort:%d\n",HostPort,MyPort);

	printf("Test of sock app,input what you want to send\n");
	pthread_create(&rcv_t, NULL, receiver, (void*)&MyPort);
	pthread_detach(rcv_t);
	sender(argv[1], HostPort, MyPort); // argv[1] is host ip
	return 0; 
}
