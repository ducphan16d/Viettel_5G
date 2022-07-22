#include <stdio.h>  
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <ctype.h>
#include <arpa/inet.h>
#include<netinet/in.h>
#include <pthread.h> 

struct NgAP_Paging
{
    int Message_type, TAC, CN_Domain;
    int UE_ID;
};

struct NgAP_Paging NgAP_Paging_AMF;


void *Socket_client_gNB(){
    int sockfd = -1;
    struct sockaddr_in server_addr;
    char secv_buffer[1024];
    char send_buffer[1024];
    int pagingMsg[4];
    int UE_ID;
    time_t ticks;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(10000);

    NgAP_Paging_AMF.Message_type = 100;
    NgAP_Paging_AMF.TAC = 100;
    NgAP_Paging_AMF.CN_Domain = 100;
    NgAP_Paging_AMF.UE_ID = UE_ID;



    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0){
        if(recv ( sockfd, (void*)&UE_ID, sizeof(UE_ID), 0) >= 0){
                printf("\nUE_ID receive from gNB =  %d\n", UE_ID);
            }
			NgAP_Paging_AMF.UE_ID = UE_ID;
			pagingMsg[0] = NgAP_Paging_AMF.Message_type;
			pagingMsg[1] = NgAP_Paging_AMF.UE_ID;
			pagingMsg[2] = NgAP_Paging_AMF.TAC;
			pagingMsg[3] = NgAP_Paging_AMF.CN_Domain;
			if( send(sockfd, (void*)&pagingMsg, sizeof(pagingMsg),0) < 0 ) {
    			printf("send failed!\n");
			}
			printf("\nSend NgAp Paging message to gNB: \n");
			printf("\n%d\n", pagingMsg[0]);
			printf("\n%d\n", pagingMsg[1]);
			printf("\n%d\n", pagingMsg[2]);
			printf("\n%d\n", pagingMsg[3]);	
        }
        else 
            printf("error");

        close(sockfd);
   	pthread_exit(NULL);
}
int main(){
    pthread_t thread_gNB;
    pthread_create(&thread_gNB, NULL, Socket_client_gNB, NULL);
    pthread_join(thread_gNB, NULL);
    printf("\nend AMF\n");
    return 0;
}
    
