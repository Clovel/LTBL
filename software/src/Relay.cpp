/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file Relay.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"

#include <Arduino.h>

/* Defines --------------------------------------------- */

/* Variable declarations ------------------------------- */

/* Class implementation -------------------------------- */

namespace elec {
    Relay::Relay(const gpioPin_t &pPin, const relayMode_t &pMode) : 
        mPin(pPin),
        mMode(pMode)
    {
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
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it off !");
                break;
        }
    }

    Relay::~Relay()
    {
        /* Empty */
    }

    /* Getters */
    bool Relay::isOn(void) const
    {
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                return mState;
            case RELAY_MODE_INVERTED:
                return !mState;
            default:
                Serial.println("[ERROR] Unknown relay mode, cannot know if it is on or off !");
                return false;
                break;
        }
    }

    relayMode_t Relay::mode(void) const
    {
        return mMode;
    }

    /* Setters */
    void Relay::setMode(const relayMode_t &pMode)
    {
        mMode = pMode;
    }

    /* Actions */
    void Relay::turnOn(void)
    {
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
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it on !");
                break;
        }
    }

    void Relay::turnOff(void)
    {
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
                Serial.println("[ERROR] Unknown Relay mode, cannot turn it off !");
                break;
        }
    }

    void Relay::switchState(void)
    {
        /* We don't realy care for the Relay mode here, 
         * We just change the state
         */
        mState = !mState;
    }
}