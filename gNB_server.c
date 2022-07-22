#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <ctype.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<unistd.h>
#include <pthread.h>
#include <math.h>


struct Paging_Message
{
    int Message_type;
    int UE_ID;
    int TAC;
    int CN_Domain;
};
struct Paging_Message NgAP_Paging_AMF;
uint16_t queue_number = 0;
int SFN_gNB = 0;
int UE_ID = 0;	
uint16_t Flag_UE_ID, Flag_Qnumber = 0;
struct Paging_Message queue[1024];

void *Couting(){
	printf("Thread couting SFN_gNB run !\n");
	while(1)
	{
		if(SFN_gNB == 1023){
		SFN_gNB = 0;
		}
		else{
			SFN_gNB++;
		}
		printf("SFN_gNB =  %d\n", SFN_gNB);
		sleep(1);
	}
	pthread_exit(NULL);
}



void *Socket_server_AMF()
{ 
    int server_fd = -1, new_socket = -1; 
    struct sockaddr_in server_addr; 
    time_t ticks;
    int addrlen = sizeof(server_addr); 
    char send_buffer[1024]; 
    char recv_buffer[1024];
    int NgAP_Paging_buff[4];
    
     
    memset(&server_addr, 0, sizeof(server_addr));
    memset(send_buffer, 0, sizeof(send_buffer));
    memset(recv_buffer, 0, sizeof(recv_buffer));
    memset(&NgAP_Paging_AMF, 0, sizeof(NgAP_Paging_AMF));
    memset(&NgAP_Paging_buff, 0, sizeof(NgAP_Paging_buff));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
        printf("error create socket");

    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    server_addr.sin_port = htons(10000);   
    
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_fd, 3) ;
    
    
    while(1){    
    new_socket = accept(server_fd, (struct sockaddr*)NULL, NULL);
		while(Flag_UE_ID == 0){
		sleep(2);
		}

		if(send(new_socket, (void*)&UE_ID, sizeof(UE_ID),0) < 0 ) {
			printf("send failed!\n");
		}


		if(recv(new_socket,(void*)(&recv_buffer),sizeof(recv_buffer), 0)>=0)
		{
			memcpy((void*)NgAP_Paging_buff,(void*)recv_buffer,sizeof(NgAP_Paging_buff));
			
			NgAP_Paging_AMF.Message_type = NgAP_Paging_buff[0];
			NgAP_Paging_AMF.UE_ID =  NgAP_Paging_buff[1];
			NgAP_Paging_AMF.TAC = NgAP_Paging_buff[2];
			NgAP_Paging_AMF.CN_Domain = NgAP_Paging_buff[3];
			
			printf("\nNgAP Paging buffer message receive from AMF: \n");
			printf("Message type: %d\n", NgAP_Paging_AMF.Message_type);
			printf("UE_ID: %d\n", NgAP_Paging_AMF.UE_ID);
			printf("TAC: %d\n", NgAP_Paging_AMF.TAC);
			printf("CN_Domain: %d\n\n", NgAP_Paging_AMF.CN_Domain);			
		}
		if(NgAP_Paging_AMF.Message_type == 100){
			if(NgAP_Paging_AMF.TAC == 100){
				if(NgAP_Paging_AMF.CN_Domain == 100 || NgAP_Paging_AMF.CN_Domain == 101){
						int k;
						k = abs(SFN_gNB - NgAP_Paging_AMF.UE_ID)/64 ;
						queue_number = (NgAP_Paging_AMF.UE_ID + k*64)%1024;
						Flag_Qnumber = 1;
						queue[queue_number] = NgAP_Paging_AMF;
				}
			}
		}
		close(new_socket);
		sleep(1);
	}
	close(server_fd);
   	pthread_exit(NULL);
}



void *Socket_sever_UE() {
	printf("Thread socket sever UE run !\n");
		int listenfd = -1;
		int connfd = -1;
		struct sockaddr_in server_addr;
		char send_buffer[1024];
		char recv_buffer[1024];
		int pagingMsg[4];
		time_t ticks;
		

		memset(send_buffer, 0, sizeof(send_buffer));
		memset(recv_buffer, 0, sizeof(recv_buffer));
		memset(&server_addr, 0, sizeof(server_addr));
		memset(&pagingMsg,0,sizeof(pagingMsg));

		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if(listenfd < 0) 
			printf("error create socket");

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		server_addr.sin_port = htons(10001);

		bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		listen(listenfd, 10);
		while(1) {
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);


			if(recv (connfd, (void*)&UE_ID, sizeof(UE_ID), 0) <= 0){
				printf("recive UE_ID fail");
			}
			printf("\nConnect to client UE_ID= %d \n\n",UE_ID);
			Flag_UE_ID = 1;

			while(Flag_Qnumber == 0){
				sleep(2);
			}
			printf("\nQueue number of RRC paging message = %d\n\n",queue_number);
			while (1)
			{
				if(SFN_gNB == queue_number){
				pagingMsg[0] = queue[queue_number].Message_type;
				pagingMsg[1] = queue[queue_number].UE_ID;
				pagingMsg[2] = queue[queue_number].TAC;
				pagingMsg[3] = queue[queue_number].CN_Domain;
				printf("\nCopy NgAP_Paging_buff message to RRC Paging message send to UE: \n");
				printf("Message type: %d\n", pagingMsg[0]);
				printf("UE ID: %d\n", pagingMsg[1]);
				printf("TAC: %d\n", pagingMsg[2]);
				printf("CN_Domain: %d\n\n", pagingMsg[3]);
				if( send(connfd, (void*)&pagingMsg, sizeof(pagingMsg),0) < 0 ) {
    				printf("send failed!\n");
					}		
				break;
				}
				sleep(1);
			}
			Flag_Qnumber = 0;
			close(connfd);
		}
		close(listenfd);
   	pthread_exit(NULL);
}

int main(){
    pthread_t thread_AMF, thread_UE,thread_SFN_gNB;

	pthread_create(&thread_AMF, NULL, Socket_server_AMF, NULL);
	pthread_create(&thread_UE, NULL, Socket_sever_UE, NULL);
	pthread_create(&thread_SFN_gNB, NULL, Couting, NULL);

    pthread_join(thread_AMF, NULL);
	pthread_join(thread_UE, NULL);

    return 0;
}

 
