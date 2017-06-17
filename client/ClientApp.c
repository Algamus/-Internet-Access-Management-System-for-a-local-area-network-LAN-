#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

//prototype
bool isExpired(int*,int*,int*);
int hasAuthentication(char*,int,int*,int*,int*);
int getAuthentication(char*,int,int*,int*,int*);
int cnvrtr(char);
///////////////////////77//IHAVEANYTICKET eklencek

int main(void){
	char* serverip="192.168.15.6";//server ip
	int port=22000;//server port

	int status;
	int answer;
	int ticket=0;
	int hour=0;
	int min=0;
	int sec=0;
	status=system("clear");
	


	ticket=hasAuthentication(serverip,port,&hour,&min,&sec);

	

	while(1){
	
	if(ticket==0){
		printf("Ticket: Expired\n-----------------------\n");
		printf("Enter 1: Get New Ticket\nEnter 2:Exit\n");
		printf(">> ");
		scanf("%d",&answer);
		if(answer==2){
			exit(0);
		}
		if(answer==1){
			//TCP connection get ticket
			printf(">> GOGO\n");
			
			ticket=getAuthentication(serverip,port,&hour,&min,&sec);
			
		}
	
	}else{
		printf("\nTicket: End %d-%d-%d\n-----------------------\n",hour,min,sec);
		sleep(5);
	}
	if(isExpired(&hour,&min,&sec)){
		ticket=0;
	}

	status=system("clear");
	}

	return 0;
}
bool isExpired(int* h,int* m,int* s){
	
	time_t t =time(NULL);
	struct tm tm =*localtime(&t);

	if(tm.tm_hour<=*h){
		if(tm.tm_min<*m){
			return false;
		}else if(tm.tm_min<=*m){
			if(tm.tm_sec<*s){
				return false;
			}	
		}else{
			return true;
		}
	}


	return true;
}
int hasAuthentication(char* ip,int port,int* h,int* m,int* s){
	int sockfd;
	char recvline[7];
	struct sockaddr_in servaddr;
	
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	inet_pton(AF_INET,ip,&(servaddr.sin_addr));
	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));	
		
	bzero(recvline,7);
	
	write(sockfd,"IHAVEANYTICKT",13);
	read(sockfd,recvline,7);
	*h=(cnvrtr(recvline[1]))*10+(cnvrtr(recvline[2]));
	*m=(cnvrtr(recvline[3]))*10+(cnvrtr(recvline[4]));
	*s=(cnvrtr(recvline[5]))*10+(cnvrtr(recvline[6]));
	close(sockfd);	
	if(recvline[0]=='1'){
		return 1;
	}else{
		return 0;
	}
	return 0;
}

int getAuthentication(char* ip,int port,int* h,int* m,int* s){
	int sockfd;
	char recvline[7];
	struct sockaddr_in servaddr;
	
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	inet_pton(AF_INET,ip,&(servaddr.sin_addr));
	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));	
		
	bzero(recvline,7);
	
	//strcpy(recvline,"1016031");//For test....
	write(sockfd,"IWANTINTERNET",13);
	read(sockfd,recvline,7);
	*h=(cnvrtr(recvline[1]))*10+(cnvrtr(recvline[2]));
	*m=(cnvrtr(recvline[3]))*10+(cnvrtr(recvline[4]));
	*s=(cnvrtr(recvline[5]))*10+(cnvrtr(recvline[6]));
	close(sockfd);	
	if(recvline[0]=='1'){
		return 1;
	}else{
		return 0;
	}
	return 0;
}
int cnvrtr(char c){
	if(c=='0'){
		return 0;
	}else if(c=='1'){
		return 1;
	}else if(c=='2'){
		return 2;
	}else if(c=='3'){
		return 3;
	}else if(c=='4'){
		return 4;
	}else if(c=='5'){
		return 5;
	}else if(c=='6'){
		return 6;
	}else if(c=='7'){
		return 7;
	}else if(c=='8'){
		return 8;
	}else if(c=='9'){
		return 9;
	}else{
		return -1;
	}
	return -1;
}
