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

#ifdef UNIX
#include <iostream>
#endif /* UNIX */

/* Defines --------------------------------------------- */

/* Forward declarations -------------------------------- */

/* Print function -implementation ---------------------- */
template <typename T>
static size_t print(const T &pArg, const int8_t &pFormat = -1, const size_t &pSize = 0U) {
    (void)pSize;
    (void)pFormat;

    size_t lSize = 0U;

#ifdef ESP8266_NODEMCU
    lSize = Serial.print(pArg);
#endif /* ESP8266_NODEMCU */

#ifdef UNIX
    std::cout << pArg;

    lSize = sizeof(pArg);
#endif /* UNIX */

    return lSize;
}

template <typename T>
static size_t println(const T &pArg, const int8_t &pFormat = -1, const size_t &pSize = 0U) {
    (void)pSize;
    (void)pFormat;

    size_t lSize = 0U;

#ifdef ESP8266_NODEMCU
    lSize = Serial.println(pArg);
#endif /* ESP8266_NODEMCU */

#ifdef UNIX
    std::cout << pArg << std::endl;

    lSize = sizeof(pArg);
#endif /* UNIX */

    return lSize;
}

static void flush(void) {
#ifdef ESP8266_NODEMCU
    return Serial.flush();
#endif /* ESP8266_NODEMCU */

#ifdef UNIX
    std::cout << std::flush;
#endif /* UNIX */
}

/* LogLevel struct implementation ---------------------- */
LogLevel::LogLevel(const uint8_t &pLogLevel) : 
    level(pLogLevel) 
{
    /* Empty */
}

/* EndLog struct implementation ------------------------ */

/* FlushLog struct implementation ---------------------- */

/* Format struct implementation ------------------------ */
Format::Format(const int8_t &pFmt) : 
    type(pFmt)
{
    /* Empty */
}

/* Logger class implementation ------------------------- */
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
    static Logger gLogger;

    if(!gLogger.mInitialized) {
#ifdef ESP8266_NODEMCU
        Serial.begin(LOG_BAUDRATE);

        /* Wait for the Serial port to connect, needed for Leonardo only */
        while(!Serial);
#endif /* ESP8266_NODEMCU */

        gLogger.mInitialized = true;
    }

    return gLogger;
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
#ifdef ESP8266_NODEMCU
Logger &Logger::operator<<(const String &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        print(pArg);
    }
    
    return *this;
}
#endif /* ESP8266_NODEMCU */

Logger &Logger::operator<<(const std::string &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        print(pArg.c_str());
    }
    
    return *this;
}

Logger &Logger::operator<<(const char * const pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        print(pArg);
    }

    return *this;
}

Logger &Logger::operator<<(const char &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        print(pArg);
    }
    
    return *this;
}

Logger &Logger::operator<<(const unsigned char &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const int &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const unsigned int &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const long &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const unsigned long &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* DEC */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const double &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* BIN */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const float &pArg)
{
    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        if (mFormat > 0) {
            print(pArg, mFormat);
        } else {
            print(pArg); /* BIN */
        }
    }
    
    return *this;
}

Logger &Logger::operator<<(const LogLevel &pArg)
{
    mCurrentLogLevel = pArg.level;

    return *this;
}

Logger &Logger::operator<<(const EndLog &pArg)
{
    (void)pArg;

    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        println("");
    }
    mCurrentLogLevel = LOG_DEFAULT;
    mFormat = -1;

    return *this;
}

Logger &Logger::operator<<(const FlushLog &pArg)
{
    (void)pArg;

    if (mCurrentLogLevel > LOG_MIN &&
        mLogLevel >= mCurrentLogLevel) {
        flush();
    }
    mCurrentLogLevel = LOG_DEFAULT;
    mFormat = -1;

    return *this;
}

Logger &Logger::operator<<(const Format &pArg)
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
