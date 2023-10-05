#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <errno.h>
//#include <netdb.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>

const int PORT = 9999;
const int LISTENQ = 666;
const int MAX_LINE = 2048;

void error_handling(const char *message)
{
	perror(message);
	exit(1);
}

int main()
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		error_handling("socket error");

    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1)
        error_handling("setsockopt() error");

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		error_handling("bind error");

	if (listen(listenfd, LISTENQ) < 0)
		error_handling("listen error");

	while (1)
	{
		int connfd = accept(listenfd, NULL, NULL);
		if (connfd < 0)
			error_handling("accept error");

		pid_t pid = fork();
		if (pid < 0)
			error_handling("fork error");

		if (pid == 0)
		{
			close(listenfd);

			ssize_t n;
			char buff[MAX_LINE];
			while (n = read(connfd, buff, MAX_LINE))
			{
				if (n < 0)
					break;
				write(connfd, buff, n);
			}
			close(connfd);
			exit(0);
		}
		else
			close(connfd);
	}

	close(listenfd);
	return 0;
}