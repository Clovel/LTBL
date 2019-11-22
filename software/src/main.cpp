/**
 * @brief Main source file. Contains the main routines.
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

    /* End of setup */
    Serial.println("[BOOT ] System booted !");
}

/* Main loop routine */
void loop(void) {
    static unsigned long int i = 0U;

    /* Print the loop occurence we are currently executing */
    Serial.print("[DEBUG] Looping (");
    Serial.print(i++);
    Serial.println(")...");

    /* Turn the LED on */
    gRelay->switchState();

    /* Wait a little turning the LED back off */
    delay(250);
}
