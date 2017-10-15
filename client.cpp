#include "client.h"




int startClient(char *portnum, char *hostname){
    printf("   Starting Client\n");
    SSL_CTX *ctx;
    SSL *ssl;
    int server;

    char buf[1024];
    int bytes;
    printf("   Initializing SSL Lib\n");
   // LoadCertificates(ctx, (char *)"/home/prashant/workspace/codeBlocks/SimpleTLS/cirt/server.crt", (char *)"/home/prashant/workspace/codeBlocks/SimpleTLS/cirt/server.key");
    SSL_library_init();
    ctx = InitCTX_client();
    int port = atoi(portnum);
    while(1){
        if((server = connectoToSever(port, hostname))){
            //printf("Connection Successful");
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, server);
            if(SSL_connect(ssl) == FAIL){
                ERR_print_errors_fp(stderr);
            }
            else{
                char *msg {(char *)"Hello ! I am Client..."};
                ShowCerts_client(ssl);
                printf("   Sending data to server ... \n");
                SSL_write(ssl, msg, strlen(msg));
                bytes = SSL_read(ssl, buf, sizeof(buf));
                buf[bytes] = 0;
                printf("   Received: \"%s\"", buf);
                printf("\n");
                SSL_free(ssl);
            }
        close(server);
        }
        printf("     !!!------!!!!\n");
        printf("\n\n");
        usleep(10000000);
    }

    return 0;
}

int connectoToSever(int portnum, char *hostname){
    int s;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portnum);
    //addr.sin_addr.s_addr = htonl(host);
    addr.sin_addr.s_addr = inet_addr(hostname);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("   Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(s);
        perror("   Unable to bind");
        exit(EXIT_FAILURE);
    }
    // printf("ending connectoToSever\n");

    return s;

}

SSL_CTX* InitCTX_client(void){
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();
    method = TLSv1_2_client_method();
    ctx = SSL_CTX_new(method);
    if(ctx == NULL){
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts_client(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */

    //printf("\n cert in client is ", cert);

    if ( cert != NULL )
    {
        printf("   Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("   Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("   Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("   Info: No Server certificates configured.\n");
}
