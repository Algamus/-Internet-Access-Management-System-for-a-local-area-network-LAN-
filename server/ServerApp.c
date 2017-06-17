#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <pthread.h>//gcc ServerApp.c -o ServerApp -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

//header
#include "AuthenticationCard.h"
//prototype

void listeningConnections(void*);
void speakWithTheClient(void*);
void serverListprint(void*);
void createAuthenticationCard(LIST*,char*,char*);
void deleteAuthenticationCard(CARD*);
void printAuthenticationList(LIST*);
bool isExpired(CARD*);
int cnvrtr(char);
char* hasAuthentication(char*,LIST*);
char *getTicketkey(void);
//Global

char* password="1234";//linux oturum sifresi
int theport=22000;//serverdinledigiport numarsı

char* acceptip="./shells/iptablesAcceptTheForward.sh";
char* dropall="./shells/iptablesBlockAllForwarding.sh";
char* dropip="./shells/iptablesDropTheForward.sh";


///////////////////////
typedef struct threadpackage{
	LIST* authorizedIPlist;
	int semophore;
	int port;
	
}Tpack;

typedef struct threadpackage2{
	Tpack* pkt;
	int comm;
	struct sockaddr_in *cli;
	int *clngth;
}Tpacksecond;

int main(void){

	pthread_t AuthenticationServer;
	pthread_t ServerList;
	char str[80];
	
	strcpy(str,dropall);
	strcat(str," ");
	strcat(str,password);
	system(str);

	////Threadler baslıyor
	
	Tpack* p=(Tpack *)malloc(sizeof(Tpack));

	p->authorizedIPlist=(LIST*)malloc(sizeof(LIST));
	p->authorizedIPlist->head=NULL;
	p->authorizedIPlist->tail=NULL;
	p->authorizedIPlist->count=0;
	p->semophore=1;
	p->port=theport;
	

	pthread_create(&AuthenticationServer,NULL,serverListprint,(void *) p);
	pthread_create(&ServerList,NULL,listeningConnections,(void *) p);

	pthread_join(AuthenticationServer,NULL);
	pthread_join(ServerList,NULL);

	return 0;
}
void listeningConnections(void *thepacket){//??Authenticationserver 1



	Tpack *p = (Tpack *) thepacket;
	pthread_t privateConnection;

	struct sockaddr_in *client;
	int *clength;
	
	int listen_fd, comm_fd;
	struct sockaddr_in servaddr;
	listen_fd= socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	
	servaddr.sin_addr.s_addr= htons(INADDR_ANY);
	servaddr.sin_port= htons(p->port);
	
	bind(listen_fd,(struct sockaddr *) &servaddr, sizeof(servaddr));

	

	listen(listen_fd,10);//max 10 connection
	
	
	while(1){
		
		client=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		clength=(int *)malloc(sizeof(int));
		*clength=sizeof(struct sockaddr_in);
		
		comm_fd = accept(listen_fd,(struct sockaddr*)client, clength);
		
		Tpacksecond *packed=(Tpacksecond *)malloc(sizeof(Tpacksecond));
		packed->pkt=p;
		packed->comm=comm_fd;
		packed->cli=client;
		packed->clngth=clength;

		privateConnection=(pthread_t)malloc(sizeof(pthread_t));
		pthread_create(&privateConnection,NULL,speakWithTheClient,(void *) packed);

	}
}
void speakWithTheClient(void *thepacket){//??Authenticationserver 2
	Tpacksecond *p = (Tpacksecond *) thepacket; 
	char str[100];
	char key[7];
	bzero(str,100);
	read(p->comm,str,100);

	char ip[40];	
	struct sockaddr_in client=*(p->cli);
	int clength=*(p->clngth);

	bzero(ip,40);
	bzero(key,7);	
	inet_ntop(AF_INET,(void *)&(client.sin_addr),ip,clength);
	
	//printf("\nip %s\n",ip);
	while(p->pkt->semophore<1);
	p->pkt->semophore--;
		strcpy(key,hasAuthentication(ip,p->pkt->authorizedIPlist));
	p->pkt->semophore++;
	if(strcmp(str,"IWANTINTERNET")==0){
		//check has have a ticket
	
		if(strcmp(key,"0000000")==0){
			bzero(key,7);
			strcpy(key,"1");
			strcat(key,getTicketkey());
			write(p->comm,key,7);
			//ekleme semephore artı create contact card
			while(p->pkt->semophore<1);
			p->pkt->semophore--;

				//inside semephore
				createAuthenticationCard(p->pkt->authorizedIPlist,ip,key);
			
				printAuthenticationList(p->pkt->authorizedIPlist);
				
			p->pkt->semophore++;
		}else{
			write(p->comm,key,7);
		}
		
		
	}
	
	if(strcmp(str,"IHAVEANYTICKT")==0){

		
		//check has have a ticket
		write(p->comm,key,7);//
		
		
	}
	
	close(p->comm);
}
void serverListprint(void *thepacket){//??serverlist
	Tpack *p = (Tpack *) thepacket;
	
	while(1){
		while(p->semophore<1);
		p->semophore--;

			printAuthenticationList(p->authorizedIPlist);
			
		p->semophore++;
		sleep(5);
	}
}

void createAuthenticationCard(LIST* list,char* theip,char ticket[7]){
	CARD* newcard;
	char str[80];
	int x;
	if(!(newcard=(CARD*)malloc(sizeof(CARD)))){
		printf("\n !!ERROR!!\nHafızada yer yok!!\n");
		return;
	}
	if(!(newcard->ip=(char*)malloc(sizeof(char)))){
		printf("\n !!ERROR!!\nHafızada yer yok!!\n");
		return;
	}
	
	newcard->next=NULL;
	strcpy(newcard->ip,theip);


	int h1,h2,m1,m2,s1,s2;
	h1=cnvrtr(ticket[1]);
	h1=h1*10;
	h2=cnvrtr(ticket[2]);
	newcard->hour=h1+h2;
	m1=cnvrtr(ticket[3]);
	m1=m1*10;
	m2=cnvrtr(ticket[4]);
	newcard->minute=m1+m2;
	s1=cnvrtr(ticket[5]);
	s1=s1*10;
	s2=cnvrtr(ticket[6]);
	newcard->second=s1+s2;
	//newcard->hour=00;//cnvtr(ticket[1])*10+cnvrtr(ticket[2]);
	//newcard->minute=00;//cnvtr(ticket[3])*10+cnvrtr(ticket[4]);
	//newcard->second=00;//cnvtr(ticket[5])*10+cnvrtr(ticket[6]);
	
	if(list->count==0){
		list->head=newcard;
		list->tail=newcard;
	}else{
		CARD* temp=list->tail;
		temp->next=newcard;
		list->tail=newcard;
	}
	(list->count)++;

	strcpy(str,acceptip);
	strcat(str," ");
	strcat(str,password);
	strcat(str," ");
	strcat(str,newcard->ip);
	system(str);

}
void deleteAuthenticationCard(CARD* card){
	if(card!=NULL){
		if(card->ip!=NULL){
			free(card->ip);	
		}
		if(card!=NULL){
			free(card);
		}
	}
}
void printAuthenticationList(LIST* list){
	CARD* temp;
	CARD* before=NULL;

	char str[80];	

	temp=list->head;
	system("clear");
	printf("\nAuthentication IP list\n----------------------\n");

	
	
	while(temp!=NULL){
		if(isExpired(temp)){
			
			if(temp==list->head){//basta ise

				strcpy(str,dropip);
				strcat(str," ");
				strcat(str,password);
				strcat(str," ");
				strcat(str,temp->ip);
				system(str);
			
				list->head=temp->next;
				before=temp;
				temp=temp->next;
				
				//printf("dump is here");

				//deleteAuthenticationCard(before);
				before=NULL;
			}else if(temp==list->tail){//sonda ise
	
				strcpy(str,dropip);
				strcat(str," ");
				strcat(str,password);
				strcat(str," ");
				strcat(str,temp->ip);
				system(str);				

				list->tail=before;
				
				//deleteAuthenticationCard(temp);
				temp=NULL;
			}else{//ortada ise

				strcpy(str,dropip);
				strcat(str," ");
				strcat(str,password);
				strcat(str," ");
				strcat(str,temp->ip);
				system(str);
			
				before->next=temp->next;
				
				//deleteAuthenticationCard(temp);
				temp=before->next;
			}
			(list->count)--;
		}else{
			
			printf("%s Allowed untill %d-%d-%d\n",temp->ip,temp->hour,temp->minute,temp->second);
			before=temp;
			temp=temp->next;
			
		}
	}
	
}
bool isExpired(CARD* card){
	time_t t = time(NULL);
	struct tm tm =*localtime(&t);

	if(tm.tm_hour<=card->hour){
		if(tm.tm_min<card->minute){
			return false;
		}else if(tm.tm_min==card->minute){
			if(tm.tm_sec<card->second){
				
				return false;
			}
		}else{
			return true;
		}
	}
	return true;
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
char* hasAuthentication(char* ip,LIST *l){
	CARD* temp;
	char str[7];
	char buffer[2];
	bzero(str,7);
	bzero(buffer,2);	

	temp=l->head;
	if(l->head==NULL){
		return "0000000";
	}
	while(temp!=NULL){
		if(strcmp(temp->ip,ip)==0){
			
			strcpy(str,"1");
			
			int h,m,s;
			h=temp->hour;
			m=temp->minute;
			s=temp->second;
			

			sprintf(buffer,"%02d",h);
			strcat(str,buffer);
		
			sprintf(buffer,"%02d",m);
			strcat(str,buffer);
			

			sprintf(buffer,"%02d",s);	
			strcat(str,buffer);
			
		
			return str;
			
		}
		temp=temp->next;
	}
	return "0000000";
}
char *getTicketkey(void){
	char str[6];
	char buffer[2];

	time_t t = time(NULL);
	struct tm tm =*localtime(&t);

	int h,m,s;
	s=tm.tm_sec;

	if((tm.tm_min+3)%60!=tm.tm_min){
		m=(tm.tm_min+3)%60;//burda disconnect time;??3 dk
		if((tm.tm_hour+1)%24!=tm.tm_hour+1){
			h=(tm.tm_hour+1)%24;
		}else{
			h=tm.tm_hour+1;
		}
	}else{
		m=tm.tm_min+3;

	}	
	sprintf(buffer,"%02d",h);
	strcpy(str,buffer);
	sprintf(buffer,"%02d",m);
	strcat(str,buffer);
	sprintf(buffer,"%02d",s);	
	strcat(str,buffer);
	return str;
}
