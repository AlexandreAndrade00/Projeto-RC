/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define BUF_SIZE 1024

void erro(char *msg);
void signalHandler(int sig);
void checkServerRead(int nread);
void checkServerWrite(int sig);

int fd;

int main(int argc, char *argv[]) {
	char endServer[100];
	char buffer[BUF_SIZE];
	int nread, nums_received = 0;
	struct sockaddr_in addr;
	struct hostent *hostPtr;

	signal(SIGINT, signalHandler);
	signal(SIGPIPE, checkServerWrite);

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

	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		erro("socket");
	if(connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
		erro("Connect");

	
	while(1) {
		//send messsage to server
		write(fd, argv[3], 1 + strlen(argv[3]));

		if(strcmp(argv[3], "DADOS")==0) {
			for(int i=0; i<10; i++){
				fgets(buffer, BUF_SIZE, stdin);
				write(fd, buffer, 1+strlen(buffer));
			}
			nums_received = 1;
		} else if(strcmp(argv[3], "SOMA")==0) {
			if(nums_received == 1){
				nread = read(fd, buffer, BUF_SIZE-1);
				checkServerRead(nread);
				buffer[nread] = '\0';
				printf("%s\n", buffer);
			} else
				printf("No numbers yet!\n");
		} else if(strcmp(argv[3], "MEDIA")==0) {
			if(nums_received == 1){
				nread = read(fd, buffer, BUF_SIZE-1);
				checkServerRead(nread);
				buffer[nread] = '\0';
				printf("%s\n", buffer);
			} else
				printf("No numbers yet!\n");
		} else if(strcmp(argv[3], "SAIR")==0) {
			if(nums_received == 1){
				nread = read(fd, buffer, BUF_SIZE-1);
				checkServerRead(nread);
				buffer[nread] = '\0';
				printf("%s\n", buffer);
			} else
				printf("No numbers yet!\n");
			break;
		} else
			erro("instrucao nao conhecida\n");
			
		printf("Instruction: ");
		fgets(argv[3], sizeof(argv[3]), stdin);
		argv[3][strcspn(argv[3], "\n")] = 0;
	}

	close(fd);
	exit(0);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}

void signalHandler(int sig) {
	printf("\n");
	close(fd);
	exit(0);
}

void checkServerRead(int nread){
	if(nread == 0){
		printf("Server is closed\n");
		close(fd);
		exit(0);
	}
}

void checkServerWrite(int sig){
	printf("Server is closed\n");
	close(fd);
	exit(0);
}
