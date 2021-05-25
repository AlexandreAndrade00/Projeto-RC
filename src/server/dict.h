#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "server.h"

typedef struct {
    char nome[50];
    char ip[50];
    int port;
    bool server;
    bool p2p;
    bool grupo;
} node;

typedef struct {
    char nome[50];
    char ip[50];
    char participantes[10][500];
} node_grupo;

void adicionar_dict(node *dict, char *nome, char *ip, int port, char *server, char *p2p, char *grupo);
int procurar_port(node *dict, char *ip);
char* procurar_name(node *dict, char *ip, int port);
void remover_dict(node *dict, char *ip, int port);
bool isAuthed(node *dict, char* ip, int port);
bool isAuthed_byName(node *dict, char *nome);
bool groupPermission(node *dict, char *ip, int port);
bool sendPermission(node *dict, char *ip, int port);
bool sp2pPermission(node *dict, char *ip, int port);

void criar_dict_grupo(node_grupo *dict, char *nome, char *ip);
void adicionar_dict_grupo(node_grupo *dict, char *nome_grupo, char* nome_participante);
char* procurar_grupo_ip(node_grupo *dict, char* nome_grupo);
bool belongsGroup(node_grupo *dict, char *nome_grupo, char *nome_participante);