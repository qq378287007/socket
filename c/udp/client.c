#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <sys/socket.h>
#include <arpa/inet.h>
//#include <netinet/in.h>

const char *IP = "127.0.0.1";
const int SERV_PORT = 9999;
const int MAXLINE = 2048;

void error_handling(const char *message)
{
	perror(message);
	exit(1);
}

int main(int argc, char **argv)
{
	socklen_t servlen = sizeof(struct sockaddr_in);

	struct sockaddr_in servaddr;
	bzero(&servaddr, servlen);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0)
		error_handling("inet_pton error");

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error_handling("socket error");

	char sendline[MAXLINE];
	while (fgets(sendline, MAXLINE, stdin) != NULL)
	{
		if (sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, servlen) < 0)
			error_handling("sendto error");

		char recvline[MAXLINE + 1];
		int n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		if (n < 0)
			error_handling("recvfrom error");

		recvline[n] = '\0';
		fputs(recvline, stdout);
	}

	close(sockfd);
	return 0;
}
