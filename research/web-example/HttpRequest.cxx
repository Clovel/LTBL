/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file HttpRequest.cxx
 */

/* Includes -------------------------------------------- */
#include "HttpRequest.hxx"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */

/* Variable declarations ------------------------------- */

/* Functions ------------------------------------------- */
static void split(const std::string &pStr, const char pDelim, std::vector<std::string> &pWords) {
    std::stringstream lStringStream(pStr);
    std::string lWord;

    while (getline(lStringStream, lWord, pDelim)) {
        pWords.push_back(lWord);
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
        std::cerr << "[ERROR] <parseRequest> Request is empty, nothing to parse !" << std::endl;
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

        //std::cout << "[DEBUG] <HttpRequest::parseRequest> line n°" << lLines.size() + 1U << " = " << lLine << std::endl;

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
