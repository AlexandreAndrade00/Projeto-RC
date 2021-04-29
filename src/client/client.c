#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFSIZE 1024	// Tamanho do buffer

pid_t pid;
int fd;
struct sockaddr_in addr;
char buffer[BUFFSIZE];
pthread_t receive;
socklen_t slen = sizeof(addr);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void erro(char *msg);
void *receiveMsg(void *arguments);
void userInteration();

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("cliente <server host> <port>\n");
		exit(-1);
	}
	
	int porto = (int) strtol(argv[2], (char **) NULL, 10);
	
	struct hostent *hostPtr;
	char endServer[100];

	strcpy(endServer, argv[1]);
	if ((hostPtr = gethostbyname(endServer)) == 0)
		erro("Não consegui obter endereço");

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons(porto);

	if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
		erro("socket");
		
	printf("----------------------COMANDOS--------------------\n\n");
	printf("AUTH - AUTENTICAR NO SERVER\n");
	printf("SEND - MANDAR MENSAGEM PARA UM UTILIZADOR ATRAVES DO SERVER\n");
	printf("QUIT - DESCONECTAR DO SERVIDOR E TERMINAR PROGRAMA\n\n\n");
			
	userInteration();
	
	
	pthread_mutex_destroy(&mutex);
	close(fd);
	return 0;
}

void userInteration() {
	char *string, *found;
	printf("Insira dados de autenticacao (AUTH <username> <password>):\n");
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	sendto(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, slen);
	recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *) &slen);
	if (buffer[0]=='A') {
		printf("%s\n", buffer);
		pthread_create(&receive, NULL, receiveMsg, NULL);
	
		while(1) {
			//send messsage to server
			//pthread_mutex_lock(&mutex);
			fgets(buffer, sizeof(buffer), stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			sendto(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, slen);
			string = strdup(buffer);
			found = strsep(&string, " ");

			//TODO resto de funcionalidades (primeiro server)
			/*if(strcmp(found, "AUTH")==0) {
				recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *) &slen);
				printf("%s\n", buffer);
			} else*/ 
			if(strcmp(found, "QUIT")==0) {
				pthread_cancel(receive);
				break;
			}
		}
		pthread_join(receive, NULL);
	} else
		printf("%s\n", buffer);
}

void *receiveMsg(void *arguments) {
	while(1) {
		recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *) &slen);
		printf("%s\n", buffer);
		//pthread_mutex_unlock(&mutex);
	}
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
