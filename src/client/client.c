#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFSIZE 1024	// Tamanho do buffer

void erro(char *msg);

int main(int argc, char *argv[]) {
	struct sockaddr_in addr;
	struct hostent *hostPtr;
	char endServer[100], buffer[BUFFSIZE], *string, *found;
	int fd;
	socklen_t slen = sizeof(addr);

	if (argc != 3) {
		printf("cliente <server host> <port>\n");
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
		
	while(1) {
		//send messsage to server
		printf("Instruction: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = 0;
		sendto(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, slen);
		string = strdup(buffer);
		found = strsep(&string, " ");

		//TODO resto de funcionalidades (primeiro server)
		if(strcmp(found, "AUTH")==0) {
			recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *) &slen);
			printf("%s\n", buffer);
		} else if(strcmp(found, "QUIT")==0)
			break;
	}
	close(fd);
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
