#include "server.h"
#include "client.h"
#include <string>


using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Incorrect input parameters ... \n");
        exit(0);
    }
    cout << "Hello !" << endl;
    string startCon = "";
    cout << "DO you want to start Server or Client ? \n" << endl;

    char *hostname, *portnum;
    portnum = (char *)argv[1];
    hostname = (char *)argv[2];

    label:
        cin >> startCon;

        if(startCon == "Server"){
            startServer(portnum);
        }
        else if (startCon == "Client"){
            startClient(portnum, hostname);
       }
        else{
            printf("Please enter word 'Server' or 'Client' ... \n");
            goto label;
        }
    return 0;
}
