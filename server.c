#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#define PORT 8888

char members[100][25];
int sel;
int msgtype;
int client_index[100]; // array to store names of clients
int n=0,member_count=0;
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int i,j;
    int cl_id = *(int*)socket_desc; //client socket descriptor
    char name[25],send_name[25];
    int sendcl_id;

    recv(cl_id ,name,sizeof(name),0);
    strcpy(members[member_count++],name);
    //printf("%d\n",member_count);
	client_index[n]=cl_id;  
	n++;

    int read_size =0;
    char client_message[2000];
    while((read_size =recv(cl_id , client_message ,sizeof(client_message), 0))>0){ //Receive a message from client
        
        recv(cl_id ,&sel,sizeof(sel),0);
        //printf("%d\n",ntohl(sel));
        if(ntohl(sel) == 1){
            msgtype = htonl(1);
            //Send message to  all remaining clients in group 
	        printf("%s\n",client_message);       
	        for(i=0;i<n;i++){
	            if(client_index[i] != cl_id){ // to send message all the remaining client
                    send(client_index[i], client_message , sizeof(client_message),0);	
                    send(client_index[i],&msgtype,sizeof(msgtype),0);
                    send(client_index[i], name , sizeof(name),0);	
	            }
	        }
        }       
        else { // send message to a single person
            msgtype = htonl(0);
            recv(cl_id ,send_name,sizeof(send_name),0);
            //printf("%s\n",send_name);
            for(i=0;i<member_count;i++){
                if(strcmp(members[i],send_name)==0){
                    //printf("%s\n",members[i]);
	                sendcl_id = client_index[i]; // to send message to particlualr client
                    send(sendcl_id, client_message , sizeof(client_message),0);	
                    send(client_index[i],&msgtype,sizeof(msgtype),0);
                    send(sendcl_id, name , sizeof(name),0);    
	            }
	        }
        } 
    }           
  
   if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    
    for(i = 0; i < n; i++) {
		if(client_index[i] == cl_id) {
			j = i;
			while(j < n-1) {
				client_index[j] = client_index[j+1];
				j++;
			}
		}
	}
	member_count--;
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
}


int main(int argc, char *argv[])
{
    int socket_desc ,client_sock, *new_sock,i;
    pthread_t sendt,recvt;
    int opt=1;
    struct sockaddr_in serv_addr,client;
    int addrlen = sizeof(struct sockaddr_in );
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
	
    puts("Socket created");
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT); 

    bind(socket_desc, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   
    listen(socket_desc, 150); 
    
    while((client_sock = accept(socket_desc, (struct sockaddr *)&client,  
                       (socklen_t*)&addrlen)))
    {

        if( pthread_create( &recvt , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	    else  // added else
            puts("Handler assigned");

     }    
return 0; 
} 
