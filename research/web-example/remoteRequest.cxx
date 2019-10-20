/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file acceptRequest.cxx
 */

/* Includes -------------------------------------------- */
#include "remoteRequest.hxx"

#include "Relay.hxx"

/* C++ System */
#include <iostream>

/* C System */
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>

#include <ctype.h>

/* Defines --------------------------------------------- */
#define IS_SPACE(x) isspace((int)(x))

/* Variable declarations ------------------------------- */
extern elec::Relay sRelay;

/* Functions ------------------------------------------- */
int htmlSend(const int pClient, const char * const pStr) {
    return send(pClient, pStr, strlen(pStr), 0);
}

int htmlSend(const int pClient, const std::string pStr) {
    return send(pClient, pStr.c_str(), pStr.length(), 0);
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
    int i = 0, n = 0;
    char c = '\0';

    while ((i < pSize - 1) && (c != '\n'))
    {
        n = recv(pSock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(pSock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(pSock, &c, 1, 0);
                else
                    c = '\n';
            }
            pBuf[i++] = c;
        }
        else
        c = '\n';
    }
    pBuf[i] = '\0';

    return(i);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(const int pClient, FILE * const pResource)
{
    char lBuf[1024U];
    memset(lBuf, 0, 1024U);

    /* Get the contents of the file */
    fgets(lBuf, sizeof(lBuf), pResource);

    /* Send the contents of the file into the socket */
    while (!feof(pResource)) {
        send(pClient, lBuf, strlen(lBuf), 0);
        fgets(lBuf, sizeof(lBuf), pResource);
    }
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
    
    htmlSend(pClient, lUnimplemented);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(const int pClient)
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

    (void)htmlSend(pClient, lBadReq);
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(const int pClient)
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

    (void)htmlSend(pClient, lExeError);
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

    (void)htmlSend(pClient, lHeader);
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

    (void)htmlSend(pClient, l404);
}

/**********************************************************************/
/* Give a client a "found" status message. */
/**********************************************************************/
void home(const int pClient)
{
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

    (void)htmlSend(pClient, lHome);
}

void togglePage(const int pClient, const int pVar = 0) {
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
    int lResult = htmlSend(pClient, lTogglePage);
    std::cout << "[DEBUG] htmlSend returned " << lResult << std::endl;
}
/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(const int pClient, const char * const pFileName)
{
    FILE         *lResource   = NULL;
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
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{
    char    buf[1024];
    int     cgi_output[2];
    int     cgi_input[2];
    pid_t   pid;
    int     status;
    int     i;
    char    c;
    int     numchars        = 1;
    int     content_length  = -1;

    buf[0]  = 'A';
    buf[1]  = '\0';
    if (strcasecmp(method, "GET") == 0) {
        while ((numchars > 0) && strcmp("\n", buf)) { /* read & discard headers */
            numchars = getLine(client, buf, sizeof(buf));
        }
    } else { /* POST */
        numchars = getLine(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf)) {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0) {
                content_length = atoi(&(buf[16]));
            }
            numchars = getLine(client, buf, sizeof(buf));
        }
        if (content_length == -1) {
            bad_request(client);
            return;
        }
    }

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);

    if (pipe(cgi_output) < 0) {
        cannot_execute(client);
        return;
    }
    if (pipe(cgi_input) < 0) {
        cannot_execute(client);
        return;
    }

    if ((pid = fork()) < 0) {
        cannot_execute(client);
        return;
    }
    if (pid == 0) { /* child: CGI script */
        char    meth_env[255];
        char    query_env[255];
        char    length_env[255];

        dup2(cgi_output[1], 1);
        dup2(cgi_input[0], 0);
        close(cgi_output[0]);
        close(cgi_input[1]);
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        if (strcasecmp(method, "GET") == 0) {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        } else { /* POST */
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, path, NULL);
        exit(0);
    } else { /* parent */
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (strcasecmp(method, "POST") == 0) {
            for (i = 0; i < content_length; i++) {
                recv(client, &c, 1, 0);
                write(cgi_input[1], &c, 1);
            }
        }
        while (read(cgi_output[0], &c, 1) > 0) {
            send(client, &c, 1, 0);
        }

        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
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