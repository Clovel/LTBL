/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file remoteRequest.cxx
 */

/* Includes -------------------------------------------- */
#include "remoteRequest.hxx"

#include "Relay.hxx"

/* C++ System */
#include <iostream>

/* C System */
#include <sys/socket.h>

#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>

#include <ctype.h>

#include <errno.h> /* For errno */
#include <cstring> /* For strerror */

/* Defines --------------------------------------------- */
#define IS_SPACE(x) isspace((int)(x))

/* Variable declarations ------------------------------- */
extern elec::Relay sRelay;

/* Forward declaration of private functions ------------ */
int htmlSend(const int pClient, const char * const pStr);
int htmlSend(const int pClient, const std::string pStr);

int getLine(int pSock, char *pBuf, int pSize);
void cat(const int pClient, FILE * const pResource);
void serveFile(const int pClient, const char * const pFileName);
void errorDie(const char * const sc);

void unimplemented(const int pClient);
void badRequest(const int pClient);
void cannotExecute(const int pClient);
void headers(const int pClient, const char * const pFileName);
void notFound(const int pClient);
void home(const int pClient);
void togglePage(const int pClient);

/* Functions ------------------------------------------- */
int htmlSend(const int pClient, const char * const pStr) {
    int lResult = 0;
    
    errno = 0;
    lResult = send(pClient, pStr, strlen(pStr), 0);
    if(0 > lResult) {
        std::cerr << "[ERROR] <htmlSend> send failed with error code " << lResult << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << ", " << strerror(errno) << std::endl;
        }
    }

    return lResult;
}

int htmlSend(const int pClient, const std::string pStr) {
    int lResult = 0;
    
    errno = 0;
    lResult = send(pClient, pStr.c_str(), pStr.length(), 0);
    if(0 > lResult) {
        std::cerr << "[ERROR] <htmlSend> send failed with error code " << lResult << std::endl;
        if(errno) {
            std::cerr << "        errno = " << errno << ", " << strerror(errno) << std::endl;
        }
    }

    return lResult;
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int getLine(int pSock, char *pBuf, int pSize)
{
    /* Variable declarations */
    char c = '\0';
    char *lBuf = pBuf;
    bool lGotCR = false;
    int lReadChars = 0, i = 0;

    /* Loop over the line */
    do {
        /* Read a character */
        errno = 0;
        lReadChars = recv(pSock, &c, 1U, 0);

        /* Check errno */
        bool lErrnoBreak = false;
        switch (errno)
        {
            case 0:
                /* Nothing to do, everything is fine ! */
                break;
#if EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif /* EAGAIN != EWOULDBLOCK */
            case EWOULDBLOCK: 
                /* Socket is O_NONBLOCK and there is no data available */
                std::cerr << "[WARN ] <getLine> recv got EAGAIN, this is normal, nothing received, errno = " << errno << " (" << std::string(strerror(errno)) << ")" << std::endl;
                lErrnoBreak = true;
                break;
            case EINTR: 
                /* An interrupt (signal) has been caught */
                /* This should be ingored in most cases */
                std::cerr << "[ERROR] <getLine> recv got EINTR, an interrupt (signal) has been caught. Should be ignored. errno = " << errno << " (" << std::string(strerror(errno)) << ")" << std::endl;
                lErrnoBreak = true;
                break;
            default:
                /* socket has an error, not valid anymore */
                std::cerr << "[ERROR] <getLine> recv failed, errno = " << errno << " (" << std::string(strerror(errno)) << ")" << std::endl;
                //std::cout << "[DEBUG] <getLine> c = " << c << ", i.e. : " << std::hex << (unsigned int)c << std::dec << std::endl;
                return -1;
        }

        /** Break the do/while loop if we caught an errno 
         * that requires a break of loop */
        if(lErrnoBreak) {
            break;
        }

        /* Check lReadChars */
        if(0 > lReadChars) {
            std::cerr << "[ERROR] <getLine> recv failed !" << std::endl;
            return -1;
        } else if (0 == lReadChars) {
            //std::cout << "[WARN ] <getLine> Unexpected behavior, read 0 chars from socket but no errors..." << std::endl;
            /* The socket buffer is empty, return */
            break;
        }

        /* Put the character in the buffer */
        *lBuf = c;
        ++lBuf;

        /* Increment the number of chars read */
        ++i;

        /* Check for CR */
        if('\r' == c) {
            lGotCR = true;
            /* e expect to get a LF afterwards */
            //std::cout << "[DEBUG] <getLine> Got CR, expecting an LF..." << std::endl;
        }

        /** Check the character for LF 
         * Note : In the HTTP protocol, the CR-LF sequence is always used to terminate a line.
         */
        if('\n' == c) {
            if(!lGotCR) {
                std::cout << "[WARN ] <getLine> Got LF without CR, doesn't comply with HTTP protocols !" << std::endl;
            } else {
                //std::cout << "[DEBUG] <getLine> Got LF after CR !" << std::endl;
            }
            break;
        }

        if('\0' == c) {
            //std::cout << "[DEBUG] <getLine> Got \\0, exiting..." << std::endl;
            break;
        }

        //std::cout << "[DEBUG] <getLine> c = " << c << ", i.e. : " << std::hex << (unsigned int)c << std::dec << std::endl;
        // {
        //     std::string pBufString(pBuf);
        //     std::cout << "[DEBUG] <getLine> pBufString = " << std::string(pBufString) << std::endl;
        //     if(pBufString.empty()) {
        //         std::cout << "[DEBUG] <getLine> c = " << c << ", i.e. : " << std::hex << (unsigned int)c << std::dec << std::endl;
        //     }
        // }

    } while(true);

    return i;
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(const int pClient, FILE * const pResource) {
    char lBuf[1024U];
    memset(lBuf, 0, 1024U);

    /* Get the contents of the file */
    fgets(lBuf, sizeof(lBuf), pResource);

    /* Send the contents of the file into the socket */
    while (!feof(pResource)) {
        errno = 0;
        send(pClient, lBuf, strlen(lBuf), 0);
        if(errno) {
            std::cerr << "[ERROR] <cat> send failed : errno = " << errno << ", " << strerror(errno) << std::endl;
        }
        fgets(lBuf, sizeof(lBuf), pResource);
    }
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serveFile(const int pClient, const char * const pFileName) {
    FILE         *lResource   = nullptr;
    unsigned int  lNumChars   = 1;
    char          lBuf[1024U];
    memset(lBuf, 0, 1024U);

    lBuf[0U] = 'A';
    lBuf[1U] = '\0';

    while ((lNumChars > 0) && strcmp("\n", lBuf)) { /* read & discard headers */
        lNumChars = getLine(pClient, lBuf, sizeof(lBuf));
    }

    lResource = fopen(pFileName, "r");
    if (lResource == NULL) {
        notFound(pClient);
    } else {
        headers(pClient, pFileName);
        cat(pClient, lResource);
    }
    
    fclose(lResource);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char * const sc)
{
    perror(sc);
    exit(EXIT_FAILURE);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(const int pClient)
{
    std::string lUnimplemented = "HTTP/1.0 501 Method Not Implemented\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang=\"en\">\r\n"
    "    <head>\r\n"
    "        <title>Method Not Implemented</title>\r\n"
    "    </head>\r\n"
    "    <body>\r\n"
    "        HTTP request method not supported.\r\n"
    "    </body>\r\n"
    "</html>\r\n";
    
    const int lResult = htmlSend(pClient, lUnimplemented);
    if(0 > lResult) {
        std::cerr << "[ERROR] <unimplemented> htmlSend failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void badRequest(const int pClient)
{
    std::string lBadReq = "HTTP/1.0 400 BAD REQUEST\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">"
    "    <head>\r\n"
    "        <title>web-example - 500 Server error</title>\r\n"
    "    </head>\r\n"
    "    <body>\r\n"
    "        Your browser sent a bad request, "
    "        such as a POST without a Content-Length.\r\n"
    "    </body>\r\n"
    "</html>\r\n";

    const int lResult = htmlSend(pClient, lBadReq);
    if(0 > lResult) {
        std::cerr << "[ERROR] <badRequest> htmlSend failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannotExecute(const int pClient)
{
    std::string lExeError = "HTTP/1.0 500 Internal Server Error\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">"
    "    <head>\r\n"
    "        <title>web-example - 500 Server error</title>\r\n"
    "    </head>\r\n"
    "    <body>\r\n"
    "        Server-side error prohibited execution."
    "    </body>\r\n"
    "</html>\r\n";

    const int lResult = htmlSend(pClient, lExeError);
    if(0 > lResult) {
        std::cerr << "[ERROR] <cannotExecute> htmlSend failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(const int pClient, const char * const pFileName)
{
    std::string lHeader = "HTTP/1.0 200 Ok\r\n"
    "Content-Type: text/html\r\n"
    "\r\n";

    const int lResult = htmlSend(pClient, lHeader);
    if(0 > lResult) {
        std::cerr << "[ERROR] <headers> htmlSend failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void notFound(const int pClient)
{
    std::string l404 = "HTTP/1.0 404 NOT FOUND\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">"
    "    <head>\r\n"
    "        <title>web-example - 404 Not Found</title>\r\n"
    "    </head>\r\n"
    "    <body>\r\n"
    "        The server could not fulfill your request because the resource specified\r\n"
    "        is unavailable or nonexistent.\r\n"
    "        <br>\r\n"
    "        Please check that you entered the correct URL.\r\n"
    "    </body>\r\n"
    "</html>\r\n";

    const int lResult = htmlSend(pClient, l404);
    if(0 > lResult) {
        std::cerr << "[ERROR] <notFound> htmlSend failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* Give a client a "found" status message. */
/**********************************************************************/
void home(const int pClient) {
    std::string lHome = "HTTP/1.0 200 Ok\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">"
    "    <head>\r\n"
    "        <title>web-example</title>\r\n"
    "    </head>\r\n"
    "    <body>\r\n"
    "        Hello there !<br>\r\n"
    "        General Kenobi !\r\n"
    "    </body>\r\n"
    "</html>\r\n";

    const int lResult = htmlSend(pClient, lHome);
    if(0 > lResult) {
        std::cerr << "[ERROR] <home> htmlSend failed with return code " << lResult << std::endl;
    }
}

void togglePage(const int pClient) {
    std::string lTogglePage = "HTTP/1.0 200 Ok\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">"
    "    <link href=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css\" rel=\"stylesheet\" />"
    "    <script src=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js\"></script>"
        
    "    <title>web-example - Toggle switch</title>"
    "    <body>"
    "        <h2>Toggle Switch</h2>"
    "        <h3>Switch is currently at value : " + std::string((sRelay.isOn() ? "ON" : "OFF")) + "<h3>"
    "        <form action=\"\" method=\"post\">"
    "            <input type=\"submit\" name=\"Toggle\" value=\"Toggle Lamp\" />"
    "        </form>"
    "    </body>"
    "</html>";

    //std::cout << "[DEBUG] Toggle page code :" << std::endl << std::endl << lTogglePage << std::endl;
    std::cout << "[DEBUG] Sending the toggle page code !" << std::endl;

    const int lResult = htmlSend(pClient, lTogglePage);
    if(0 > lResult) {
        std::cerr << "[ERROR] <home> togglePage failed with return code " << lResult << std::endl;
    }
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
int acceptRequest(const int pClient, int * const pResult) {
    if(nullptr == pResult) {
        std::cerr << "[ERROR] Result is a nullptr !" << std::endl;
        return -1;
    }

    char         *lQueryStr = nullptr;
    unsigned int  lNumChars = 0;

    char lBuf[1024U], lMethodStr[256U], lURLStr[256U], lPath[512U];
    memset(lBuf,       0, 1024U);
    memset(lMethodStr, 0, 256U);
    memset(lURLStr,    0, 256U);
    memset(lPath,      0, 512U);

    lNumChars = getLine(pClient, lBuf, sizeof(lBuf));
    std::cout << "[DEBUG] <acceptRequest> Got \"" << std::string(lBuf) << "\" from the client. " << std::endl;

    size_t i = 0U, j = 0U;

    /* Get the method from the client's request */
    while((!IS_SPACE(lBuf[j]) && (sizeof(lMethodStr) - 1 > i))) {
        lMethodStr[i++] = lBuf[j++];
    }
    lMethodStr[i] = '\0'; /* NULL terminate the method */

    /* Get the URL of the request from the client's request */
    i = 0;
    while(IS_SPACE(lBuf[j]) && (sizeof(lBuf) > j)) {
        ++j;
    }
    while (!IS_SPACE(lBuf[j]) && (sizeof(lURLStr) - 1 > i) && (sizeof(lBuf) > j)) {
        lURLStr[i++] = lBuf[j++];
    }
    lURLStr[i] = '\0'; /* NULL terminate the URL */
    std::string lURL(lURLStr);
    std::cout << "[DEBUG] <acceptRequest> The URL is " << lURL << std::endl;

    /* Get the query from the client's request */
    lQueryStr = lURLStr;
    while(('?' != *lQueryStr) && ('\0' != *lQueryStr)) {
        ++lQueryStr;
    }

    /* The real query is located after the "?" symbol */
    if('?' == *lQueryStr) {
        *lQueryStr = '\0';
        ++lQueryStr;
    }
    std::string lQuery = std::string(lQueryStr);
    if(lQuery.empty()) {
        std::cout << "[DEBUG] <acceptRequest> No query found. " << std::endl;
    } else {
        std::cout << "[DEBUG] <acceptRequest> The query is " << lQuery << " (empty = " << (lQuery.empty() ? "true" : "false") << ")" << std::endl;
    }

    /* What is the URL ? */
    if("/" == lURL || "/index.html" == lURL) {
        /* Home page */
    } else {
        /* Unimplemented */
        notFound(pClient);
        std::cout << "[ERROR] Unknown URL requested !" << std::endl;
        close(pClient);
        return -1;
    }

    /* What method is it ? */
    restMethod_t lMethod = REST_UNKNOWN;
    if(0 == strcasecmp("GET", lMethodStr)) {
        lMethod = REST_GET;
    } else if (0 == strcasecmp("POST", lMethodStr)) {
        lMethod = REST_POST;
    } else if (0 == strcasecmp("PUT", lMethodStr)) {
        lMethod = REST_PUT;
    } else if (0 == strcasecmp("HEAD", lMethodStr)) {
        lMethod = REST_HEAD;
    } else if (0 == strcasecmp("DELETE", lMethodStr)) {
        lMethod = REST_DELETE;
    } else if (0 == strcasecmp("PATCH", lMethodStr)) {
        lMethod = REST_PATCH;
    } else if (0 == strcasecmp("OPTIONS", lMethodStr)) {
        lMethod = REST_OPTIONS;
    } else {
        unimplemented(pClient);
        std::cout << "[ERROR] <acceptRequest> Unknown REST method requested ! " << std::endl;
        close(pClient);
        return -1;
    }

    std::cout << "[DEBUG] <acceptRequest> Request str = " << std::string(lMethodStr) << std::endl;
    switch(lMethod) {
        case REST_GET:
            std::cout << "[DEBUG] <acceptRequest> Request is GET" << std::endl;
            togglePage(pClient);
            break;
        case REST_POST:
            std::cout << "[DEBUG] <acceptRequest> Request is POST" << std::endl;

            /* Switch the relays state */
            sRelay.switchState();

            /* Check the relay's state, print it out */
            if(sRelay.isOn()) {
                std::cout << "[DEBUG] <acceptRequest> Toggled relay, is now ON" << std::endl;
            } else {
                std::cout << "[DEBUG] <acceptRequest> Toggled relay, is now OFF" << std::endl;
            }

            /* Show the main page */
            togglePage(pClient);
            break;
        case REST_INDEX:
            std::cout << "[DEBUG] <acceptRequest> Request is INDEX" << std::endl;
            home(pClient);
            break;
        case REST_PUT:
            std::cout << "[DEBUG] <acceptRequest> Request is PUT" << std::endl;
            unimplemented(pClient);
            close(pClient);
            return -1;
        case REST_UNKNOWN:
        default:
            std::cout << "[ERROR] <acceptRequest> Request is unknown" << std::endl;
            unimplemented(pClient);
            close(pClient);
            return -1;
    }

    /* Close the client */
    close(pClient);

    return 0;
}