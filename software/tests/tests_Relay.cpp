/**
 * @brief UTs for the Relay class
 */

/* Includes -------------------------------------------- */
#include "tests_Relay.hpp"
#include "Relay.hpp"

/* Tests ----------------------------------------------- */
int test_Relay_isOn(void) {
    /* Create normal relay */
    elec::Relay lRelay(13, elec::RELAY_MODE_NORMAL);

    /* Check the relay state */
    bool lState = lRelay.isOn();
    assert(false == lState);

    return EXIT_SUCCESS;
}

int test_Relay_switchState(void) {
    /* Create normal relay */
    elec::Relay lRelay(13, elec::RELAY_MODE_NORMAL);

    /* Check the relay state */
    bool lState = lRelay.isOn();
    assert(false == lState);

    /* Switch state */
    lRelay.switchState();
    
    /* Check the relay state */
    lState = lRelay.isOn();
    assert(true == lState);

    return EXIT_SUCCESS;
}

int test_Relay_turnOn(void) {
    /* Create normal relay */
    elec::Relay lRelay(13, elec::RELAY_MODE_NORMAL);

    /* Check the relay state */
    bool lState = lRelay.isOn();
    assert(false == lState);

    /* TurnOn */
    lRelay.turnOn();
    
    /* Check the relay state */
    lState = lRelay.isOn();
    assert(true == lState);

    return EXIT_SUCCESS;
}

int test_Relay_turnOff(void) {
    /* Create normal relay */
    elec::Relay lRelay(13, elec::RELAY_MODE_NORMAL);

    /* Check the relay state */
    bool lState = lRelay.isOn();
    assert(false == lState);

    /* TurnOn */
    lRelay.turnOn();
    
    /* Check the relay state */
    lState = lRelay.isOn();
    assert(true == lState);

    /* TurnOff */
    lRelay.turnOn();
    
    /* Check the relay state */
    lState = lRelay.isOff();
    assert(false == lState);

    return EXIT_SUCCESS;
}

int test_Relay_setMode(void) {
    /* Create normal relay */
    elec::Relay lRelay(13, elec::RELAY_MODE_NORMAL);

    /* Get mode */
    relayMode_t lMode = lRelay.mode();
    assert(elec::RELAY_MODE_NORMAL == lMode);

    /* Set other mode */
    lRelay.setMode(elec::RELAY_MODE_INVERTED);

    /* Get mode */
    lMode = lRelay.mode();
    assert(elec::RELAY_MODE_INVERTED == lMode);

    return EXIT_SUCCESS;
}
