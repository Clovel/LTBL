/** 
 * Let There Be Light project
 * 
 * @brief Relay class implementation
 * 
 * @file Relay.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"

#ifndef TESTS
#include <Arduino.h>
#else /* TESTS */
#include <iostream>

extern void digitalWrite(int pPin, int pLevel);
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
    Relay::Relay(const gpioPin_t &pPin, const relayMode_t &pMode) : 
        mPin(pPin),
        mMode(pMode)
    {
        pinMode(mPin, OUTPUT);
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                mState = LOW;
                digitalWrite(mPin, LOW);
                break;
            case RELAY_MODE_INVERTED:
                mState = HIGH;
                digitalWrite(mPin, HIGH);
                break;
            default:
#ifndef TESTS
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it off !");
#else /* TESTS */
                std::cout << "[ERROR] Unknown Relay mode, cannot turn it off !" << std::endl;
#endif /* TESTS */
                break;
        }
    }

    Relay::~Relay() {
        /* Empty */
    }

    /* Getters */
    bool Relay::isOn(void) const {
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                return mState;
            case RELAY_MODE_INVERTED:
                return !mState;
            default:
#ifndef TESTS
                Serial.println("[ERROR] Unknown relay mode, cannot know if it is on or off !");
#else /* TESTS */
                std::cout << "[ERROR] Unknown relay mode, cannot know if it is on or off !" << std::endl;
#endif /* TESTS */
                return false;
                break;
        }
    }

    relayMode_t Relay::mode(void) const {
        return mMode;
    }

    /* Setters */
    void Relay::setMode(const relayMode_t &pMode) {
        mMode = pMode;
    }

    /* Actions */
    void Relay::turnOn(void) {
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                mState = HIGH;
                digitalWrite(mPin, HIGH);
                break;
            case RELAY_MODE_INVERTED:
                mState = LOW;
                digitalWrite(mPin, LOW);
                break;
            default:
#ifndef TESTS
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it on !");
#else /* TESTS */
                std::cout << "[ERROR] Unknown Relay mode, cannot turn it on !" << std::endl;
#endif /* TESTS */
                break;
        }
    }

    void Relay::turnOff(void) {
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                mState = LOW;
                digitalWrite(mPin, LOW);
                break;
            case RELAY_MODE_INVERTED:
                mState = HIGH;
                digitalWrite(mPin, HIGH);
                break;
            default:
#ifndef TESTS
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it off !");
#else /* TESTS */
                std::cout << "[ERROR] Unknown Relay mode, cannot turn it off !" << std::endl;
#endif /* TESTS */
                break;
        }
    }

    void Relay::switchState(void) {
        /* We don't realy care for the Relay mode here, 
         * We just change the state
         */
        mState = !mState;
        digitalWrite(mPin, mState);

        Serial.print("[DEBUG] <Relay::switchState> Switching to ");
        Serial.println(mState);
    }
}
