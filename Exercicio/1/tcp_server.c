/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surgem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SERVER_PORT 9003
#define BUF_SIZE 1024

typedef struct {
	int nums[10];
} sharedMemory;

void process_client(int client_fd, int counter, struct sockaddr_in client_info);
void erro(char *msg);

sharedMemory *sharedVar;
int shmid;
key_t key;

int main() {
  int fd, client;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;
  int aux = 0;
  
  shmid = shmget(key, sizeof(sharedMemory), IPC_CREAT|0700);
  sharedVar = (sharedMemory*) shmat(shmid, NULL, 0);

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(SERVER_PORT);
   

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);
  printf("Receving messages from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    if (client > 0) {
      aux++;
      if (fork() == 0) {
        close(fd);
        process_client(client, aux, client_addr);
        exit(0);
      }
    close(client);
    }
  }
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
}

void process_client(int client_fd, int counter, struct sockaddr_in client_info) {
	int nread = 0, aux, i;
	char buffer[BUF_SIZE], message[100];
	
	while(1) {
		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread] = '\0';
		printf("Client %d, connected from %s:%d says: %s\n", client_fd, inet_ntoa(client_info.sin_addr),client_info.sin_port,buffer);
	
		if(strcmp(buffer, "DADOS")==0) {
			printf("Reciving numbers\n");
			for (i=0; i<10; i++) {
				nread = read(client_fd, buffer, BUF_SIZE-1);
				sharedVar->nums[i] = (int) strtol(buffer, (char **) NULL, 10);
				printf("%d ",sharedVar->nums[i]);
				fflush(stdout);
			}
			printf("\nNumeres received successfully\n");
		} else if(strcmp(buffer, "SOMA")==0){
			aux=0;
			for(i=0; i<10; i++) {
				aux = aux + sharedVar->nums[i];
			}
			snprintf(message, 100, "Soma=%d\n", aux);
			write(client_fd, message, 1+strlen(message));
		} else if(strcmp(buffer, "MEDIA")==0){
			aux=1;
			for(i=0; i<10; i++) {
				aux += sharedVar->nums[i];
			}
			float med = aux/10;
			snprintf(message, 100, "Media=%0.2f\n", med);
			write(client_fd, message, 1+strlen(message));
		} else if(strcmp(buffer, "SAIR")==0) {
			printf("Client leaved\n");
			break;
		}
		else
			erro("mensagem nao conhecida");
			
		printf("\n");
	}
	close(client_fd);
}


void erro(char *msg){
	printf("Erro: %s\n", msg);
	exit(-1);
}
