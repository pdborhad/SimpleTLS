#include "server.h"
#define FAIL    -1

int OpenListener(int port)
{
    int sd;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("   Can't configure listening port");
        exit(EXIT_FAILURE);
    }
    else{
        printf("   Listening ...");
    }
    return sd;
}

SSL_CTX* InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}


void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    //printf("Inside LoadCertificates \n ");
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        //printf("Checking Cirt \n ");
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        //printf("Checking KeyFile \n ");
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        //printf("SSL_CTX_check_private_key \n ");
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
    //printf("Ending LoadCertificates \n ");
}

void ShowCerts_server(SSL* ssl)
{   X509 *cert;
    char *line;
     //printf("Inside ShowCerts_server \n ");
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("  Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("  Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("  Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("   Info: No Client certificates configured.\n");
}

void Servlet_server(SSL* ssl) /* Serve the connection -- threadable */
{
    char buf[1024];
    char reply[1024];
    int sd, bytes;
    const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";
    //const char* reply = "Test";

    if ( SSL_accept(ssl) == FAIL )     /* do SSL-protocol accept */
    {
        printf("   Executing Servlet_server...");
        ERR_print_errors_fp(stderr);
    }
    else
    {
        ShowCerts_server(ssl);        /* get any certificates */
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
        if ( bytes > 0 )
        {
            buf[bytes] = 0;
            printf("   Client msg: \"%s\"\n", buf);
            sprintf(reply, HTMLecho, buf);   /* construct reply */
            SSL_write(ssl, reply, strlen(reply)); /* send reply */

        }
        else
            ERR_print_errors_fp(stderr);
    }
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
    printf("    !!!-------!!! \n");
    printf("\n\n\n");
}


int startServer(char *portnum){
    printf("   Starting the Server...\n");
    SSL_CTX *ctx;
    int server;
    SSL_library_init();
    // Initialize SSL
    ctx = InitServerCTX();

    LoadCertificates(ctx, (char *)"/home/prashant/workspace/codeBlocks/SimpleTLS/cirt/server.crt", (char *)"/home/prashant/workspace/codeBlocks/SimpleTLS/cirt/server.key"); /* load certs */
    printf("   Opening socket connection\n");
    int port = atoi(portnum);
    server = OpenListener(port);    /* create server socket */

    while(1){
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection From: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        ssl = SSL_new(ctx);              /* get new SSL state with context */

        SSL_set_fd(ssl, client);      /* set connection socket to SSL state */
        Servlet_server(ssl);         /* service connection */

    }



    return 0;
}
