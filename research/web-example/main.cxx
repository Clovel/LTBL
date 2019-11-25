/** 
 * Let There Be Light project
 * 
 * @file main.cxx
 * @brief This file is a test for running a minimalist
 * web page to change a variable in a C/C++ program.
 */

/* Includes -------------------------------------------- */
#include "remoteRequest.hxx"

#include "Relay.hxx"

/* C++ System */
#include <iostream>

/* C Networking */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <pthread.h>

#include <unistd.h> /* close(int fd) */
#include <fcntl.h>  /* To set client socket to O_NONBLOCK */

#include <errno.h> /* For errno */
#include <cstring> /* For strerror */

/* Defines --------------------------------------------- */
#define MAX_REQUEST_NB 5U

/* Variable declarations ------------------------------- */
std::string sProgName;
static int sVar = 0;
elec::Relay sRelay(elec::RELAY_MODE_NORMAL);

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
        std::cerr << "[ERROR] Wrong number of arguments !" << std::endl;
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
    //pthread_t lThread;

    /* Initializing error variable */
    int lResult = 0;

    /* Creating the TCP socket */
    errno = 0;
    if(0 > (lServerSocket = socket(AF_INET, SOCK_STREAM, 0))) {
        std::cerr << "[ERROR] Failed to create the socket (lServerSocket = " << lServerSocket << ") !" << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << " : " << strerror(errno) << std::endl;
        }
        exit(EXIT_FAILURE);
    }

    /* Setting the socket options */
    errno = 0;
    int lEnableOpt = 1;
    if (0 > setsockopt(lServerSocket, SOL_SOCKET, SO_REUSEADDR & SO_REUSEPORT, &lEnableOpt, sizeof(int))) {
        std::cout << "[ERROR] setsockopt failed to set server socket options !" << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << " : " << strerror(errno) << std::endl;
        }
        exit(EXIT_FAILURE);
    }

    /* Bind socket */
    errno = 0;
    lResult = bind(lServerSocket, (struct sockaddr *)&lServerSocketName, sizeof(lServerSocketName));
    if(-1 == lResult) {
        std::cerr << "[ERROR] Failed to bind the server socket ! (" << lResult << ")" << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << " : " << strerror(errno) << std::endl;
        }
        exit(EXIT_FAILURE);
    } else {
        lResult = 0;
    }

    /* Initializing the request queue */
    errno = 0;
    lResult = listen(lServerSocket, MAX_REQUEST_NB);
    if(-1 == lResult) {
        std::cerr << "[ERROR] listen on server socket failed with error code " << lResult << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << " : " << strerror(errno) << std::endl;
        }
    } else {
        lResult = 0;
    }

    /* Voiding variables to fix warnings */
    /* TODO : Fix this correctly */
    (void)lServerHostPtr;
    (void)lServerSocketNameLen;
    (void)lClientHostPtr;

    /* Enter main loop */
    std::cout << "[INFO ] Server is listening..." << std::endl;
    int lError = 0;
    while(true) {
        /* Get client socket */
        errno = 0;
        lClientSocket = accept(lServerSocket, (struct sockaddr *)&lClientSocketName, &lClientSocketNameLen);
        if(-1 == lClientSocket) {
            std::cerr << "[ERROR] Failed to \"accept\" client socket" << std::endl;
            if(errno) {
                std::cerr << "        errno = " << errno << " : " << strerror(errno) << std::endl;
            }
            exit(EXIT_FAILURE);
        }

        /* Test if the socket is in non-blocking mode */
        if(fcntl(lClientSocket, F_GETFL) & O_NONBLOCK) {
            /* socket is non-blocking */
        }

        /* Put the socket in non-blocking mode */
        if(fcntl(lClientSocket, F_SETFL, fcntl(lClientSocket, F_GETFL) | O_NONBLOCK) < 0) {
            /* handle error */
        }

        /* Sleep a while to give the browser some time to process */
        usleep(10000);

        /* Process the received request */
        lError = acceptRequest(lClientSocket, &sVar);
        if(0 != lError) {
            std::cerr << "[ERROR] Failed to process remote request w/ acceptRequest !" << std::endl;
        } else {
            std::cout << "[DEBUG] Processed acceptRequest successfully !" << std::endl;
        }

        std::cout << std::endl;
    }

    /* Close the socket */
    close(lServerSocket);

    std::cout << "[INFO ] Server ending. Bye..." << std::endl;

    return EXIT_SUCCESS;
}