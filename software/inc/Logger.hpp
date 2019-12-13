/** 
 * Let There Be Light project
 * 
 * @file Logger.hpp
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

/* Includes -------------------------------------------- */
#ifndef TESTS
/* Arduino */
#include <Arduino.h>
#endif /* TESTS */

/* C++ system */
#include <string>

/* Defines --------------------------------------------- */
#define LOG_MIN     0x0U
#define LOG_ERROR   0x1U
#define LOG_DEFAULT 0x2U
#define LOG_WARN    0x3U
#define LOG_APPL    0x4U
#define LOG_VERBOSE 0x5U
#define LOG_DEBUG   0x6U

#ifdef TESTS
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#endif /* TESTS */

/* Forward declarations -------------------------------- */

/* Type definitions ------------------------------------ */

/**
 * @brief Marker to start log message
 */
struct LogLevel {
    uint8_t level;
    LogLevel(const uint8_t &pLogLevel);
}; 

/**
 * @brief Marker to end a log message
 * (prints newline and resets log level)
 */
struct EndLog {};
const EndLog endlog = EndLog();
 
/**
 * @brief Marker to end a log message
 * (flushes output stream and resets log level)
 */
struct FlushLog {};
const FlushLog flushlog = FlushLog();
 
/**
 * @brief Marker to modify output stream
 */
struct Format {
    int8_t type;
    Format(const int8_t &pFmt = -1);
};

/* Class declaration ----------------------------------- */
class Logger {
    public:
        /* Destructor */
        virtual ~Logger();

        /** @fn static Logger &instance(void)
         * @brief Returns singleton instance of Logger
         * 
         * NOTE: pin 1 reserved
         * NOTE: if Logger is used as GLOBAL variable, the initialization
         * code will fail to properly set Serial object, thus
         * you will have to add proper code in your Setup()
         * method to make it work (e.g. Serial.begin(4800);)
         */
        static Logger &instance(void);

        /* Getters */
        uint8_t logLevel(void) const;

        /* Setters */
        void setLogLevel(const uint8_t &pLogLevel);

        /* Public operators */
#ifndef TESTS
        Logger& operator<<(const String &pArg);
#endif /* TESTS */
        Logger& operator<<(const std::string &pArg);
        Logger& operator<<(const char * const pArg);
        Logger& operator<<(const char &pArg);
        Logger& operator<<(const unsigned char &pArg);
        Logger& operator<<(const int &pArg);
        Logger& operator<<(const unsigned int &pArg);
        Logger& operator<<(const long &pArg);
        Logger& operator<<(const unsigned long &pArg);
        Logger& operator<<(const double &pArg);
        Logger& operator<<(const float &pArg);
    
        Logger& operator<<(const LogLevel &pArg);
        Logger& operator<<(const EndLog &pArg);
        Logger& operator<<(const FlushLog &pArg);
        Logger& operator<<(const Format &pArg);

        /* alternative to << loglevel(value) */
        inline Logger &log(const uint8_t &pLogLevel = LOG_DEFAULT);
    protected:
    private:
        /* Constructors */
        Logger();
        Logger(const Logger &pLogger);

        /* Private operators */
        Logger &operator=(const Logger &pLogger);

        /* Private members */
        uint8_t mLogLevel;          /* Logger log level */
        uint8_t mCurrentLogLevel;   /* Current message log level */
        int8_t  mFormat;            /* Format with which the Logger prints data */
        bool    mInitialized;       /* Is the logger initialized ? */
};

#endif /* LOGGER_HPP */
