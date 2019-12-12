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

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

/* Defines --------------------------------------------- */
#define LOG_BAUDRATE 9600

#define LED_DIO    D1
#define SWITCH_DIO D2

#define AP_NAME   "LTBL"
#define AP_PASSWD "TOTO"

/* Global variables ------------------------------------ */
elec::Relay  *gRelay   = nullptr;
elec::Switch *gSwitch  = nullptr;

WiFiManager  *gWiFiMgr = nullptr;
WiFiServer   *gServer  = nullptr;

/* On-boot routine */
void setup(void) {
    /* Set up the serial port for printing logs */
    Serial.begin(LOG_BAUDRATE);

    /* Init relay */
    gRelay = new elec::Relay(LED_DIO, elec::RELAY_MODE_NORMAL);

    /* Init Switch */
    gSwitch = new elec::Switch(SWITCH_DIO);

    /* Init WiFi manager */
    gWiFiMgr = new WiFiManager;
    if(!gWiFiMgr->autoConnect(AP_NAME, AP_PASSWD)) {
        Serial.println("[ERROR] Failed to connect to WiFi !");
        ESP.reset();
        delay(1000U);
    }
    Serial.print("[BOOT ] IPv4 Address : ");
    Serial.println(WiFi.localIP());

    /* Set up web server */
    gServer = new WiFiServer(80U);
    gServer->begin();

    /* End of setup */
    Serial.println("[BOOT ] System booted !");
}

/* Main loop routine */
void loop(void) {
    static bool              lChangeRelayState  = false;
    static bool              lOldRelayState     = gRelay->isOn();
    static int               lManualSwitchState = 0;

    /* Check manual switch state */
    if(lOldRelayState != (lManualSwitchState = gSwitch->isActive())) {
        /* Save new state */
        lOldRelayState = lManualSwitchState;

        /* Set the relay to be switched */
        lChangeRelayState = true;
    }

    /* Switch the LED's state */
    if(lChangeRelayState) {
        Serial.print("[DEBUG] Switching LED state : ");
        if(0 == lManualSwitchState) 
            Serial.println("OFF");
        else
            Serial.println("ON");
        
        /* Switch Relay state */
        gRelay->switchState();

        /* Set the relay to not be switched */
        lChangeRelayState = false;
    }

    /* Wait a little before looping back to avoid CPU overload */
    delay(10U);
}
