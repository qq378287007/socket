#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
//#include <sys/types.h>
//#include <sys/socket.h>

const int SERV_PORT = 9999;
const int MAXLINE = 2048;

void error_handling(const char *message)
{
	perror(message);
	exit(1);
}

int main(int argc, char **argv)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error_handling("socket error");

	socklen_t len = sizeof(struct sockaddr_in);

	struct sockaddr_in servaddr;
	bzero(&servaddr, len);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	if (bind(sockfd, (struct sockaddr *)&servaddr, len))
		error_handling("bind error");

	while (1)
	{
		char mesg[MAXLINE];
		struct sockaddr cliaddr;
		int n = recvfrom(sockfd, mesg, MAXLINE, 0, &cliaddr, &len);
		if (n < 0)
			error_handling("recvfrom error");

		if (sendto(sockfd, mesg, n, 0, &cliaddr, len) < 0)
			error_handling("sendto error");
	}

	close(sockfd);
	return 0;
}
