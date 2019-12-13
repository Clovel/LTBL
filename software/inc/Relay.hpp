/** 
 * Let There Be Light project
 * 
 * @brief Relay class definition
 * 
 * @file Relay.hpp
 */

#ifndef ELEC_RELAY_HPP
#define ELEC_RELAY_HPP

/* Includes -------------------------------------------- */
#include "elec.hpp"

/* Defines --------------------------------------------- */

namespace elec {
    /* Type definitions -------------------------------- */
    typedef enum _relayMode {
        RELAY_MODE_UNKNOWN = 0,
        RELAY_MODE_NORMAL,
        RELAY_MODE_INVERTED
    } relayMode_t;

    /* Class definition -------------------------------- */
    class Relay {
        public:
            Relay(const gpioPin_t &pPin, const relayMode_t &pMode = RELAY_MODE_UNKNOWN);
            virtual ~Relay();

            /* Getters */
            bool isOn(void) const;
            relayMode_t mode(void) const;

            /* Setters */
            void setMode(const relayMode_t &pMode);

            /* Actions */
            void turnOn(void);
            void turnOff(void);
            void switchState(void);

        protected:
        private:
            gpioPin_t mPin;
            relayMode_t mMode;
            gpioLevel_t mState;

    };
}

#endif /* ELEC_RELAY_HPP */
