/** 
 * Let There Be Light project
 * 
 * @file HttpRequest.hpp
 */

/* Includes -------------------------------------------- */
#include <string>

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */

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
        int parseRequest(const std::string &pRequest);

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
