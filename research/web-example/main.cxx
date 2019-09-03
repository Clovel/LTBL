/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file main.cxx
 * @brief This file is a test for running a minimalist
 * web page to change a variable in a C/C++ program.
 */

/* Includes -------------------------------------------- */
#include "remoteRequest.hxx"

/* C++ System */
#include <iostream>

/* C Networking */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <pthread.h>

#include <unistd.h> /* cloe(int fd) */

/* Defines --------------------------------------------- */
#define MAX_REQUEST_NB 5U

/* Variable declarations ------------------------------- */
std::string sProgName;
static int sVar = 0;
std::string sWebStr = "HTTP/1.0 200 Ok\r\n"
"Constant_Type: text/html\r\n"
"\r\n"
"<html lang=\"en\">\r\n"
"    <head>\r\n"
"        <title>web-example</title>\r\n"
"    </head>\r\n"
"    <body>\r\n"
"        HHello there !\r\nGeneral Kenobi !\r\n"
"    </body>\r\n"
"</html>\r\n";

/* Support funtions ------------------------------------ */
void usage(void) {
    std::cout << "[USAGE] " << sProgName << " usage : " <<std ::endl;
    std::cout << "        " << sProgName << " <port>" << std::endl;
}

/* Main function --------------------------------------- */
int main(const int argc, const char * const * const argv) {
    std::cout << "[DEBUG] Hello there ! General Kenobi !" << std::endl << std::endl;

    sProgName = std::string(argv[0]);

    if(2 != argc) {
        std::cerr << "[ERROR] Wrong number of arguments !" << std::cerr;
        usage();
        exit(EXIT_FAILURE);
    }

    /* Get the port number specified in the argument */
    int lPort = std::atoi(argv[1]);

    /* Server socket definition */
    int lServerSocket = -1; /* Socket descriptor */
    struct sockaddr_in lServerSocketName; /* Socket internet structure */
    struct hostent *lServerHostPtr = nullptr; /* Server information */
    socklen_t lServerSocketNameLen = sizeof(lServerSocketName);

    /* Input socket definition */
    int lClientSocket = -1;
    struct sockaddr_in lClientSocketName; /* Socket internet structure */
    struct hostent *lClientHostPtr = nullptr; /* Client information */
    socklen_t lClientSocketNameLen = sizeof(lClientSocketName);

    /* Initializing the sockaddr structure w/ formatted information */
    lServerSocketName.sin_family      = AF_INET; /* Address family */
    lServerSocketName.sin_port        = htons(lPort); /* Port */
    lServerSocketName.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Declaring thread */
    pthread_t lThread;

    /* Creating the TCP socket */
    if(0 > (lServerSocket = socket(AF_INET, SOCK_STREAM, 0))) {
        std::cerr << "[ERROR] Failed to create the socket (lServerSocket = " << lServerSocket << ") !" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Bind socket */
    if(-1 == bind(lServerSocket, (struct sockaddr *)&lServerSocketName, sizeof(lServerSocketName))) {
        std::cerr << "[ERROR] Failed to bind the socket !" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Initializing the request queue */
    listen(lServerSocket, MAX_REQUEST_NB);

    /* Enter main loop */
    std::cout << "[INFO ] Server is listening..." << std::endl;
    int lError = 0;
    while(true) {
        /* Get client socket */
        lClientSocket = accept(lServerSocket, (struct sockaddr *)&lClientSocketName, &lClientSocketNameLen);

        if(-1 == lClientSocket) {
            std::cerr << "[ERROR] Failed to \"accept\" client socket" << std::endl;
            exit(EXIT_FAILURE);
        }

        lError = acceptRequest(lClientSocket, &sVar);
        if(0 != lError) {
            std::cerr << "[ERROR] Failed to process remote request w/ acceptRequest !" << std::endl;
        } else {
            std::cout << "[DEBUG] Processed acceptRequest successfully !" << std::endl;
        }
    }

    /* Printout the web page contents */
    std::cout << "[DEBUG] Web page contents : " << std::endl << sWebStr << std::endl;

    /* Close the socket */
    close(lServerSocket);

    std::cout << "[INFO ] Server ending. Bye..." << std::endl;

    return EXIT_SUCCESS;
}