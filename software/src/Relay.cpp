/** 
 * Let There Be Light project
 * 
 * @brief Relay class implementation
 * 
 * @file Relay.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"

#include "Logger.hpp"

#ifndef TESTS
#include <Arduino.h>
#else /* TESTS */

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
extern Logger *gLogger;

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
                *gLogger << "[ERROR] Unknown Relay mode, cannot turn it off !" << endlog;
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
                *gLogger << "[ERROR] Unknown relay mode, cannot know if it is on or off !" << endlog;
                return false;
        }
    }

    relayMode_t Relay::mode(void) const {
        return mMode;
    }

    std::string Relay::stringState(void) const {
        return 0 == mState ? "OFF" : "ON";
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
                *gLogger << "[ERROR] Unknown Relay mode, cannot turn it on !" << endlog;
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
                *gLogger << "[ERROR] Unknown Relay mode, cannot turn it off !" << endlog;
                break;
        }
    }

    void Relay::switchState(void) {
        /* We don't realy care for the Relay mode here, 
         * We just change the state
         */
        mState = !mState;
        digitalWrite(mPin, mState);

        *gLogger << "[DEBUG] <Relay::switchState> Switching to " << mState << endlog;
    }
}
