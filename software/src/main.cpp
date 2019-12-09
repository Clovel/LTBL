/**
 * Let There Be Light project
 * 
 * @brief Main source file. Contains the main routines.
 * 
 * @file main.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"
#include "Switch.hpp"

#include <Arduino.h>

/* Defines --------------------------------------------- */
#define LOG_BAUDRATE 9600

#define LED_DIO    D1
#define SWITCH_DIO D2

/* Global variables ------------------------------------ */
elec::Relay  *gRelay  = nullptr;
elec::Switch *gSwitch = nullptr;

/* On-boot routine */
void setup(void) {
    /* Set up the serial port for printing logs */
    Serial.begin(LOG_BAUDRATE);

    /* Init relay */
    gRelay = new elec::Relay(LED_DIO, elec::RELAY_MODE_NORMAL);

    /* Init Switch */
    gSwitch = new elec::Switch(SWITCH_DIO);

    /* End of setup */
    Serial.println("[BOOT ] System booted !");
}

/* Main loop routine */
void loop(void) {
    static unsigned long int i                  = 0U;
    static bool              lChangeRelayState  = false;
    static bool              lOldRelayState     = gRelay->isOn();
    static int               lManualSwitchState = 0;

    /* Print the loop occurence we are currently executing */
    Serial.print("[DEBUG] Looping (");
    Serial.print(i++);
    Serial.println(")...");

    /* Check manual switch state */
    if(lOldRelayState != (lManualSwitchState = gSwitch->isActive())) {
        /* Save new state */
        lOldRelayState = lManualSwitchState;

        /* Set the relay to be switched */
        lChangeRelayState = true;
    }

    /* Switch the LED's state */
    if(lChangeRelayState) {
        gRelay->switchState();

        /* Set the relay to not be switched */
        lChangeRelayState = false;
    }

    /* Wait a little before looping back to avoid CPU overload */
    delay(10);
}

