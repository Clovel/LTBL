/**
 * Let There Be Light project
 * 
 * @brief Contains callback functions for the WiFiManager.
 * 
 * @file wificallbacks.cpp
 */

/* Includes -------------------------------------------- */
#include <Arduino.h>

#include <WiFiManager.h> /* https://github.com/tzapu/WiFiManager WiFi Configuration Magic */
#include <ESP8266WiFi.h> /* ESP8266 Core WiFi Library */

/* Static variables ------------------------------------ */
static bool shouldSaveConfig = false; /* Config save flag */

/* WiFiManager callbacks ------------------------------- */
void configModeCallback(WiFiManager *myWiFiManager) {
    Serial.println("[BOOT ] <WiFiManager> Entered config mode");
    Serial.println(WiFi.softAPIP());

    /* Print SSID */
    Serial.print("[BOOT ] <WiFiManager> SSID : ");
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

/* Callback notifying us of the need to save config */
void saveConfigCallback (void) {
    Serial.println("Should save config");
    shouldSaveConfig = true;
}
