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
        
        if(std::string::npos != lPos) {
            ++lPos;
        }

        /* Extract the substring (lSub) */
        lSub = pStr.substr(lOldPos, lPos - lOldPos);

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

/* Class implementation -------------------------------- */
HttpRequest::HttpRequest() {
    /* Empty */
}

HttpRequest::~HttpRequest() {
    /* Empty */
}

int HttpRequest::parseRequest(const std::string &pRequestStr) {
    std::vector<std::string> lLines;

    if(pRequestStr.empty()) {
        *gLogger << "[ERROR] <parseRequest> Request is empty, nothing to parse !" << endlog;
        return -1;
    }

    /* Set full request in class */
    mRequestStr = pRequestStr;

    lLines = split(pRequestStr, '\n');

    if(lLines.empty()) {
        /* Got nothing, returning error */
        *gLogger << "[ERROR] <parseRequest> Got 0 lines when parsing." << endlog;
        return 1;
    }

    /* Get Method, URL & HTTP version */
    {
        std::vector<std::string> lWords = split(lLines[0U], ' ');

        if(3U != lWords.size()) {
            *gLogger << "[ERROR] <parseRequest> Failed to parse Method, URL & HTTP version" << endlog;
            return 1;
        }
        
        mMethod         = lWords[0U];
        mURL            = lWords[1U];
        mHttpVersion    = lWords[2U];

        if (mURL.find('?') != std::string::npos) {
            mShortURL = mURL.substr(0U, mURL.find('?'));
            mQuery    = mURL.substr(mURL.find('?') + 1U);
        } else {
            mShortURL = mURL;
        }

    }

    return 0;
}

/* Getters */
std::string HttpRequest::request(void) const {
    return mRequestStr;
}

std::string HttpRequest::method(void) const {
    return mMethod;
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
    *gLogger << "[DEBUG] <HttpRequest::parseRequest> Line 1 : " << endlog
            << "Method       : " << mMethod << endlog
            << "URL          : " << mURL << endlog
            << "Short URL    : " << mShortURL << endlog
            << "Query        : " << mQuery << endlog
            << "HTTP Version : " << mHttpVersion << endlog
            << endlog;
}
