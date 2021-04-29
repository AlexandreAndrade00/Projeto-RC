#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	char nome[50];
	char ip[50];
	int port;
	bool server;
	bool p2p;
	bool grupo;
} node;

void adicionar_dict(node *dict, char *nome, char *ip, int port, char *server, char *p2p, char *grupo);
int procurar_port(node *dict, char *ip);
char* procurar_name(node *dict, char *ip, int port);
void remover_dict(node *dict, char *ip, int port);
