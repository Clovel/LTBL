/**
 * Let There Be Light project
 * 
 * @brief Contains callback functions for the WiFiManager.
 * 
 * @file wificallbacks.cpp
 */

/* Includes -------------------------------------------- */
#include "Logger.hpp"

#include <Arduino.h>

#include <WiFiManager.h> /* https://github.com/tzapu/WiFiManager WiFi Configuration Magic */
#include <ESP8266WiFi.h> /* ESP8266 Core WiFi Library */

/* External variables ---------------------------------- */
extern Logger *gLogger;

/* Static variables ------------------------------------ */
static bool shouldSaveConfig = false; /* Config save flag */

/* WiFiManager callbacks ------------------------------- */
void configModeCallback(WiFiManager *myWiFiManager) {
    *gLogger << "[BOOT ] <WiFiManager> Entered config mode" << endlog;
    *gLogger << WiFi.softAPIP().toString() << endlog;

    /* Print SSID */
    *gLogger << "[BOOT ] <WiFiManager> SSID : ";
    *gLogger << myWiFiManager->getConfigPortalSSID() << endlog;
}

/* Callback notifying us of the need to save config */
void saveConfigCallback (void) {
    *gLogger << "Should save config" << endlog;
    shouldSaveConfig = true;
}
