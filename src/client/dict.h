#include <stdio.h>
#include <string.h>
#include <time.h>
#include "client.h"

typedef struct {
	char nome[50];
	char ip[50];
	int port;
	time_t expiritionDate;
} node;

typedef struct {
	char nome[50];
	char ip[50];
} nodeGrp;

void adicionar_dict(node *dict, char *nome, char *ip, int port);
void procurar_ip_port(node *dict, char* nome, char *info, int size);
void remover_dict(node *dict, char *ip, int port);

void adicionar_dict_grp(nodeGrp *dict, char *nome, char *ip);
void procurar_ip_grp(nodeGrp *dict, char *nome, char *ip);