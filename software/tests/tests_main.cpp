/**
 * @brief Main testing file
 */

/* Includes -------------------------------------------- */
/* Gaussian */
#include "tests_Relay.hpp"

/* System */
#include <iostream>

/* C System */
#include <cstring>

/* Support functions ----------------------------------- */
static void print_usage(const char * const pProgName)
{
    std::cout << "[USAGE] " << pProgName << " <test#>" << std::endl;
}

/* Main ------------------------------------------------ */
int main(const int argc, const char * const * const argv) {
    /* Test function initialization */
    int32_t lTestNum;
    int16_t lResult = EXIT_SUCCESS;

    if ((2 > argc) || (0 == std::strcmp(argv[1], "--help"))) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    lTestNum = strtol(argv[1], NULL, 10);
    std::cout << "[TEST ] Executing test #" << lTestNum << std::endl;

    /* Executing test */
    switch (lTestNum) {
        case -1:
            std::cout << "[INFO ] Default test." << std::endl;
        case 0:
            lResult = test_Relay_isOn()
            break;
        default:
            std::cout << "[INFO ] test #" << lTestNum << " not available" << std::endl;
            fflush(stdout);
            lResult = EXIT_FAILURE;
            break;
    }

    return lResult;
}
