#include "server.h"

FILE *fptr;
int fdConfig, fdClient, admin, client;

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
		clientes(argv[1], argv[3]);
		exit(0);
	}
	
	while(wait(NULL)>0);

}

void clientes(char *port, char *file) {
	char buffer[BUFFSIZE], *string, *found, info[7][BUFFSIZE], fileLine[7][128], auth[100][32];
	int porto = (int) strtol(port, (char **) NULL, 10), indexAuth=0, count;
	struct sockaddr_in clientAddr, newClientAddr;
	socklen_t slen = sizeof(newClientAddr);
	bool exist=false;
	
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //endereco do server
	clientAddr.sin_port = htons(porto);					  //porto onde recebe comandos config
	
	if((fdClient=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
		erro("na criação do socket client");
	
	if(bind(fdClient,(struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) 
		erro("no bind client");
		
	printf("Server for clients open on %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	
	while (1) {
	    if(recvfrom(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, (socklen_t *)&slen) < 0) {
	 		erro("no recvfrom");
		}
		
		string = strdup(buffer);				//partir string em substrings
		
		count=0;
		while((found = strsep(&string, " ")) != NULL) {	//criar substrings
            strcpy(info[count], found);
            count++;
        }
		//TODO resto de funcionalidades
		if (strcmp(info[0], "AUTH")==0) {
			fptr=fopen(file, "r");
			
			while(fgets(buffer, sizeof(buffer), fptr) != NULL) {
				string = strdup(buffer);
        		
        		count=0;
				while((found = strsep(&string, " ")) != NULL) {	//criar substrings
            		strcpy(fileLine[count], found);
            		count++;
        		}
        		
        		if (strcmp(fileLine[0], info[1])==0) {
        			exist=true;
        			//TODO verifcacao que ja fez login
        			if (strcmp(fileLine[2], info[2])==0) {
        				strcpy(auth[indexAuth], info[1]);
        				indexAuth++;
        				fclose(fptr);
        				snprintf(buffer, 1024, "Autenticado com sucesso!\nPermissoes: cliente-servidor: %s, P2P: %s, grupo: %s", fileLine[3], fileLine[4], fileLine[5]);
        				sendto(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, slen);
        				break;
        			} else {
        				strcpy(buffer, "Password errada!\n");
        				sendto(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, slen);
        				break;
        			}
        		}
        	}
        	if (exist==false) {
        		strcpy(buffer, "Nao esta registado!\n");
        			sendto(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, slen);
        	} else
        		exist=false;
		}
	}
}

void config(char *port, char *file) {
	int adminAddrSize, porto = (int) strtol(port, (char **) NULL, 10);
	struct sockaddr_in configAddr, adminAddr;
	
	configAddr.sin_family = AF_INET;
	configAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //endereco do server
	configAddr.sin_port = htons(porto);					  //porto onde recebe comandos config

	//abrir socket, bind ao porto, ficar a espera de conexoes
	if ((fdConfig = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	  	erro("na criação do socket config");

	if (bind(fdConfig, (struct sockaddr *)&configAddr, sizeof(configAddr)) < 0)
		erro("no bind config");

	if (listen(fdConfig, 5) < 0) 
	  	erro("na funcao listen");
	  	
	adminAddrSize = sizeof(adminAddr);

	printf("Server for config open on %s:%d\n", inet_ntoa(configAddr.sin_addr), ntohs(configAddr.sin_port));
	
	while (1) {
	    // clean finished child processes, avoiding zombies
	    // must use WNOHANG or would block whenever a child process was working
	    while (waitpid(-1, NULL, WNOHANG) > 0);
	    // wait for new connection
	    admin = accept(fdConfig, (struct sockaddr *)&adminAddr, (socklen_t *)&adminAddrSize);
	    if (admin > 0) {
	    	//cada conexao vai ser handled por um processo distinto
    		if (fork() == 0) {
    		    close(fdConfig);
    		    processAdmin(admin, adminAddr, file);
    		    exit(0);
    		}
      		close(admin);
    	}
	}
}

void processAdmin(int admin, struct sockaddr_in adminAddr, char *file) {
	char buffer[BUFFSIZE], *string, *found, info[7][BUFFSIZE];
	int count;
	
	while(1) {
		read(admin, buffer, BUFFSIZE);	//esperar mensagens do client
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
        		
        		if (strcmp(found, user)!=0)			//copiar tudo menos utilizador a eliminar
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
	close(fdConfig);
	char aux = 1;
	setsockopt(fdConfig, SOL_SOCKET, SO_REUSEADDR,&aux, sizeof(aux));
	exit(0);
}
