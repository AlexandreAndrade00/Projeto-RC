#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFSIZE 1024

void erro(char *msg);
void signalHandler(int sig);

int fd;

int main(int argc, char *argv[]) {	
    char endServer[100], buffer[BUFFSIZE], *string, *found;
    struct sockaddr_in addr;
    struct hostent *hostPtr;

    signal(SIGINT, signalHandler);

    if (argc != 3) {
        printf("client <host> <port>\n");
        exit(-1);
    }

    strcpy(endServer, argv[1]);
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Não consegui obter endereço");
        
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr.sin_port = htons((int) strtol(argv[2], (char **) NULL, 10));

    //abrir socket e realizar conexao
    if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
        erro("socket");
    if(connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
        erro("Connect");
        
    while(1) {
        //send messsage to server
        printf("Instruction: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        write(fd, buffer, sizeof(buffer));
        string = strdup(buffer);
        found = strsep(&string, " ");

        if(strcmp(found, "LIST")==0) {
            while(1) {
                read(fd, buffer, BUFFSIZE);
                if(strcmp(buffer, "end")==0) {
                    printf("\n");
                    break;
                }
                else
                    printf("%s", buffer);
            }
        } else if(strcmp(found, "ADD")==0 || strcmp(found, "DEL")==0) {
            read(fd, buffer, BUFFSIZE);
            printf("%s\n", buffer);
        } else if(strcmp(found, "QUIT")==0)
            break;
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
