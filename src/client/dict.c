#include "dict.h"

void adicionar_dict(node *dict, char *nome, char *ip, int port) {
	int i=0;
	
	while (dict[i].port!=0 && dict[i].port!=-1) {
		i++;
	}
	strncpy(dict[i].nome, nome, strlen(nome)+1);
	strncpy(dict[i].ip, ip, strlen(ip)+1);
	dict[i].port=port;
	dict[i].expiritionDate = time(NULL)+(5*60);
}

void adicionar_dict_grp(nodeGrp *dict, char *nome, char *ip) {
	int i=0;

	while (dict[i].nome[0]!='\0') {
		if (i==MAX_GROUPS) {
            printf("Numero maximo de grupos em que participa antigido!\n");
            return;
        }
		i++;
	}

	strncpy(dict[i].nome, nome, strlen(nome)+1);
	strncpy(dict[i].ip, ip, strlen(ip)+1);
}

void procurar_ip_grp(nodeGrp *dict, char *nome, char *ip) {
	int i=0;

	while (dict[i].nome[0]!='\0') {
		if (i==MAX_GROUPS) {
            printf("Grupo nao encontrado!\n");
            return;
        } else if (strcmp(dict[i].nome, nome)==0) {
        	strcpy(ip, dict[i].ip);
        	return;
        }
		i++;
	}
}

void procurar_ip_port(node *dict, char* nome, char *info, int size) {
	int i=0;
	while(dict[i++].port!=0) {
		if (strcmp(nome, dict[i].nome)==0) {
			if (time(NULL)<=dict[i].expiritionDate) {
				snprintf(info, size, "%s:%d", dict[i].ip, dict[i].port);
				return;
			} else {
				remover_dict(dict, dict[i].ip, dict[i].port);
				printf("Tempo expirado, a requisitar informacao ao servidor novamente\n");
				info[0]='\0';
				return;
			}
		}
	}
	info[0]='\0';
}

void remover_dict(node *dict, char *ip, int port) {
	int i=0;
	while(dict[i].port!=0) {
		if (strcmp(dict[i].ip, ip)==0 && dict[i].port==port) {
			dict[i].port=-1;
			break;
		}
		i++;
	}
}
