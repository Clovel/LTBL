/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file Relay.hpp
 */

/* Includes -------------------------------------------- */

/* Defines --------------------------------------------- */

namespace elec {
    /* Type definitions -------------------------------- */
typedef enum _relayMode {
    RELAY_MODE_UNKNOWN = 0,
    RELAY_MODE_NORMAL,
    RELAY_MODE_INVERTED
} relayMode_t;

    typedef int gpioPin_t;
    typedef int gpioLevel_t;

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
            gpioLevel_t mState;
            relayMode_t mMode;

            gpioPin_t mPin;
    };
}