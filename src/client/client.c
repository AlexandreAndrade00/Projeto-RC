#include "client.h"
#include "dict.h"

int fd, fd1;
struct in_addr localInterface;
struct sockaddr_in addr, addrP2P, addrReceive, addrGroup;
struct ip_mreq group;
char buffer[BUFFSIZE];
pthread_t receive, receiveGrp;
socklen_t slen = sizeof(addr);
char whoIam[512];
nodeGrp *dictGrp;
node *dict;
sem_t mutex;
bool waitingServer=false;

void handleP2P();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("cliente <server host> <port>\n");
        exit(-1);
    }

    sem_init(&mutex, 0, 0);
    
    int porto = (int) strtol(argv[2], (char **) NULL, 10);
    
    struct hostent *hostPtr;
    char endServer[100];

    strcpy(endServer, argv[1]);
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Não consegui obter endereço");

    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr.sin_port = htons(porto);

    addrGroup.sin_family = AF_INET;
    addrGroup.sin_addr.s_addr  = htonl(INADDR_ANY);
    addrGroup.sin_port = htons(9005);

    if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
        erro("socket");

    if ((fd1 = socket(AF_INET,SOCK_DGRAM,0)) == -1)
        erro("socket");

    int multicastTTL = 255;
    if (setsockopt(fd1, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &multicastTTL, sizeof(multicastTTL)) < 0) {
        perror("socket opt");
        exit(-1);
    }

    int reuse=1;
    if (setsockopt(fd1, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
        perror("setting SO_REUSEADDR");
        close(fd1);
        exit(1);
    }

    if (bind(fd1, (struct sockaddr*)&addrGroup, sizeof(addrGroup))) {
        perror("binding datagram socket");
        close(fd1);
        exit(1);
    }
        
    printf("----------------------COMANDOS--------------------\n\n");
    printf("AUTH - AUTENTICAR NO SERVER\n");
    printf("SEND - MANDAR MENSAGEM PARA UM UTILIZADOR ATRAVES DO SERVER\n");
    printf("SP2P - MANDAR MENSAGEM PARA UTILIZADOR ATRAVES DE P2P\n");
    printf("CGRUPO - CRIAR GRUPO\n");
    printf("JGRUPO - JUNTAR-SE A GRUPO\n");
    printf("SGRUPO - ENVIAR MENSAGEM PARA GRUPO\n");
    printf("QUIT - DESCONECTAR DO SERVIDOR E TERMINAR PROGRAMA\n\n\n");
            
    userInteration();
    
    close(fd);
    return 0;
}

void userInteration() {
    char *string, *found, aux[BUFFSIZE], info[10][512];
    dict = malloc(sizeof(node)*MAX_USERS_SAVED);
    dictGrp = malloc(sizeof(nodeGrp)*MAX_GROUPS);
    int count=0;

    while(1) {
        printf("Insira dados de autenticacao (AUTH <username> <password>):\n");
        fgets(buffer, sizeof(buffer), stdin);
        string = strdup(buffer);
        found = strsep(&string, " ");
        if (strcmp(found, "AUTH")==0)
            break;
    }

    buffer[strcspn(buffer, "\n")] = 0;
    sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);
    strncpy(aux, buffer, strlen(buffer)+1);
    recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *) &slen);

    if (buffer[0]=='A') {
        getMyName(aux);
        printf("%s\n", buffer);
        pthread_create(&receive, NULL, receiveMsg, NULL);
        pthread_create(&receiveGrp, NULL, receiveMsgGrp, NULL);
    
        while(1) {
            //send messsage to server
            fgets(buffer, sizeof(buffer), stdin);
            
            buffer[strcspn(buffer, "\n")] = 0;
            count=0;
            string=strdup(buffer);
            while((found = strsep(&string, " ")) != NULL) { //criar substrings
                strcpy(info[count], found);
                count++;
            }

            if(strcmp(info[0], "QUIT")==0) {
                sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);
                pthread_cancel(receive);
                pthread_cancel(receiveGrp);
                sem_destroy(&mutex);
                break;
            } else if (strcmp(info[0], "SEND")==0) {
                if (count!=3)
                    printf("SEND <user> <message>\n");
                else
                    sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);
            } else if (strcmp(info[0], "SP2P")==0) {
                if (count!=3)
                    printf("SP2P <user> <message>\n");
                else
                    handleP2P();
            } else if (strcmp(info[0], "CGRUPO")==0) {
                if (count<3)
                    printf("CGRUPO <nome_grupo> <participante1> <participante2>...\n");
                else
                    sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);
            } else if (strcmp(info[0], "SGRUPO")==0) {
                if (count!=3)
                    printf("SGRUPO <group_name> <message>\n");
                else
                    handleGrupo();
            } else if (strcmp(info[0], "JGRUPO")==0) {
                if (count!=2)
                    printf("JGRUPO <group_name>\n");
                else {
                    sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);

                    waitingServer=true;
                    sem_wait(&mutex);
                    waitingServer=false;

                    strcpy(aux, info[1]);
                    string=strdup(buffer);
                    count=0;
                    while((found = strsep(&string, " ")) != NULL) { //criar substrings
                        strcpy(info[count], found);
                        count++;
                    }

                    if (strcmp(info[0], "REQUESTED")==0) {
                        group.imr_multiaddr.s_addr = inet_addr(info[1]);
                        if (setsockopt(fd1, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0) {
                            perror("adding multicast group");
                            close(fd1);
                            exit(1);
                        }
                        adicionar_dict_grp(dictGrp, aux, info[1]);
                    } else
                        printf("Algo aconteceu...\n");
                }
            } else
                printf("Comando desconhecido!\n");

            free(string);
        }
        pthread_join(receive, NULL);
        pthread_join(receiveGrp, NULL);
    } else
        printf("%s\n", buffer);
    
    free(dict);
}

void *receiveMsg(void *arguments) {
    char *string, *found;
    while(1) {
        recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr *) &addrReceive, (socklen_t *) &slen);
        if (waitingServer==true)
            sem_post(&mutex);
        string=strdup(buffer);
        found = strsep(&string, " ");
       
        if (strcmp(found, "REQUESTED")!=0)       
            printf("%s\n", buffer);
    }
}

void *receiveMsgGrp(void *arguments) {
    while(1) {
        recvfrom(fd1, buffer, BUFFSIZE, 0, (struct sockaddr *) &addrReceive, (socklen_t *) &slen);
        printf("%s\n", buffer);
    }
}

void handleP2P() {
    int temp, count;
    char *found, *string, info[3][500], aux1[500], ipPort[2][50];
    string = strdup(buffer);
    count=0;
    while((found = strsep(&string, " ")) != NULL) { //criar substrings
        strcpy(info[count], found);
        count++;
    }
    procurar_ip_port(dict, info[1], aux1, sizeof(aux1));
    
    if (aux1[0] == '\0') {
        snprintf(buffer, BUFFSIZE, "REQUEST %s", info[1]);
        sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addr, slen);

        waitingServer=true;
        sem_wait(&mutex);
        waitingServer=false;

        string = strdup(buffer);
        
        found = strsep(&string, " ");
        if (strcmp(found, "REQUESTED")==0) {
            count=0;
            while((found = strsep(&string, ":")) != NULL) { //criar substrings
                strcpy(ipPort[count], found);
                count++;
            }
            temp = (int) strtol(ipPort[1], (char **) NULL, 10);
           
            adicionar_dict(dict, info[1], ipPort[0], temp);
            addrP2P.sin_addr.s_addr = inet_addr(ipPort[0]);
            addrP2P.sin_port=htons(temp);
            snprintf(buffer, BUFFSIZE, "%s SENT %s", whoIam, info[2]);
            sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addrP2P, slen);
        } else
            printf("Algo aconteceu...\n");
    } else {
        string = strdup(aux1);
        count=0;
        while((found = strsep(&string, ":")) != NULL) { //criar substrings
            strcpy(ipPort[count], found);
            count++;
        }
        temp = (int) strtol(ipPort[1], (char **) NULL, 10);
        addrP2P.sin_addr.s_addr = inet_addr(ipPort[0]);
        addrP2P.sin_port=htons(temp);
        snprintf(buffer, BUFFSIZE, "%s SENT %s", whoIam, info[2]);
        sendto(fd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &addrP2P, slen);
    }
}


void handleGrupo() {
    int count;
    char *found, *string, info[3][128], aux[128];
    string = strdup(buffer);
    count=0;
    while((found = strsep(&string, " ")) != NULL) { //criar substrings
        strcpy(info[count], found);
        count++;
    }

    procurar_ip_grp(dictGrp, info[1], aux);
    addrGroup.sin_addr.s_addr = inet_addr(aux);

    sprintf(buffer, "%s SENT to %s: %s", whoIam, info[1], info[2]);

    if (sendto(fd1, buffer, strlen(buffer)+1, 0, (struct sockaddr*)&addrGroup, sizeof(addrGroup)) < 0) {
        perror("sending datagram message");
    }

}


void erro(char *msg) {
    printf("Erro: %s\n", msg);
    exit(-1);
}

void getMyName(char *aux) {
    int count=0;
    char *found, *string, info[3][512];
    string = strdup(aux);
    while((found = strsep(&string, " ")) != NULL) { //criar substrings
        strncpy(info[count], found, strlen(found)+1);
        count++;
    }
    strncpy(whoIam, info[1], strlen(info[1])+1);
    free(string);
}
