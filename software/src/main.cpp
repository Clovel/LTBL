/**
 * @brief Main source file. Contains the main routines.
 */

#include <Arduino.h>

#define LED_GREEN_PIN D1

/* On-boot routine */
void setup(void) {
    /* Set up the serial port for printing logs */
    Serial.begin(9600);

    /* Set up LED pin */
    pinMode(LED_GREEN_PIN, OUTPUT);

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
    digitalWrite(LED_GREEN_PIN, HIGH);

    /* Wait a little turning the LED back off */
    delay(250);

    digitalWrite(LED_GREEN_PIN, LOW);

    /* Wait a little before looping back */
    delay(250);
}
