#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <search.h>

#define BUFFSIZE 1024
#define ADDRESS "10.90.0.1"

void config();
void processAdmin(int admin, struct sockaddr_in adminAddr, char *file);
void erro(char *msg);
void signalHandler(int sig);
void clientes();
