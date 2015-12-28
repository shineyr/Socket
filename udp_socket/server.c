#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/ip.h>

const int SERV_PORT = 6000;
const int MAXLINE = 2048;

void dg_echo(int sockfd , struct sockaddr *pcliaddr , socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];

	for( ; ;)
	{
		len = clilen;
		if((n = recvfrom(sockfd , mesg , MAXLINE , 0 , pcliaddr , &len))<0)
		{
			perror("recvfrom error");
			exit(1);
		}//if

		if((n = sendto(sockfd , mesg , n , 0 , pcliaddr , len)) < 0)
		{
			perror("sendto error");
			exit(1);
		}//if
	}//for

}

int main(int argc , char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr , cliaddr;

	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if((sockfd = socket(AF_INET , SOCK_DGRAM , 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}//if

	if(bind(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)))
	{
		perror("bind error");
		exit(1);
	}//if

	dg_echo(sockfd , (struct sockaddr *)&cliaddr , sizeof(cliaddr));	
}
