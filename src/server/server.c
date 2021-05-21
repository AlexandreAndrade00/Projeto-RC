#include "server.h"
#include "dict.h"

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
	node *dict;
	dict = malloc(sizeof(node)*100);
	node_grupo *dict_grupo;
	dict_grupo = malloc(sizeof(node_grupo)*100);
	char buffer[BUFFSIZE], *string, *found, info[12][128], fileLine[7][128];
	int porto = (int) strtol(port, (char **) NULL, 10), count, ip_grupo=1;
	struct sockaddr_in clientAddr, newClientAddr;
	char *newClient;
	socklen_t slen = sizeof(newClientAddr);
	bool exist=false;
	
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = inet_addr(ADDRESS);  //endereco do server
	clientAddr.sin_port = htons(porto);					  //porto onde recebe comandos config
	
	if((fdClient=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
		erro("na criação do socket client");
	
	if(bind(fdClient,(struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) 
		erro("no bind client");
		
	printf("Server for clients open on %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	
	
	while (1) {
		printf("A aguardar mensagem!\n");
	    if(recvfrom(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, (socklen_t *)&slen) < 0) {
	 		erro("no recvfrom");
		}
		
		string = strdup(buffer);				//partir string em substrings
		
		count=0;
		while((found = strsep(&string, " ")) != NULL) {	//criar substrings
            strcpy(info[count], found);
            count++;
        }
        printf("%s\n", info[0]);
		//TODO resto de funcionalidades
		//verificar se utilizador esta autenticado antes de realizar opearcoes sem ser AUTH
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
        				newClient = inet_ntoa(newClientAddr.sin_addr);
        				if (strcmp(fileLine[1], newClient)==0) {
        					adicionar_dict(dict, fileLine[0], newClient, ntohs(newClientAddr.sin_port), fileLine[3], fileLine[4], fileLine[5]);
        					snprintf(buffer, 1024, "Autenticado com sucesso!\nPermissoes: cliente-servidor: %s, P2P: %s, grupo: %s", fileLine[3], fileLine[4], fileLine[5]);
        					sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
        					printf("%s conectado -> %s:%d\n", fileLine[0], newClient, ntohs(newClientAddr.sin_port));
        					break;
        				} else {
        					strcpy(buffer, "Endereco nao corresponde ao guardado!\n");
        					sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
        					break;
        				}
        			} else {
        				strcpy(buffer, "Password errada!\n");
        				sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
        				break;
        			}
        		}
        	}
        	if (exist==false) {
        		strcpy(buffer, "Nao esta registado!\n");
        			sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
        	} else
        		exist=false;
        		
        	fclose(fptr);
        		
		} else if (strcmp(info[0], "SEND")==0) {
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
        			porto = procurar_port(dict, fileLine[1]);
        			if (porto==0) {
        				printf("Utilizador nao autenticado no servidor\n");
						break;
        			}
        			newClient = inet_ntoa(newClientAddr.sin_addr);
        			char *aux = procurar_name(dict, newClient, ntohs(newClientAddr.sin_port));
        			sprintf(buffer, "%s SENT %s", aux, info[2]);
        			newClientAddr.sin_addr.s_addr = inet_addr(fileLine[1]);
        			newClientAddr.sin_port=htons(porto);
        			printf("A redirecionar mensagem para %s:%d\n", fileLine[1], porto);
        			sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
					break;
        		}
        	}
        	if (exist==false) {
        		strcpy(buffer, "Nao esta registado!\n");
        			sendto(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, slen);
        	} else
        		exist=false;

        	fclose(fptr);
		} else if (strcmp(info[0], "REQUEST")==0) {

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
        			porto = procurar_port(dict, fileLine[1]);
        			if (porto==0) {
        				printf("Utilizador nao autenticado no servidor\n");
						break;
        			}
        			printf("A enviar ip e porto de utilizador requisitado!\n");
        			sprintf(buffer, "REQUESTED %s:%d", fileLine[1], porto);
        			sendto(fdClient, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &newClientAddr, slen);
					break;
        		}
        	}
        	if (exist==false) {
        		strcpy(buffer, "Nao esta registado!\n");
        			sendto(fdClient, buffer, BUFFSIZE, 0, (struct sockaddr *) &newClientAddr, slen);
        	} else
        		exist=false;

        	fclose(fptr);
		} else if (strcmp(info[0], "CGRUPO")==0) {
			char ip_aux[100];
			sprintf(ip_aux, "224.0.0.%d", ip_grupo);
			ip_grupo++;
			criar_dict_grupo(dict_grupo, info[1], ip_aux);
			for (int i=2; i<12; i++) {
				if(info[i][0]!='\0') {
					adicionar_dict_grupo(dict_grupo, info[1], info[i]);
				} else
					break;
			}
			sendto(fdClient, ip_aux, strlen(ip_aux)+1, 0, (struct sockaddr *) &newClientAddr, slen);

		} else if (strcmp(info[0], "IPGRUPO")==0) {
			char *aux = procurar_grupo_ip(dict_grupo, info[1]);
			sendto(fdClient, aux, strlen(aux)+1, 0, (struct sockaddr *) &newClientAddr, slen);
		} else if (strcmp(info[0], "QUIT")==0) {
			remover_dict(dict, inet_ntoa(newClientAddr.sin_addr), ntohs(newClientAddr.sin_port));
		}
	}
	free(dict);
	free(dict_grupo);
}

void config(char *port, char *file) {
	int adminAddrSize, porto = (int) strtol(port, (char **) NULL, 10);
	struct sockaddr_in configAddr, adminAddr;
	
	configAddr.sin_family = AF_INET;
	configAddr.sin_addr.s_addr = inet_addr(ADDRESS);  //endereco do server
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

