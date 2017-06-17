#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	char* routesetupshell="./shells/ClientRoutingSetUp.sh";
	char* pass="1234";
	char* serverip="192.168.15.6";
	int status;
	char str[80];
	strcpy(str,routesetupshell);
	strcat(str," ");
	strcat(str,pass);
	strcat(str," ");
	strcat(str,serverip);
	status = system(str);
	status = system("clear");
	printf("System Routing Done.\n");
	status = system("route");
	return 0;
}
