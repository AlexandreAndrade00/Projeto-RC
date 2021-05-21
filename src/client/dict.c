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


void procurar_ip_port(node *dict, char* nome, char *info, int size) {
	int i=0;
	while(dict[i].port!=0) {
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
