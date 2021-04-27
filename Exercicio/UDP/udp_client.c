#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define BUFLEN 1024	// Tamanho do buffer

void erro(char *msg);

int main(int argc, char *argv[]) {
	struct sockaddr_in addr;
	struct hostent *hostPtr;
	char endServer[100];
	char buf[BUFLEN];
	int send_len, fd, recv_len;
	socklen_t slen = sizeof(addr);

	if (argc != 4) {
		printf("cliente <host> <port> <string>\n");
		exit(-1);
	}

	strcpy(endServer, argv[1]);
	if ((hostPtr = gethostbyname(endServer)) == 0)
		erro("Não consegui obter endereço");

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons((short) atoi(argv[2]));

	if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
		erro("socket");
		
	if((send_len = sendto(fd, argv[3], strlen(argv[3]) + 1, 0, (struct sockaddr *) &addr, slen)) == -1) {
		erro("sendto");
	}
	if((recv_len = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *) &addr, (socklen_t *) &slen)) == -1) {
	 		erro("Erro no recvfrom");
	}
	printf("Numero de carateres: %s\n", buf);
	close(fd);
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
	  

