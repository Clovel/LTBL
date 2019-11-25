/**
 * Let There Be Light project
 * 
 * @brief Main source file. Contains the main routines.
 * 
 * @file main.cpp
 */

/* Includes -------------------------------------------- */
#include "Relay.hpp"

#include <Arduino.h>

/* Defines --------------------------------------------- */

/* Global variables ------------------------------------ */
elec::Relay *gRelay = nullptr;

/* On-boot routine */
void setup(void) {
    /* Set up the serial port for printing logs */
    Serial.begin(9600);

    /* Init relay */
    gRelay = new elec::Relay(D1, elec::RELAY_MODE_NORMAL);

    /* Set up input for switch */
    pinMode(D2, INPUT_PULLUP);

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
    if(lOldRelayState != (lManualSwitchState = digitalRead(D2))) {
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

    /* Wait a little before turning the LED back off */
    //delay(250);
}
