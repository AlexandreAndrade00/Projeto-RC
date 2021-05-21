#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
	char nome[50];
	char ip[50];
	int port;
	time_t expiritionDate;
} node;

void adicionar_dict(node *dict, char *nome, char *ip, int port);
void procurar_ip_port(node *dict, char* nome, char *info, int size);
void remover_dict(node *dict, char *ip, int port);
