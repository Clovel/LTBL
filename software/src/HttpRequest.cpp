/** 
 * Let There Be Light project
 * 
 * @file HttpRequest.cpp
 */

/* Includes -------------------------------------------- */
#include "HttpRequest.hpp"

#include "Logger.hpp"

#include <string>
#include <vector>
#include <sstream>

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */

/* Variable declarations ------------------------------- */
extern Logger *gLogger;

/* Functions ------------------------------------------- */
static void split(const std::string &pStr, const char pDelim, std::vector<std::string> &pWords) {
    size_t lPos = 0U, lOldPos = 0U;
    while(std::string::npos != lPos) {
        std::string lSub;

        /* Save the old position in the string */
        lOldPos = lPos;

        /* Find the position of the next '\n' */
        lPos = pStr.find(pDelim, lPos);

        /* Extract the substring (lSub) */
        lSub = pStr.substr(lOldPos, lPos - lOldPos);

        if(std::string::npos != lPos) {
            ++lPos;
        }

        if(lSub.empty()) {
            break;
        }

        pWords.push_back(lSub);
    }
}

static std::vector<std::string> split(const std::string &pStr, char pDelim) {
    std::vector<std::string> lWords;

    split(pStr, pDelim, lWords);

    return lWords;
}

static std::string getStrAfterDelim(const std::string &pStr, char pDelim) {
    size_t lDelimPos = pStr.find(pDelim);
    size_t lEOLPos   = pStr.find('\n');

    if(std::string::npos != lEOLPos) {
        ++lEOLPos;
    }

    std::string lSub = pStr.substr(lDelimPos, lEOLPos - lDelimPos);

    return lSub;
}

static std::string getStrBeforeDelim(const std::string &pStr, char pDelim) {
    return pStr.substr(0U, pStr.find(pDelim));
}

static void removeChar(std::string &pStr, const char pChar) {
    size_t lPos = 0U;

    while(std::string::npos != lPos) {
        if(std::string::npos == (lPos = pStr.find(pChar))) {
            break;
        }
        pStr = pStr.substr(0U, lPos) + pStr.substr(lPos + 1U);
    }
}

static void removeFirstChar(std::string &pStr, const char pChar) {
    if(0U == pStr.find(pChar)) {
        pStr = pStr.substr(1U);
    }
}

static void removeTrailingChar(std::string &pStr, const char pChar) {
    if(pChar == pStr[pStr.size() - 1]) {
        pStr = pStr.substr(0U, pStr.size() - 1);
    }
}

static void removeChar(std::vector<std::string> &pStrs, const char pChar) {
    for(size_t i = 0U; i < pStrs.size(); ++i) {
        removeChar(pStrs[i], pChar);
    }
}

static void removeCR(std::string &pStr) {
    removeChar(pStr, '\r');
}

static void removeCR(std::vector<std::string> &pStrs) {
    for(size_t i = 0U; i < pStrs.size(); ++i) {
        removeCR(pStrs[i]);
    }
}

/* Class implementation -------------------------------- */
HttpRequest::HttpRequest() {
    /* Empty */
}

HttpRequest::~HttpRequest() {
    /* Empty */
}

httpRequestParseError_t HttpRequest::parseRequest(const std::string &pRequestStr) {
    std::vector<std::string> lLines;

    if(pRequestStr.empty()) {
        *gLogger << "[ERROR] <parseRequest> Request is empty, nothing to parse !" << endlog;
        return HTTP_REQ_PARSE_ERROR_CONTENTS;
    }

    /* Set full request in class */
    mRequestStr = pRequestStr;

    /* Clean the request string */
    removeCR(mRequestStr);

    lLines = split(mRequestStr, '\n');

    if(lLines.empty()) {
        /* Got nothing, returning error */
        *gLogger << "[ERROR] <parseRequest> Got 0 lines when parsing." << endlog;
        return HTTP_REQ_PARSE_ERROR_CONTENTS;
    }

    /* Get Method, URL & HTTP version */
    {
        std::vector<std::string> lWords = split(lLines[0U], ' ');

        if(3U != lWords.size()) {
            *gLogger << "[ERROR] <parseRequest> Failed to parse Method, URL & HTTP version" << endlog;
            return HTTP_REQ_PARSE_ERROR_CONTENTS;
        }

        mMethodStr = lWords[0U];
        /* What method is it ? */
        if(0 == strcasecmp("GET", mMethodStr.c_str())) {
            mMethod = REST_GET;
        } else if (0 == strcasecmp("POST", mMethodStr.c_str())) {
            mMethod = REST_POST;
        } else if (0 == strcasecmp("PUT", mMethodStr.c_str())) {
            mMethod = REST_PUT;
        } else if (0 == strcasecmp("HEAD", mMethodStr.c_str())) {
            mMethod = REST_HEAD;
        } else if (0 == strcasecmp("DELETE", mMethodStr.c_str())) {
            mMethod = REST_DELETE;
        } else if (0 == strcasecmp("PATCH", mMethodStr.c_str())) {
            mMethod = REST_PATCH;
        } else if (0 == strcasecmp("OPTIONS", mMethodStr.c_str())) {
            mMethod = REST_OPTIONS;
        } else {
            *gLogger << "[ERROR] <parseRequest> Unknown REST method requested ! " << endlog;
            return HTTP_REQ_PARSE_ERROR_METHOD;
        }

        mURL            = lWords[1U];
        mHttpVersion    = lWords[2U];

        if (mURL.find('?') != std::string::npos) {
            mShortURL = mURL.substr(0U, mURL.find('?'));
            mQuery    = mURL.substr(mURL.find('?') + 1U);
        } else {
            mShortURL = mURL;
        }
    }

    /* Starting for loop at 1, we already parsed the first line */
    for(unsigned short int i = 1U; i < lLines.size(); ++i) {
        //*gLogger << "[DEBUG] <parseRequest> lLines[" << i << "] = \"" << lLines[i] << "\"" << endlog;

        if(lLines[i].empty()) {
            continue;
        }

        std::string lKey = getStrBeforeDelim(lLines[i], ' ');
        std::string lVal = getStrAfterDelim(lLines[i], ' ');

        /* Remove trailing ":" of the key if there is one */
        removeTrailingChar(lKey, ':');
        /* Remove first space of the value if there is one */
        removeFirstChar(lVal, ' ');

        //*gLogger << "[DEBUG] <parseRequest> lKey = " << lKey << ", lVal = " << lVal << endlog;

        /* Set the HttpRequest attributes */
        if("Host" == lKey) {
            mHost = lVal;
        } else if("Upgrade-Insecure-Requests" == lKey) {
            mUpgradeInsecureRequests = std::atoi(lVal.c_str());
        } else if("Accept" == lKey) {
            mAccept = lVal;
        } else if("User-Agent" == lKey) {
            mUserAgent = lVal;
        } else if("Accept-Language" == lKey) {
            mAccpetLanguage = lVal;
        } else if("Accept-Encoding" == lKey) {
            mAccpetEncoding = lVal;
        } else if("Connection" == lKey) {
            mConnectionType = lVal;
        } else if("Referer" == lKey) {
            mReferer = lVal;
        } else {
            *gLogger << "[WARN ] Unknown key parsed in the HTTP request (key: " << lKey << ", val: " << lVal << ")" << endlog;
            /* TODO : Should we return an error here ? */
        }
    }

    return HTTP_REQ_PARSE_ERROR_NONE;
}

/* Getters */
std::string HttpRequest::request(void) const {
    return mRequestStr;
}

restMethod_t HttpRequest::method(void) const {
    return mMethod;
}

std::string HttpRequest::methodStr(void) const {
    return mMethodStr;
}

std::string HttpRequest::URL(void) const {
    return mURL;
}

std::string HttpRequest::shortURL(void) const {
    return mShortURL;
}

std::string HttpRequest::query(void) const {
    return mQuery;
}

std::string HttpRequest::httpVersion(void) const {
    return mHttpVersion;
}

/* Printer */
void HttpRequest::print(void) {
    *gLogger << "[DEBUG] <HttpRequest::parseRequest> PRINT" << endlog
            << "Method          : " << mMethod << endlog
            << "URL             : " << mURL << endlog
            << "Short URL       : " << mShortURL << endlog
            << "Query           : " << mQuery << endlog
            << "HTTP Version    : " << mHttpVersion << endlog
            << "Host            : " << mHost << endlog
            << "Connection Type : " << mConnectionType << endlog
            << "U-I-Requests    : " << mUpgradeInsecureRequests << endlog
            << "Accept          : " << mAccept << endlog
            << "User Agent      : " << mUserAgent << endlog
            << "Accept Language : " << mAccpetLanguage << endlog
            << "Accpet Encoding : " << mAccpetEncoding << endlog
            << endlog;
}
