/** 
 * Let There Be Light project
 * 
 * @brief Switch class implementation
 * 
 * @file Switch.cpp
 */

/* Includes -------------------------------------------- */
#include "Switch.hpp"

#ifndef TESTS
#include <Arduino.h>
#else /* TESTS */
#include <iostream>

extern void digitalRead(int pPin);
extern void pinMode(int pPin, int pMode);
#endif /* TESTS */

/* Defines --------------------------------------------- */
#ifndef LOW
#define LOW 0
#endif /* LOW */

#ifndef HIGH
#define HIGH 1
#endif /* HIGH */

#ifndef INPUT
#define INPUT 0x0
#endif /* INPUT */

#ifndef OUTPUT
#define OUTPUT 0x1
#endif /* OUTPUT */

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif /* INPUT_PULLUP */

/* Variable declarations ------------------------------- */

/* Class implementation -------------------------------- */
namespace elec {
    Switch::Switch(const gpioPin_t &pPin) : mPin(pPin)
    {
        pinMode(mPin, INPUT_PULLUP);
    }

    Switch~Switch() {
        /* Empty */
    }

    /* Getters */
    bool Switch::isActive(void) const {
        return digitalRead(mPin);
    }
}
