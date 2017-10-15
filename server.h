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


int startServer(char *portnum);
SSL_CTX* InitServerCTX(void);
int OpenListener(int port);
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);
void ShowCerts_server(SSL* ssl);
void Servlet_server(SSL* ssl);

