/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surgem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVER_PORT 9000
#define BUF_SIZE 1024


void process_client(int client_fd, int counter, struct sockaddr_in client_info);
void erro(char *msg);
void signalHandler(int sig);
void checkClient(int nread);

int fd_aux;

int main() {
  int fd, client, aux = 0;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;

  signal(SIGINT, signalHandler);

  bzero((void *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(SERVER_PORT);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  	erro("na funcao socket");

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    erro("na funcao bind");

  if (listen(fd, 5) < 0) 
  	erro("na funcao listen");

  client_addr_size = sizeof(client_addr);
  printf("Receving messages from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  while (1) {
    // clean finished child processes, avoiding zombies
    // must use WNOHANG or would block whenever a child process was working
    while (waitpid(-1, NULL, WNOHANG) > 0);
    // wait for new connection
    client = accept(fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
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
  return 0;
}

void process_client(int client_fd, int counter, struct sockaddr_in client_info) {
  int nread = 0, aux, i, numbers[10];
  char buffer[BUF_SIZE], message[100];
  fd_aux = client_fd;

  while (1) {
    nread = read(client_fd, buffer, BUF_SIZE - 1);
    checkClient(nread);
    buffer[nread] = '\0';
    printf("Client %d, connected from %s:%d says: %s\n", counter, inet_ntoa(client_info.sin_addr), client_info.sin_port, buffer);

    if (strcmp(buffer, "DADOS") == 0) {
      printf("Reciving numbers\n");
      for (i = 0; i < 10; i++) {
        nread = read(client_fd, buffer, BUF_SIZE - 1);
        checkClient(nread);
        numbers[i] = (int)strtol(buffer, (char **)NULL, 10);
        printf("%d ", numbers[i]);
        fflush(stdout);
      }
      printf("\nNumbers received successfully\n");
    } else if (strcmp(buffer, "SOMA") == 0) {
      aux = 0;
      for (i = 0; i < 10; i++) {
        aux = aux + numbers[i];
      }
      snprintf(message, 100, "Soma=%d\n", aux);
      write(client_fd, message, 1 + strlen(message));
    } else if (strcmp(buffer, "MEDIA") == 0) {
      aux = 1;
      for (i = 0; i < 10; i++) {
        aux += numbers[i];
      }
      float med = aux / 10;
      snprintf(message, 100, "Media=%0.2f\n", med);
      write(client_fd, message, 1 + strlen(message));
    } else if (strcmp(buffer, "SAIR") == 0) {
      printf("Client %d left\n", counter);
      break;
    } else
      erro("mensagem nao conhecida");

    printf("\n");
  }
  close(client_fd);
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
  exit(-1);
}

void signalHandler(int sig) {
    printf("Closing sockets...\n");
		exit(0);
}

void checkClient(int nread){
  if(nread == 0){
    printf("\nClient disconnected\n");
		close(fd_aux);
		exit(0);
	}
}
