/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file Relay.cxx
 */

/* Includes -------------------------------------------- */
#include "Relay.hxx"

/* C++ System */
#include <iostream>

/* Defines --------------------------------------------- */

/* Variable declarations ------------------------------- */

/* Class implementation -------------------------------- */

namespace elec {
    Relay::Relay(const relayMode_t &pMode) : 
        mMode(pMode)
    {
        /* Empty */
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
                std::cout << "[ERROR] Unknown relay mode, cannot know if it is on or off !" << std::endl;
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
                mState = true;
                break;
            case RELAY_MODE_INVERTED:
                mState = false;
                break;
            default:
                std::cout << "[ERROR] Unknown Relay mode, cannot turn it on !" << std::endl;
                break;
        }
    }

    void Relay::turnOff(void)
    {
        switch(mMode) {
            case RELAY_MODE_NORMAL:
                mState = false;
                break;
            case RELAY_MODE_INVERTED:
                mState = true;
                break;
            default:
                std::cout << "[ERROR] Unknown Relay mode, cannot turn it off !" << std::endl;
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