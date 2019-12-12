/** 
 * Let There Be Light project
 * 
 * @file webtools.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"

#include "webtools.hpp"
#include "HttpRequest.hpp"

/* ESP includes */
#include <ESP8266WiFi.h>

/* Defines --------------------------------------------- */
#define IS_SPACE(x) isspace((int)(x))

/* Variable declarations ------------------------------- */
extern elec::Relay *gRelay;

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
        Serial.print("[ERROR] <htmlSend> write failed with error code ");
        Serial.println(lResult);
    }

    return lResult;
}

int htmlSend(WiFiClient * const pClient, const std::string pStr) {
    int lResult = pClient->write(pStr.c_str(), pStr.length());
    if(0 > lResult) {
        Serial.print("[ERROR] <htmlSend> write failed with error code ");
        Serial.println(lResult);
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
            Serial.println("[ERROR] <getLine> recv failed !");
            return -1;
        } else if (0 == lReadChars) {
            //Serial.println("[WARN ] <getLine> Unexpected behavior, read 0 chars from socket but no errors...");
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
            //Serial.println("[DEBUG] <getLine> Got CR, expecting an LF...");
        }

        /** Check the character for LF 
         * Note : In the HTTP protocol, the CR-LF sequence is always used to terminate a line.
         */
        if('\n' == lChar) {
            if(!lGotCR) {
                Serial.println("[WARN ] <getLine> Got LF without CR, doesn't comply with HTTP protocols !");
            } else {
                //Serial.println("[DEBUG] <getLine> Got LF after CR !");
            }
            break;
        }

        if('\0' == lChar) {
            //Serial.println("[DEBUG] <getLine> Got \\0, exiting...");
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
    "</html>\r\n\r\n";
    
    const int lResult = htmlSend(pClient, lUnimplemented);
    if(0 > lResult) {
        Serial.print("[ERROR] <unimplemented> htmlSend failed with return code ");
        Serial.println(lResult);
    }
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void badRequest(WiFiClient * const pClient)
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
    "</html>\r\n\r\n";

    const int lResult = htmlSend(pClient, lBadReq);
    if(0 > lResult) {
        Serial.print("[ERROR] <badRequest> htmlSend failed with return code ");
        Serial.println(lResult);
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannotExecute(WiFiClient * const pClient)
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
    "</html>\r\n\r\n";

    const int lResult = htmlSend(pClient, lExeError);
    if(0 > lResult) {
        Serial.print("[ERROR] <cannotExecute> htmlSend failed with return code ");
        Serial.println(lResult);
    }
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(WiFiClient * const pClient, const char * const pFileName)
{
    std::string lHeader = "HTTP/1.0 200 Ok\r\n"
    "Content-Type: text/html\r\n"
    "\r\n";

    const int lResult = htmlSend(pClient, lHeader);
    if(0 > lResult) {
        Serial.print("[ERROR] <headers> htmlSend failed with return code ");
        Serial.println(lResult);
    }
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void notFound(WiFiClient * const pClient)
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
    "</html>\r\n\r\n";

    const int lResult = htmlSend(pClient, l404);
    if(0 > lResult) {
        Serial.print("[ERROR] <notFound> htmlSend failed with return code ");
        Serial.println(lResult);
    }
}

/**********************************************************************/
/* Give a client a "found" status message. */
/**********************************************************************/
void home(WiFiClient * const pClient) {
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
    "</html>\r\n\r\n";

    const int lResult = htmlSend(pClient, lHome);
    if(0 > lResult) {
        Serial.print("[ERROR] <home> htmlSend failed with return code ");
        Serial.println(lResult);
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

    Serial.println("[DEBUG] Sending the toggle page code !");

    const int lResult = htmlSend(pClient, lTogglePage);
    if(0 > lResult) {
        Serial.print("[ERROR] <togglePage> htmlSend failed with return code ");
        Serial.println(lResult);
    } else {
        // Serial.println("[DEBUG] <togglePage> Toggle page code sent : ");
        // Serial.println(lTogglePage.c_str());
    }
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
int acceptRequest(WiFiClient * const pClient) {
    if(nullptr == pClient) {
        Serial.println("[ERROR] Client is a nullptr !");
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
            Serial.print("[ERROR] <acceptRequest> getLine failed, returned ");
            Serial.println(lNumChars);
            pClient->stop();
            return -1;
        } else if(0 == lNumChars) {
            Serial.println("[WARN ] <acceptRequest> Read 0 characters !");
        } else if(std::string(lBuf).empty()) {
            Serial.println("[WARN ] <acceptRequest> Got an empty line !");
            if(lNumChars) {
                Serial.print("[ERROR] <acceptRequest> Got an empty line, but lNumChars = ");
                Serial.println(lNumChars);
                pClient->stop();
                return -1;
            }
        }

        /* Append the line to the request */
        lRequestStr += std::string(lBuf);
    } while(0 < lNumChars);
    
    /* Print out what we received */
    Serial.print("[DEBUG] <acceptRequest> lRequestStr = \"");
    Serial.print(lRequestStr.c_str());
    Serial.println("\"");
    if(lRequestStr.empty()) {
        Serial.println("[WARN ] Got an empty request, exiting...");
        pClient->stop();
        return -1;
    }
    lRequest.parseRequest(lRequestStr);

    /* Get the method from the client's request */
    std::string lMethod = lRequest.method();
    Serial.print("[DEBUG] <acceptRequest> The method is ");
    Serial.println(lMethod.c_str());

    /* Get the URL of the request from the client's request */
    Serial.print("[DEBUG] <acceptRequest> The method is ");
    Serial.println(lRequest.URL().c_str());
    Serial.print("[DEBUG] <acceptRequest> The short URL is    : ");
    Serial.println(lRequest.shortURL().c_str());
    std::string lURL = lRequest.shortURL();

    /* The real query is located after the "?" symbol */
    std::string lQuery = lRequest.query();
    if(lQuery.empty()) {
        Serial.println("[DEBUG] <acceptRequest> No query found.");
    } else {
        Serial.print("[DEBUG] <acceptRequest> The query is ");
        Serial.println(lQuery.c_str());
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
        Serial.println("[ERROR] <acceptRequest> Unknown REST method requested ! ");
        pClient->stop();
        return -1;
    }

    switch(lREST) {
        case REST_GET:
            Serial.println("[DEBUG] <acceptRequest> Request is GET");

            /* What is the URL ? */
            if("/" == lURL || "/index.html" == lURL) {
                /* Home page */
            } else if ("/toggle" == lURL) {
                /* Switch the relays state */
                gRelay->switchState();

                /* Check the relay's state, print it out */
                if(gRelay->isOn()) {
                    Serial.println("[DEBUG] <acceptRequest> Toggled relay, is now ON");
                } else {
                    Serial.println("[DEBUG] <acceptRequest> Toggled relay, is now OFF");
                }
            } else {
                /* Unimplemented */
                notFound(pClient);
                Serial.println("[ERROR] Unknown URL requested !");
                pClient->stop();
                return -1;
            }

            /* Send back the toggle page */
            togglePage(pClient);
            break;
        case REST_POST:
            Serial.println("[DEBUG] <acceptRequest> Request is POST");
            unimplemented(pClient);
            pClient->stop();
            return -1;
            break;
        case REST_INDEX:
            Serial.println("[DEBUG] <acceptRequest> Request is INDEX");
            home(pClient);
            break;
        case REST_PUT:
            Serial.println("[DEBUG] <acceptRequest> Request is PUT");
            unimplemented(pClient);
            pClient->stop();
            return -1;
        case REST_UNKNOWN:
        default:
            Serial.println("[ERROR] <acceptRequest> Request is unknown");
            unimplemented(pClient);
            pClient->stop();
            return -1;
    }

    /* Close the client */
    delay(10U);
    pClient->stop();

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
        Serial.println("[ERROR] pClient is a nullptr !");
        return -1;
    }

    if(nullptr == pCurrentLine) {
        Serial.println("[ERROR] pCurrentLine is a nullptr !");
        return -1;
    }

    if(nullptr == pHeader) {
        Serial.println("[ERROR] pHeader is a nullptr !");
        return -1;
    }

    /* Loop while the client is still connected */
    while(pClient->connected()) {
        /* Check if there's bytes to read from the client */
        if(pClient->available()) {
            /* Read a byte */
            const char lChar = pClient->read();

            /* Print the char out the serial port */
            Serial.write(lChar);

            /* Add the char to the header */
            *pHeader += lChar;

            /* Check if the char is a newline */
            if('\n' == lChar) {
                /** if the current line is blank, you got two newline characters in a row.
                 * that's the end of the client HTTP request, so send a response
                 */
                if(0 == pCurrentLine->length()) {
                    /** HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                     * and a content-type so the client knows what's coming, then a blank line
                     */
                    pClient->println("HTTP/1.1 200 OK");
                    pClient->println("Content-type:text/html");
                    pClient->println("Connection: close");
                    pClient->println();

                    /* Display the HTML web page */
                    pClient->println("<!DOCTYPE html><html>");
                    pClient->println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                    pClient->println("<link rel=\"icon\" href=\"data:,\">");
                    // CSS to style the on/off buttons 
                    // Feel free to change the background-color and font-size attributes to fit your preferences
                    pClient->println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                    pClient->println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                    pClient->println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                    pClient->println(".button2 {background-color: #77878A;}</style></head>");

                    /* Turn the relay ON or OFF */
                    if (String(pHeader->c_str()).indexOf("GET /5/on") >= 0) {
                        Serial.println("Relay ON");
                        gRelay->turnOn();
                    } else if (String(pHeader->c_str()).indexOf("GET /5/off") >= 0) {
                        Serial.println("Relay OFF");
                        gRelay->turnOff();
                    }

                    /* Web page heading */
                    pClient->println("<body><h1>ESP8266 Web Server</h1>");

                    /* Display current state of Relay */
                    pClient->println(("<p>GPIO 5 - State " + gRelay->stringState() + "</p>").c_str());
                    if(gRelay->isOn()) {
                        pClient->println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
                    } else {
                        pClient->println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
                    }

                    /* End the web page */
                    pClient->println("</body></html>");
                    pClient->println();

                    break;
                } else {
                    *pCurrentLine = "";
                }
            } else if ('\r' != lChar) {
                 /** if you got anything else but a carriage return character,
                 * add it to the end of the currentLine
                 */
                *pCurrentLine += lChar;
            }
        }
    }

    return 0;
}
