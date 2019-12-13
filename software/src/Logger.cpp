/** 
 * Let There Be Light project
 * 
 * @file Logger.cpp
 */

/* Includes -------------------------------------------- */
#include "Logger.hpp"

#include "def.h"

/* C++ System */
#include <sstream>

#ifdef TESTS
#include <iostream>
#endif /* TESTS */

/* Defines --------------------------------------------- */

/* Forward declarations -------------------------------- */

/* Print function -implementation ---------------------- */
template <typename T>
static size_t print(const T &pArg, const size_t &pSize) {
    (void)pSize;

#ifndef TESTS
    return Serial.print(*pArg);
#else /* TESTS */
    std::cout << pArg;

    return sizeof(pArg);
#endif /* TESTS */
}

template <typename T>
static size_t println(const T &pArg, const size_t &pSize) {
    (void)pSize;

#ifndef TESTS
    return Serial.println(*pArg);
#else /* TESTS */
    std::cout << pArg << std::endl;

    return sizeof(pArg);
#endif /* TESTS */
}

/* LogLevel struct impletentation ---------------------- */
LogLevel::LogLevel(const uint8_t &pLogLevel) : 
    level(pLogLevel) 
{
    /* Empty */
}

/* EndLog struct impletentation ------------------------ */

/* FlushLog struct impletentation ---------------------- */

/* Format struct impletentation ------------------------ */
Format::Format(const int8_t &pFmt) : 
    type(pFmt)
{
    /* Empty */
}

/* Logger class implementation ------------------------- */
#ifndef TESTS

/* Constructors */
Logger::Logger() : 
    mLogLevel(LOG_DEFAULT),
    mCurrentLogLevel(LOG_DEFAULT),
    mFormat(-1),
    mInitialized(false)
{
    /* Empty */
}

/* Destructor */
Logger::~Logger() {
    /* Empty */
}

/* Instance getter */
Logger &Logger::instance(void) {
    static Logger sLogger;

    if(!sLogger.mInitialized) {
        Serial.begin(LOG_BAUDRATE);

        /* Wait for the Serial port to connect, needed for Leonardo only */
        while(!Serial);

        sLogger.mInitialized = true;
    }

    return sLogger;
}

/* Getters */
uint8_t Logger::logLevel(void) const {
    return mLogLevel;
}

/* Setters */
void Logger::setLogLevel(const uint8_t &pLogLevel) {
    mLogLevel = pLogLevel;
}

inline Logger &Logger::log(const uint8_t &pLogLevel) {
    *this << LogLevel(pLogLevel);

    return *this;
}

/* Operator implementation ----------------------------- */
#ifndef TESTS
Logger& Logger::operator<<(const String &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.print(pArg);
    }
    
    return *this;
}
#endif /* TESTS */

Logger& Logger::operator<<(const std::string &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.print(pArg.c_str());
    }
    
    return *this;
}

Logger& Logger::operator<<(const char * const pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.print(pArg);
    }

    return *this;
}

Logger& Logger::operator<<(const char &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.print(pArg);
    }
    
    return *this;
}

Logger& Logger::operator<<(const unsigned char &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const int &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const unsigned int &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const long &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const unsigned long &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const double &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* BIN */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const float &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            Serial.print(pArg, mFormat);
        } else {
            Serial.print(pArg); /* BIN */
        }
    }
    
    return *this;
}

Logger& Logger::operator<<(const LogLevel &pArg)
{
    mCurrentLogLevel = pArg.level;

    return *this;
}

Logger& Logger::operator<<(const EndLog &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.println("");
    }
    mCurrentLogLevel = LOG_DEFAULT;
    mFormat = -1;

    return *this;
}

Logger& Logger::operator<<(const FlushLog &pArg)
{
    if (mCurrentLogLevel > LOG_DEFAULT &&
        mLogLevel >= mCurrentLogLevel) {
        Serial.flush();
    }
    mCurrentLogLevel = LOG_DEFAULT;
    mFormat = -1;

    return *this;
}

Logger& Logger::operator<<(const Format &pArg)
{
    switch (pArg.type)
    {
        case DEC:
        case HEX:
        case OCT:
        case BIN:
            mFormat = pArg.type;
            break;
        default:
            mFormat = -1;
            break;
    }
    
    return *this;
}

#endif /* TESTS */
