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

#include "wificallbacks.hpp"
#include "webtools.hpp"

#include "Logger.hpp"

#include "def.h"

#include <Arduino.h>

//#include <FS.h>     /* FileSystem header */

#include <ESP8266WiFi.h>          /* ESP8266 Core WiFi Library */
#include <DNSServer.h>            /* Local DNS Server used for redirecting all requests to the configuration portal */
#include <ESP8266WebServer.h>     /* Local WebServer used to serve the configuration portal */
#include <WiFiManager.h>          /* https://github.com/tzapu/WiFiManager WiFi Configuration Magic */

/* Defines --------------------------------------------- */
#define LED_DIO    D1
#define SWITCH_DIO D2

#define AP_NAME   "LTBL"
#define AP_PASSWD "TOTO"

/* Global variables ------------------------------------ */
elec::Relay  *gRelay   = nullptr;
elec::Switch *gSwitch  = nullptr;

WiFiManager  *gWiFiMgr = nullptr;
WiFiServer   *gServer  = nullptr;

Logger *gLogger = nullptr;

/* Static variables ------------------------------------ */
static std::string sRequest;

/* On-boot routine ------------------------------------- */
void setup(void) {
    /* Set up logger */
    gLogger = &Logger::instance();

    /* Init relay */
    gRelay = new elec::Relay(LED_DIO, elec::RELAY_MODE_NORMAL);

    /* Init Switch */
    gSwitch = new elec::Switch(SWITCH_DIO);

    /* Init WiFi manager */
    gWiFiMgr = new WiFiManager;

    /** Set callback that gets called when connecting 
     * to previous WiFi fails, 
     * and enters Access Point mode */
    gWiFiMgr->setAPCallback(configModeCallback);

    /* Disable debug mode for the WiFiManager */
    gWiFiMgr->setDebugOutput(false);

    /** Fetches SSID and password and tries to connect
     * if it does not connect it starts an access point with the specified name
     * (defined here by AP_PASSWD)
     * and goes into a blocking loop awaiting configuration
     */
    if(!gWiFiMgr->autoConnect(AP_NAME, AP_PASSWD)) {
        *gLogger << "[ERROR] Failed to connect to WiFi !" << endlog;
        ESP.reset();
        delay(1000U);
    }
    *gLogger << "[BOOT ] Successfully connected to " << WiFi.SSID() << endlog;
    *gLogger << "[BOOT ] IPv4 Address : " << WiFi.localIP().toString() << endlog;

    /* Set up web server */
    gServer = new WiFiServer(80U);
    gServer->begin();

    /* End of setup */
    *gLogger << "[BOOT ] System booted !" << endlog;
}

/* Main loop routine ----------------------------------- */
void loop(void) {
    static bool              lChangeRelayState  = false;
    static bool              lOldRelayState     = gRelay->isOn();
    static int               lManualSwitchState = 0;

    /* Listen for incoming web clients */
    WiFiClient lClient = gServer->available();

    if(0 < lClient) {
        *gLogger << "[DEBUG] New client, IP : " << lClient.localIP().toString() << endlog;

        std::string lCurrentLine = "";
        int lResult = 0;

        // /* Loop while the client is connected */
        // while(lClient.connected()) {
        //     /* Process request */
        //     lResult = acceptRequest(&lClient);
        //     if(0 != lResult) {
        //         Serial.println("[ERROR] Failed to process remote request w/ acceptRequest !");
        //     } else {
        //         Serial.println("[DEBUG] Processed acceptRequest successfully !");
        //     }
        // }

        lResult = testAccept(&lClient, &lCurrentLine, &sRequest);
        if(0 != lResult) {
            *gLogger << "[ERROR] Failed to process remote request w/ testAccept !" << endlog;
        } else {
            *gLogger << "[DEBUG] Processed testAccept successfully !" << endlog;
        }

        /* Clear the request string */
        sRequest = "";
    }

    /* Check manual switch state */
    if(lOldRelayState != (lManualSwitchState = gSwitch->isActive())) {
        /* Save new state */
        lOldRelayState = lManualSwitchState;

        /* Set the relay to be switched */
        lChangeRelayState = true;
    }

    /* Switch the LED's state */
    if(lChangeRelayState) {
        *gLogger << "[DEBUG] Switching LED state : " << (0 == lManualSwitchState ? "OFF" : "ON") << endlog;
        
        /* Switch Relay state */
        gRelay->switchState();

        /* Set the relay to not be switched */
        lChangeRelayState = false;
    }

    /* Wait a little before looping back to avoid CPU overload */
    delay(10U);
}
