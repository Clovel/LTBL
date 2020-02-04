/** 
 * Let There Be Light project
 * 
 * @file HttpRequest.hpp
 */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

/* Includes -------------------------------------------- */
#include <string>

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */
typedef enum _restMethods {
    REST_UNKNOWN = 0,
    REST_INDEX, /* Shows the index page */
    REST_GET, /* GET is used to request data from a specified resource. */
    REST_POST, /* POST is used to send data to a server to create/update a resource. */
    REST_PUT, /* PUT is used to send data to a server to create/update a resource. */
    REST_HEAD, /* HEAD is almost identical to GET, but without the response body. */
    REST_DELETE, /* The DELETE method deletes the specified resource. */
    REST_PATCH,
    REST_OPTIONS /* The OPTIONS method describes the communication options for the target resource. */
} restMethod_t;

typedef enum _parseErrors {
    HTTP_REQ_PARSE_ERROR_NONE = 0U,
    HTTP_REQ_PARSE_ERROR_METHOD,        /* Unknown REST method */
    HTTP_REQ_PARSE_ERROR_CONTENTS,      /*! Contents of the parsed requests are non-compliant */
} httpRequestParseError_t;

/* Variable declarations ------------------------------- */

/* Functions ------------------------------------------- */

/* Class definition ------------------------------------ */
class HttpRequest {
    public:
        /* Constructors */
        HttpRequest();

        /* Destructors */
        virtual ~HttpRequest();

        /* Request parser */
        httpRequestParseError_t parseRequest(const std::string &pRequest);

        /* Getters */
        std::string request(void) const;
        std::string method(void) const;
        std::string URL(void) const;
        std::string shortURL(void) const;
        std::string query(void) const;
        std::string httpVersion(void) const;

        /* Printer */
        void print(void);

    protected:
        std::string mRequestStr;
        
        std::string mMethod;
        std::string mURL;
        std::string mShortURL;
        std::string mQuery;
        std::string mHttpVersion;

        std::string mHost;
        std::string mConnectionType;
        unsigned int mUpgradeInsecureRequests;
        std::string mAccept;
        std::string mUserAgent;
        std::string mAccpetLanguage;
        std::string mAccpetEncoding;
    private:
};

#endif /* HTTP_REQUEST_HPP */
