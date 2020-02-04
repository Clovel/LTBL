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
        lOldPos = ++lPos;

        /* Find the position of the next '\n' */
        *gLogger << "[DEBUG] <split> Find the position of the next 'pDelim'" << endlog;
        lPos = pStr.find(pDelim, lPos);

        if(std::string::npos == lPos) {
            *gLogger << "[DEBUG] <split> lPos == npos" << endlog;
            break;
        }

        /* Extract the substring (lSub) */
        *gLogger << "[DEBUG] <split> Extract the substring (lSub)" << endlog;
        lSub = pStr.substr(lOldPos, lPos - lOldPos);

        *gLogger << "[DEBUG] <split> lSub = " << lSub << endlog;

        if(lSub.empty()) {
            *gLogger << "[DEBUG] <split> lSub.empty()" << endlog;
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

    std::istringstream lStrStream(pRequestStr);
    std::string lLine;

    while (std::getline(lStrStream, lLine)) {
        if(lLine.empty()) {
            break;
        }

        // Serial.print("[DEBUG] <HttpRequest::parseRequest> line n°");
        // Serial.print(lLines.size() + 1U);
        // Serial.print(" = ");
        // Serial.println(lLine.c_str());

        lLines.push_back(lLine);
    }

    /* Get Method, URL & HTTP version */
    {
        std::vector<std::string> lWords = split(lLines[0], ' ');

        mMethod         = lWords[0U];
        mURL            = lWords[1U];
        mHttpVersion    = lWords[2U];

        if (mURL.find('?') != std::string::npos) {
            mShortURL = mURL.substr(0U, mURL.find('?'));
            mQuery    = mURL.substr(mURL.find('?') + 1U);
        } else {
            mShortURL = mURL;
        }

        /* Print result for debugging purposes */
        /*
        std::cout << "[DEBUG] <HttpRequest::parseRequest> Line 1 : " << std::endl
            << "Method       : " << mMethod << std::endl
            << "URL          : " << mURL << std::endl
            << "Short URL    : " << mShortURL << std::endl
            << "Query        : " << mQuery << std::endl
            << "HTTP Version : " << mHttpVersion << std::endl
            << std::endl;
        */
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
