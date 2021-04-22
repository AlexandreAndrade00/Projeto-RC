#include "server.h"

FILE *fptr;
int fd, admin;

int main(int argc, char *argv[]) {

	if (argc != 4) {
		printf("server <port_clients> <port_config> <filename>\n");
		exit(-1);
	}
	
	signal(SIGINT, signalHandler);
	
	if(fork()==0) {
		config(argv[2], argv[3]);
		exit(0);
	}
	
	if(fork()==0) {
		clientes();
		exit(0);
	}
	
	while(wait(NULL)>0);

}

void clientes() {
	//TODO
	//fazer parte de receber comandos dos clientes
	//modo server (reencaminhar mensagens)
	//modo p2p (devolver ip)
}

void config(char *port, char *file) {
	int adminAddrSize, porto = (int) strtol(port, (char **) NULL, 10);
	struct sockaddr_in configAddr, adminAddr;
	
	configAddr.sin_family = AF_INET;
	configAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //endereco do server
	configAddr.sin_port = htons(porto);					  //porto onde recebe comandos config

	//abrir socket, bind ao porto, ficar a espera de conexoes
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	  	erro("na funcao socket");

	if (bind(fd, (struct sockaddr *)&configAddr, sizeof(configAddr)) < 0)
		erro("na funcao bind");

	if (listen(fd, 5) < 0) 
	  	erro("na funcao listen");
	  	
	adminAddrSize = sizeof(adminAddr);

	printf("Server for config open on %s:%d\n", inet_ntoa(configAddr.sin_addr), ntohs(configAddr.sin_port));
	
	while (1) {
	    // clean finished child processes, avoiding zombies
	    // must use WNOHANG or would block whenever a child process was working
	    while (waitpid(-1, NULL, WNOHANG) > 0);
	    // wait for new connection
	    admin = accept(fd, (struct sockaddr *)&adminAddr, (socklen_t *)&adminAddrSize);
	    if (admin > 0) {
	    	//cada conexao vai ser handled por um processo distinto
    		if (fork() == 0) {
    		    close(fd);
    		    processAdmin(admin, adminAddr, file);
    		    exit(0);
    		}
      		close(admin);
    	}
	}
}

void processAdmin(int admin, struct sockaddr_in adminAddr, char *file) {
	char buffer[BUFFSIZE], *string, *found, info[7][BUFFSIZE];
	int nread, count;
	
	while(1) {
		nread = read(admin, buffer, BUFFSIZE);	//esperar mensagens do client
		//printf("%s\n", buffer);				//print para debug
		string = strdup(buffer);				//partir string em substrings
		
		count=0;
		while((found = strsep(&string, " ")) != NULL) {	//criar substrings
            strcpy(info[count], found);
            count++;
        }
        
        if(strcmp(info[0], "LIST")==0) {
        	//TODO dar print a nome das colunas das tabelas
        	fptr = fopen(file, "r");
        	
        	while(fgets(buffer, sizeof(buffer), fptr) != NULL)
        		write(admin, buffer, sizeof(buffer));			//output users
        		
        	strcpy(buffer, "end");								//indicar que acabou lista
        	write(admin, buffer, sizeof(buffer));
        	fclose(fptr);
        	
        } else if (strcmp(info[0], "ADD")==0) {
        	//TODO verificacao de dados de input
        	buffer[strlen(buffer)]='\n';						//acrescentar \n
        	fptr = fopen(file, "a");
        	fprintf(fptr, "%s %s %s %s %s %s\n", info[1], info[2], info[3], info[4], info[5], info[6]);
        	fclose(fptr);
        	strcpy(buffer, "Utilizador adicionado com sucesso!\n");
        	write(admin, buffer, sizeof(buffer));
        	
        } else if (strcmp(info[0], "DEL")==0) {
        	char user[BUFFSIZE];
        	strcpy(user, info[1]);
        	FILE *tempfptr;
        	fptr = fopen(file, "r");
        	tempfptr = fopen("temp.txt", "w");			//criacao ficheiro temporario
        	
        	while(fgets(buffer, sizeof(buffer), fptr) != NULL) {
        		string = strdup(buffer);
        		found = strsep(&string, " ");			//ir buscar primeira substring 
        		
        		if (strcmp(found, user)!=0)				//copiar tudo menos utilizador a eliminar
        			fputs(buffer, tempfptr);
        	}
        	fclose(fptr);
        	fclose(tempfptr);
        	remove(file);								//apagar ficheiro antigo
        	rename("temp.txt", file);					//rename ficheiro temporario
        	strcpy(buffer, "Utilizador removido com sucesso!\n");
        	write(admin, buffer, sizeof(buffer));
        	
        } else if (strcmp(info[0], "QUIT")==0) {
        	close(admin);
        	break;
        	
        } else {
        	strcpy(buffer, "Comando nao conhecido...\n");
        	write(admin, buffer, sizeof(buffer));
        }
	}
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
  exit(-1);
}

void signalHandler(int sig) {
	close(admin);
	close(fd);
	char aux = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&aux, sizeof(aux));
	exit(0);
}

