/** 
 * Let There Be Light project
 * 
 * @file webtools.cpp
 */

/* Includes -------------------------------------------- */
#include "webtools.hpp"

#include "Relay.hpp"
#include "HttpRequest.hpp"
#include "webpage.hpp"

#include "Logger.hpp"

/* ESP includes */
#include <ESP8266WiFi.h>

/* Defines --------------------------------------------- */
#define IS_SPACE(x) isspace((int)(x))

/* Variable declarations ------------------------------- */
extern elec::Relay *gRelay;
extern Logger *gLogger;

/* Forward declaration of private functions ------------ */
int htmlSend(WiFiClient * const pClient, const char * const pStr);
int htmlSend(WiFiClient * const pClient, const std::string pStr);

int getLine(int pSock, char *pBuf, int pSize);
#if 0
void cat(WiFiClient * const pClient, FILE * const pResource);
void serveFile(WiFiClient * const pClient, const char * const pFileName);
#endif /* 0 */
void errorDie(const char * const sc);

void unimplemented(WiFiClient * const pClient);
void badRequest(WiFiClient * const pClient);
void cannotExecute(WiFiClient * const pClient);
void headers(WiFiClient * const pClient, const char * const pFileName);
void notFound(WiFiClient * const pClient);
void home(WiFiClient * const pClient);
void togglePage(WiFiClient * const pClient);

/* Functions ------------------------------------------- */
int htmlSend(WiFiClient * const pClient, const char * const pStr) {
    int lResult = pClient->write(pStr, strlen(pStr));
    if(0 > lResult) {
        *gLogger << "[ERROR] <htmlSend> write failed with error code " << lResult << endlog;
    }

    return lResult;
}

int htmlSend(WiFiClient * const pClient, const std::string pStr) {
    int lResult = pClient->write(pStr.c_str(), pStr.length());
    if(0 > lResult) {
        *gLogger << "[ERROR] <htmlSend> write failed with error code " << lResult << endlog;
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
int getLine(WiFiClient * const pClient, char *pBuf, int pSize)
{
    /* Variable declarations */
    char lChar = '\0';
    char *lBuf = pBuf;
    bool lGotCR = false;
    int lReadChars = 0, i = 0;

    /* Loop over the line */
    do {

        /* Check if there are bytes available */
        if(!pClient->available())
            break;
        
        /* Read a character */
        lChar = pClient->read();

        /* Check lReadChars */
        if(0 > lReadChars) {
            *gLogger << "[ERROR] <getLine> recv failed !" << endlog;
            return -1;
        } else if (0 == lReadChars) {
            //*gLogger << "[WARN ] <getLine> Unexpected behavior, read 0 chars from socket but no errors..." << endlog;
            /* The socket buffer is empty, return */
            break;
        }

        /* Put the character in the buffer */
        *lBuf = lChar;
        ++lBuf;

        /* Increment the number of chars read */
        ++i;

        /* Check for CR */
        if('\r' == lChar) {
            lGotCR = true;
            /* e expect to get a LF afterwards */
            //*gLogger << "[DEBUG] <getLine> Got CR, expecting an LF..." << endlog;
        }

        /** Check the character for LF 
         * Note : In the HTTP protocol, the CR-LF sequence is always used to terminate a line.
         */
        if('\n' == lChar) {
            if(!lGotCR) {
                *gLogger << "[WARN ] <getLine> Got LF without CR, doesn't comply with HTTP protocols !" << endlog;
            } else {
                //*gLogger << "[DEBUG] <getLine> Got LF after CR !" << endlog;
            }
            break;
        }

        if('\0' == lChar) {
            //*gLogger << "[DEBUG] <getLine> Got \\0, exiting..." << endlog;
            break;
        }

        // Serial.print("[DEBUG] <getLine> lChar = ");
        // Serial.print(", i.e. : 0x");
        // Serial.println(lChar, HEX);
        // {
        //     std::string pBufString(pBuf);
        //     Serial.print("[DEBUG] <getLine> pBufString = ");
        //     Serial.println(std::string(pBufString));
        //     if(pBufString.empty()) {
        //         Serial.print("[DEBUG] <getLine> lChar = ");
        //         Serial.print(lChar);
        //         Serial.print(", i.e. : 0x");
        //         Serial.println(lChar, HEX);
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
#if 0
void cat(WiFiClient * const pClient, FILE * const pResource) {
    char lBuf[1024U];
    memset(lBuf, 0, 1024U);

    /* Get the contents of the file */
    fgets(lBuf, sizeof(lBuf), pResource);

    /* Send the contents of the file into the socket */
    while (!feof(pResource)) {
        htmlSend(pClient, lBuf);
        fgets(lBuf, sizeof(lBuf), pResource);
    }
}
#endif /* 0 */

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
#if 0
void serveFile(WiFiClient * const pClient, const char * const pFileName) {
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
#endif /* 0 */

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
void unimplemented(WiFiClient * const pClient)
{
    const int lResult = htmlSend(pClient, htmlResponseCode501);
    if(0 > lResult) {
        *gLogger << "[ERROR] <unimplemented> htmlSend failed with return code " << lResult << endlog;
    }
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void badRequest(WiFiClient * const pClient)
{
    const int lResult = htmlSend(pClient, htmlResponseCode400);
    if(0 > lResult) {
        *gLogger << "[ERROR] <badRequest> htmlSend failed with return code " << lResult << endlog;
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannotExecute(WiFiClient * const pClient)
{
    const int lResult = htmlSend(pClient, htmlResponseCode500);
    if(0 > lResult) {
        *gLogger << "[ERROR] <cannotExecute> htmlSend failed with return code " << lResult << endlog;
    }
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(WiFiClient * const pClient, const char * const pFileName)
{
    const int lResult = htmlSend(pClient, htmlResponseCode200);
    if(0 > lResult) {
        *gLogger << "[ERROR] <headers> htmlSend failed with return code " << lResult << endlog;
    }
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void notFound(WiFiClient * const pClient)
{
    const int lResult = htmlSend(pClient, htmlResponseCode404);
    if(0 > lResult) {
        *gLogger << "[ERROR] <notFound> htmlSend failed with return code " << lResult << endlog;
    }
}

/**********************************************************************/
/* Give a client a "found" status message. */
/**********************************************************************/
void home(WiFiClient * const pClient)
{
    int lResult = htmlSend(pClient, htmlResponseCode200);
    if(0 > lResult) {
        *gLogger << "[ERROR] <home> htmlSend failed with return code " << lResult << endlog;
        return;
    }

    lResult = htmlSend(pClient, htmlGeneralKenobi);
    if(0 > lResult) {
        *gLogger << "[ERROR] <home> htmlSend failed with return code " << lResult << endlog;
    }
}

void togglePage(WiFiClient * const pClient) {
    std::string lTogglePage = "HTTP/1.0 200 Ok\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<html lang =\"en\">\r\n"
    "    <link href=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css\" rel=\"stylesheet\" />\r\n"
    "    <script src=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js\"></script>\r\n"
    "<head>\r\n"
    "    <meta charset=\"utf-8\">\r\n"
    "    <title>web-example - Toggle switch</title>\r\n"
    "</head>\r\n"
    "\r\n"
    "    <body>\r\n"
    "        <h2>Toggle Switch</h2>\r\n"
    "        <h3>Switch is currently at value : " + std::string((gRelay->isOn() ? "ON" : "OFF")) + "<h3>\r\n"
    "        <form action=\"/toggle\" method=\"get\">\r\n"
    "            <input type=\"submit\" name=\"Toggle Lamp\" value=\"Toggle Lamp\" />\r\n"
    "        </form>\r\n"
    "    </body>\r\n"
    "</html>\r\n\r\n";

    *gLogger << "[DEBUG] Sending the toggle page code !" << endlog;

    const int lResult = htmlSend(pClient, lTogglePage);
    if(0 > lResult) {
        *gLogger << "[ERROR] <togglePage> htmlSend failed with return code " << lResult << endlog;
    } else {
        *gLogger << "[DEBUG] <togglePage> Toggle page code sent : " << lTogglePage.c_str() << endlog;
    }
}

void togglePage2(WiFiClient * const pClient)
{
    std::string lTogglePage2 = R"=====(
    <body>
        <h1>
            ESP8266 Web Server - LTBL
        </h1>
        <p>
            GPIO 5 - State )=====" + gRelay->stringState() +
            R"=====(</p>
        <p>
            <a href="/)=====" + (gRelay->isOn() ? "off" : "on") + R"=====(">
                <button class="button)=====" + (gRelay->isOn() ? R"=====( button2">)=====" : R"=====(">)=====") +
                    (gRelay->isOn() ? "OFF" : "ON") +
                R"=====(</button>
            </a>
        </p>
    </body>
    )=====";

    *gLogger << "[DEBUG] Sending the toggle page code !" << endlog;

    const int lResult = htmlSend(pClient, lTogglePage2);
    if(0 > lResult) {
        *gLogger << "[ERROR] <togglePage2> htmlSend failed with return code " << lResult << endlog;
    } else {
        //*gLogger << "[DEBUG] <togglePage2> Toggle page 2 code sent : " << lTogglePage2.c_str() << endlog;
    }
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return. Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
int acceptRequest(WiFiClient * const pClient) {
    if(nullptr == pClient) {
        *gLogger << "[ERROR] Client is a nullptr !" << endlog;
        return -1;
    }

    int lNumChars = 0;

    HttpRequest lRequest;

    char lBuf[1024U];
    memset(lBuf, 0, 1024U);

    std::string lRequestStr;
    do {
        lNumChars = 0;
        /* Reset the buffer to receive the line */
        memset(lBuf, 0, 1024U);

        /* Get the line */
        lNumChars = getLine(pClient, lBuf, sizeof(lBuf));
        //std::cout << "[DEBUG] <acceptRequest> Got \"" << std::string(lBuf) << "\" from the client. " << std::endl;

        if(0 > lNumChars) {
            *gLogger << "[ERROR] <acceptRequest> getLine failed, returned " << lNumChars << endlog;
            pClient->stop();
            return -1;
        } else if(0 == lNumChars) {
            *gLogger << "[WARN ] <acceptRequest> Read 0 characters !" << endlog;
        } else if(std::string(lBuf).empty()) {
            *gLogger << "[WARN ] <acceptRequest> Got an empty line !" << endlog;
            if(lNumChars) {
                *gLogger << "[ERROR] <acceptRequest> Got an empty line, but lNumChars = " << lNumChars << endlog;
                pClient->stop();
                return -1;
            }
        }

        /* Append the line to the request */
        lRequestStr += std::string(lBuf);
    } while(0 < lNumChars);
    
    /* Print out what we received */
    *gLogger << "[DEBUG] <acceptRequest> lRequestStr = \"" << lRequestStr.c_str() << "\"" << endlog;
    if(lRequestStr.empty()) {
        *gLogger << "[WARN ] Got an empty request, exiting..." << endlog;
        pClient->stop();
        return -1;
    }
    lRequest.parseRequest(lRequestStr);

    /* Get the method from the client's request */
    std::string lMethod = lRequest.methodStr();
    *gLogger << "[DEBUG] <acceptRequest> The method is " << lMethod.c_str() << endlog;

    /* Get the URL of the request from the client's request */
    *gLogger << "[DEBUG] <acceptRequest> The requested URL is : " << lRequest.URL().c_str() << endlog;
    *gLogger << "[DEBUG] <acceptRequest> The short URL is     : " << lRequest.shortURL().c_str() << endlog;
    std::string lURL = lRequest.shortURL();

    /* The real query is located after the "?" symbol */
    std::string lQuery = lRequest.query();
    if(lQuery.empty()) {
        *gLogger << "[DEBUG] <acceptRequest> No query found." << endlog;
    } else {
        *gLogger << "[DEBUG] <acceptRequest> The query is " << lQuery.c_str() << endlog;
    }

    /* What method is it ? */
    restMethod_t lREST = REST_UNKNOWN;
    if(0 == strcasecmp("GET", lMethod.c_str())) {
        lREST = REST_GET;
    } else if (0 == strcasecmp("POST", lMethod.c_str())) {
        lREST = REST_POST;
    } else if (0 == strcasecmp("PUT", lMethod.c_str())) {
        lREST = REST_PUT;
    } else if (0 == strcasecmp("HEAD", lMethod.c_str())) {
        lREST = REST_HEAD;
    } else if (0 == strcasecmp("DELETE", lMethod.c_str())) {
        lREST = REST_DELETE;
    } else if (0 == strcasecmp("PATCH", lMethod.c_str())) {
        lREST = REST_PATCH;
    } else if (0 == strcasecmp("OPTIONS", lMethod.c_str())) {
        lREST = REST_OPTIONS;
    } else {
        unimplemented(pClient);
        *gLogger << "[ERROR] <acceptRequest> Unknown REST method requested ! " << endlog;
        pClient->stop();
        return -1;
    }

    switch(lREST) {
        case REST_GET:
            *gLogger << "[DEBUG] <acceptRequest> Request is GET" << endlog;

            /* What is the URL ? */
            if("/" == lURL || "/index.html" == lURL) {
                /* Home page */
            } else if ("/toggle" == lURL) {
                /* Switch the relays state */
                gRelay->switchState();

                /* Check the relay's state, print it out */
                if(gRelay->isOn()) {
                    *gLogger << "[DEBUG] <acceptRequest> Toggled relay, is now ON" << endlog;
                } else {
                    *gLogger << "[DEBUG] <acceptRequest> Toggled relay, is now OFF" << endlog;
                }
            } else {
                /* Unimplemented */
                notFound(pClient);
                *gLogger << "[ERROR] Unknown URL requested !" << endlog;
                pClient->stop();
                return -1;
            }

            /* Send back the toggle page */
            togglePage(pClient);
            break;
        case REST_POST:
            *gLogger << "[DEBUG] <acceptRequest> Request is POST" << endlog;
            unimplemented(pClient);
            pClient->stop();
            return -1;
            break;
        case REST_INDEX:
            *gLogger << "[DEBUG] <acceptRequest> Request is INDEX" << endlog;
            home(pClient);
            break;
        case REST_PUT:
            *gLogger << "[DEBUG] <acceptRequest> Request is PUT" << endlog;
            unimplemented(pClient);
            pClient->stop();
            return -1;
        case REST_UNKNOWN:
        default:
            *gLogger << "[ERROR] <acceptRequest> Request is unknown" << endlog;
            unimplemented(pClient);
            pClient->stop();
            return -1;
    }

    /* Close the client */
    delay(10U);
    pClient->stop();

    return 0;
}

int sendTogglePage(WiFiClient &pClient, const HttpRequest &pRequest) {
    /** HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
     * and a content-type so the client knows what's coming, then a blank line
     */
    (void)htmlSend(&pClient, htmlResponseCode200);

    /* Display the HTML web page */
    (void)htmlSend(&pClient, htmlDocType);
    (void)htmlSend(&pClient, htmlPageBegin);
    (void)htmlSend(&pClient, htmlHead);

    /* Turn the relay ON or OFF */
    if (pRequest.URL.find("/on") != std::string::npos) {
        *gLogger << "Relay ON" << endlog;
        gRelay->turnOn();
    } else if (pRequest.URL.find("/off") != std::string::npos) {
        *gLogger << "Relay OFF" << endlog;
        gRelay->turnOff();
    } else if (String(pRequest.URL.c_str()).indexOf("/") >= 0) {
        *gLogger << "Get Relay state" << endlog;
    }

    /* Toggle web page */
    togglePage2(&pClient);

    /* End the web page */
    htmlSend(&pClient, htmlPageEnd);

    return 0;
}

/**********************************************************************/
/* A test function given as an example by 
 * https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/ */
/**********************************************************************/
int testAccept(WiFiClient * const pClient, 
    std::string * const pCurrentLine, 
    std::string * const pHeader)
{
    if(nullptr == pClient) {
        *gLogger << "[ERROR] pClient is a nullptr !" << endlog;
        return -1;
    }

    if(nullptr == pCurrentLine) {
        *gLogger << "[ERROR] pCurrentLine is a nullptr !" << endlog;
        return -1;
    }

    if(nullptr == pHeader) {
        *gLogger << "[ERROR] pHeader is a nullptr !" << endlog;
        return -1;
    }

    std::string lReceivedStr;
    HttpRequest lRequest;
    int lErrorCode = 0;
    // size_t lSentBytes = 0U;

    /* Loop while the client is still connected */
    while(pClient->connected()) {
        /* Check if there's bytes to read from the client */
        if(pClient->available()) {
            /* Read a byte */
            const char lChar = pClient->read();

            /* Add the char to the header */
            *pHeader += lChar;

            /* Check if the char is a newline */
            if('\n' == lChar) {
                /* if the current line is blank, you got two newline characters in a row.
                 * that's the end of the client HTTP request, so send a response
                 */
                if(0 == pCurrentLine->length()) {
                    /* At this point, we have recieved the HTTP request,
                     * so we can analyse it and decide what tot do.
                     * 
                     * Everything is stored in the pHeader variable;
                     */
                    httpRequestParseError_t lParseError = lRequest.parseRequest(*pHeader);
                    switch(lParseError) {
                        case HTTP_REQ_PARSE_ERROR_NONE:
                            lErrorCode = sendTogglePage(*pClient, lRequest);
                            break;
                        case HTTP_REQ_PARSE_ERROR_METHOD:
                            unimplemented(pClient);
                            lErrorCode = 1;
                            break;
                        case HTTP_REQ_PARSE_ERROR_CONTENTS:
                            badRequest(pClient);
                            lErrorCode = 1;
                            break;
                        default:
                            *gLogger << "[ERROR] <testAccept> parseRequest failed w/ error " << (unsigned int)lParseError << endlog;
                            lErrorCode = 1;
                            break;
                    }

                    /* Break the while loop now that we sent the response */
                    break;
                } else {
                    /* Append the current line to the received string buffer */
                    lReceivedStr += *pCurrentLine;

                    /* Reset the current line */
                    *pCurrentLine = "";
                }
            } else if ('\r' != lChar) {
                /* If you got anything else but a carriage return character,
                 * add it to the end of the current line
                 */
                *pCurrentLine += lChar;
            }
        }
    }

    /* Close the client when operation is done */
    pClient->stop();
    *gLogger << "[INFO ] <testAccept> Client disconnected." << endlog;

    return lErrorCode;
}
