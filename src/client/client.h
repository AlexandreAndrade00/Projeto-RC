#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <arpa/inet.h>

#define BUFFSIZE 1024   // Tamanho do buffer
#define MAX_USERS_SAVED 100
#define MAX_GROUPS 10

void erro(char *msg);
void *receiveMsg(void *arguments);
void *receiveMsgGrp(void *arguments);
void userInteration();
void getMyName(char *aux);
void handleGrupo(char *input);