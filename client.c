#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <signal.h>


void sigintHandler(int sig_num) 
{ 
    printf("exit(Y/N)");
    if(getchar()=='Y') exit(1);
} 

void *recvmg(void *sock)
{
	int their_sock = *((int *)sock);
	char msg[500];
	int len;
	while((len = recv(their_sock,msg,500,0)) > 0) {
		printf("%s\n",msg);
		for(int i=0;i<500;i++)
		{
	  	  msg[i]=0;	
		} 
	}
}

int main(int argc, char const *argv[]) 
{ 
	int sock = 0;
	pthread_t sendt,recvt;
	struct sockaddr_in serv_addr; 
	char msg[500];  
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(argv[1])); 
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
    else
	    printf("connected"); // added else

    char name[25],send_name[25];
    strcpy(name,argv[3]);
    printf("%s\n",name);
    
    int sel = htonl(atoi(argv[4]));
    printf("%d\n",ntohl(sel));
   
    if(atoi(argv[4]) == 0){
        printf("enter name to send direct message");
        scanf("%s",send_name);}

    send(sock ,&sel, sizeof(sel),0);
    send(sock ,name, sizeof(name) , 0 );

    if( argv[4] == 0)
    {   
        printf("Hi\n");
        send(sock ,send_name, sizeof(send_name) , 0 );
        printf("%s\n",send_name);
    }

	signal(SIGINT, sigintHandler); 
	pthread_create(&recvt,NULL,recvmg,&sock);
	while(1){
	
	scanf("%s",msg);
	send(sock ,msg, strlen(msg) , 0 );
	for(int i=0;i<500;i++)
	{
	  msg[i]=0;	
	} 

	}
	pthread_join(recvt,NULL);
	return 0; 
} 