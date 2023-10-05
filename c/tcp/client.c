#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_LINE 2048

const char *IP = "127.0.0.1";
const int PORT = 9999;

void error_handling(const char *message)
{
	perror(message);
	exit(1);
}

ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t n;

	char *ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		char c;
		ssize_t rc = read(fd, &c, 1);
		if (rc < 0)
			return -1;

		if (rc == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break; /* newline is stored, like fgets() */
		}
		else
		{
			*ptr = 0;
			return (n - 1); /* EOF, n - 1 bytes were read */
		}
	}

	*ptr = 0; /* null terminate like fgets() */
	return n;
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		error_handling("socket error");

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) < 0)
	{
		printf("inet_pton error for %s\n", IP);
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		error_handling("connect error");

	char sendline[MAX_LINE] = {0};
	while (fgets(sendline, MAX_LINE - 1, stdin))
	{
		write(sockfd, sendline, strlen(sendline));

		char recvline[MAX_LINE];
		if (readline(sockfd, recvline, MAX_LINE) == 0)
			error_handling("server terminated prematurely");

		if (fputs(recvline, stdout) == EOF)
			error_handling("fputs error");
	}

	close(sockfd);
	return 0;
}