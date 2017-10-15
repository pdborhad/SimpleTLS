#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <stdio.h>
#include <iostream>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <unistd.h>

#define FAIL -1

int startClient(char * portnum, char *hostname);
int connectoToSever(int portnum, char *hostname);
SSL_CTX* InitCTX_client();
void ShowCerts_client(SSL* ssl);
