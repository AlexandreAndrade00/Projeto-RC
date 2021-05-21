#include "dict.h"

//TODO TROCAR TODOS OS WHILES POR CICLOS FOR COM LIMITES DEFINIDOS NO HEADER FILE

void adicionar_dict(node *dict, char *nome, char *ip, int port, char *server, char *p2p, char *grupo) {
	int i=0;
	
	while (dict[i].port!=0 && dict[i].port!=-1) {
		i++;
	}
	strncpy(dict[i].nome, nome, strlen(nome)+1);
	strncpy(dict[i].ip, ip, strlen(ip)+1);
	dict[i].port=port;
	
	if (strcmp(server, "yes"))
		dict[i].server = true;
	else 
		dict[i].server = false;
		
	if (strcmp(p2p, "yes"))
		dict[i].p2p = true;
	else 
		dict[i].p2p = false;
		
	if (strcmp(grupo, "yes"))
		dict[i].grupo = true;
	else 
		dict[i].grupo = false;
}

void criar_dict_grupo(node_grupo *dict, char *nome, char *ip) {
	int i=0;

	while(dict[i].ip[0]!='\0')
		i++;

	strncpy(dict[i].nome, nome, strlen(nome)+1);
	strncpy(dict[i].ip, ip, strlen(ip)+1);
}

void adicionar_dict_grupo(node_grupo *dict, char *nome_grupo, char* nome_participante) {
	int i=0;
	int j=0;

	while(strcmp(dict[i].nome, nome_grupo)!=0)
		i++;

	while(dict[i].participantes[j][0]!='\0')
		j++;

	strncpy(dict[i].participantes[j], nome_participante, strlen(nome_participante)+1);
}

char* procurar_grupo_ip(node_grupo *dict, char* nome_grupo) {
	for (int i=0; i< NUM_MAX_GRUPOS; i++) {
		if (strcmp(dict[i].nome, nome_grupo)==0) {
			return dict[i].ip;
			break;
		}
	}
	return NULL;
}

int procurar_port(node *dict, char *ip) {
	int i=0;
	while(dict[i].port!=0) {
		if (strcmp(dict[i].ip, ip)==0)
			return dict[i].port;
		i++;
	}
	return 0;
}

char* procurar_name(node *dict, char *ip, int port) {
	int i=0;
	while(dict[i].port!=0) {
		if (strcmp(dict[i].ip, ip)==0 && dict[i].port==port)
			return dict[i].nome;
		i++;
	}
	return NULL;
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
