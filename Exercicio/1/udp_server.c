#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#define BUFLEN 1024	// Tamanho do buffer
#define PORT 9876	// Porto para recepção das mensagens

void signalHandler(int sig);
void erro(char *s);

int s;

int main(void) {
	struct sockaddr_in si_minha, si_outra;
	int recv_len;
	socklen_t slen = sizeof(si_outra);
	char buf[BUFLEN];
	int send_len;
	
	signal(SIGINT, signalHandler);

	// Cria um socket para recepção de pacotes UDP
	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		erro("Erro na criação do socket");
	}

  // Preenchimento da socket address structure
	si_minha.sin_family = AF_INET;
	si_minha.sin_port = htons(PORT);
	si_minha.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associa o socket à informação de endereço
	if(bind(s,(struct sockaddr*)&si_minha, sizeof(si_minha)) == -1) {
		erro("Erro no bind");
	}

	while(1) {
		buf[0] = '\0';
		// Espera recepção de mensagem (a chamada é bloqueante)
		if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_outra, (socklen_t *)&slen)) == -1) {
	 		erro("Erro no recvfrom");
		}
		printf("Recebi uma mensagem do sistema com o endereço %s e o porto %d\n", inet_ntoa(si_outra.sin_addr), ntohs(si_outra.sin_port));
		printf("Conteúdo da mensagem: %s\n" , buf);
		
		// Para ignorar o restante conteúdo (anterior do buffer)
		buf[recv_len]='\0';
		sprintf(buf, "%ld", strlen(buf));
		if((send_len = sendto(s, buf, 1024, 0, (struct sockaddr *) &si_outra, sizeof(si_outra))) == -1) {
			erro("sendto");
		}
	}

	// Fecha socket e termina programa
	close(s);
	return 0;
}

void signalHandler(int sig) {
	printf("\n");
	close(s);
	exit(0);
}

void erro(char *s) {
	perror(s);
	exit(1);
}
