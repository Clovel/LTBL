/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file acceptRequest.cxx
 */

/* Includes -------------------------------------------- */
#include "remoteRequest.hxx"

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

/* Functions ------------------------------------------- */
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
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int pClient)
{
    char lBuf[1024];

    sprintf(lBuf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "Content-Type: text/html\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "</TITLE></HEAD>\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "<BODY><P>HTTP request method not supported.\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
    sprintf(lBuf, "</BODY></HTML>\r\n");
    send(pClient, lBuf, strlen(lBuf), 0);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)
{
    char buf[1024];

    fgets(buf, sizeof(buf), resource);
    while (!feof(resource)) {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
    perror(sc);
    exit(EXIT_FAILURE);
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
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{
    char buf[1024];

    (void)filename; /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a "found" status message. */
/**********************************************************************/
void home(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 200 Ok\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Hello there !</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>General Kenobi !\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
    FILE    *resource   = NULL;
    int     numchars    = 1;
    char    buf[1024];

    buf[0]  = 'A';
    buf[1]  = '\0';
    while ((numchars > 0) && strcmp("\n", buf)) { /* read & discard headers */
        numchars = getLine(client, buf, sizeof(buf));
    }

    resource = fopen(filename, "r");
    if (resource == NULL) {
        not_found(client);
    } else {
        headers(client, filename);
        cat(client, resource);
    }
    fclose(resource);
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

    char *lQueryStr = nullptr;
    int lNumChars   = 0;

    char lBuf[1024U], lMethodStr[256U], lURL[256U], lPath[512U];
    memset(lBuf,       0, 1024U);
    memset(lMethodStr, 0, 256U);
    memset(lURL,       0, 256U);
    memset(lPath,      0, 512U);

    lNumChars = getLine(pClient, lBuf, sizeof(lBuf));
    std::cout << "[DEBUG] <acceptRequest> Got \"" << std::string(lBuf) << "\" from the client. " << std::endl;

    size_t i = 0, j = 0;

    /* Get the method from the client's request */
    while((!IS_SPACE(lBuf[j]) && (sizeof(lMethodStr) - 1 > i))) {
        lMethodStr[i++] = lBuf[j++];
    }
    lMethodStr[i] = '\0'; /* NULL terminate the method */

    /* Get the URL of the request form the client's request */
    i = 0;
    while(IS_SPACE(lBuf[j]) && (sizeof(lBuf) > j)) {
        ++j;
    }
    while (!IS_SPACE(lBuf[j]) && (sizeof(lURL) - 1 > i) && (sizeof(lBuf) > j)) {
        lURL[i++] = lBuf[j++];
    }
    lURL[i] = '\0'; /* NULL terminate the URL */
    std::cout << "[DEBUG] <acceptRequest> The URL is " << std::string(lURL) << std::endl;

    /* What method is it ? */
    restMethod_t lMethod = REST_UNKNOWN;
    if(0 == strcasecmp("POST", lMethodStr)) {
        lMethod = REST_POST;
    } else if (0 == strcasecmp("GET", lMethodStr)) {
        lMethod = REST_GET;
    } else {
        /* We only support POST & GET for now */
        unimplemented(pClient);
        return -1;
    }

    std::cout << "[DEBUG] <acceptRequest> Request str = " << std::string(lMethodStr) << std::endl;
    switch(lMethod) {
        case REST_GET:
            std::cout << "[DEBUG] <acceptRequest> Request is GET" << std::endl;
            /* What is the client trying to GET ? */
            lQueryStr = lURL;
            std::cout << "[DEBUG] <acceptRequest> The GET query is " << std::string(lQueryStr) << std::endl;
            while(('?' != *lQueryStr) && ('\0' != *lQueryStr)) {
                ++lQueryStr;
            }

            /* The real query is located after the "?" symbol */
            if('?' == *lQueryStr) {
                *lQueryStr = '\0';
                ++lQueryStr;
            }
            std::cout << "[DEBUG] <acceptRequest> The GET query is " << std::string(lQueryStr) << std::endl;
            if('\0' == *lQueryStr) {
                std::cout << "[DEBUG] <acceptRequest> Showing home because query is empty" << std::endl;
                home(pClient);
                break;
            }
            break;
        case REST_POST:
            std::cout << "[DEBUG] <acceptRequest> Request is POST" << std::endl;
            break;
        case REST_INDEX:
            std::cout << "[DEBUG] <acceptRequest> Request is INDEX" << std::endl;
            home(pClient);
            break;
        case REST_PUT:
            std::cout << "[DEBUG] <acceptRequest> Request is PUT" << std::endl;
            unimplemented(pClient);
            return -1;
        case REST_UNKNOWN:
        default:
            std::cout << "[ERROR] <acceptRequest> Request is unknown" << std::endl;
            unimplemented(pClient);
            return -1;
    }

    return 0;
}