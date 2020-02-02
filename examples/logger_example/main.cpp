/**
 * Let There Be Light project
 * 
 * @brief Main source file of the Logger example. 
 * 
 * @file main.cpp
 */

/* Includes -------------------------------------------- */
#include "Logger.hpp"

/* Defines --------------------------------------------- */

/* Global variables ------------------------------------ */
//static Logger &sLogger = Logger::instance();

/* main ------------------------------------------------ */
int main(const int argc, const char * const * const argv) {
    (void)argc;
    (void)argv;

    Logger &lLogger = Logger::instance();

    lLogger << "[TEST ] Testing logger capabilities !" << endlog;

    return 0;
}
