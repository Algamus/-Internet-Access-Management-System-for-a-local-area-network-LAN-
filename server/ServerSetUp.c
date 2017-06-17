#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	char* routesetupshell="./shells/ServerRoutingSetUp.sh";
	char* pass="1234";
	int status;
	char str[80];
	strcpy(str,routesetupshell);
	strcat(str," ");
	strcat(str,pass);
	status = system(str);
	status = system("clear");
	printf("System Routing Done.\n");
	return 0;
}
