#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 500

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	if (argc < 3){
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)){
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1){
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))){
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	char buf[BUFSZ];
	unsigned total = 0;
	while (1){
		memset(buf, 0, BUFSZ);
		fgets(buf, BUFSZ - 1, stdin);

		size_t count = send(s, buf, strlen(buf) + 1, 0);
		if (count != strlen(buf) + 1){
			logexit("send");
		}

		memset(buf, 0, BUFSZ);
		count = recv(s, buf + total, BUFSZ - total, 0);
		if (strlen(buf) > 1){
			puts(buf);
		}

		if (count < 2){
			break;
		}
	}
	close(s);

	exit(EXIT_SUCCESS);
}