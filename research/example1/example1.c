#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "said.";
const char* password = "secret";
ESP8266WebServer server ( 80 );
const int led = 13;
const int outputLed = 12;

void turnOnRelayOne(){
  digitalWrite (outputLed, 1);
  digitalWrite (5, HIGH); //GPIO 5 // Relay 1
  server.send (200, "text/html", "Relay 1 turned on.");
  digitalWrite (outputLed, 0);
}

void turnOnRelayTwo(){
  digitalWrite (outputLed, 1);
  digitalWrite (4, HIGH); //GPIO 4 // Relay 2
  server.send (200, "text/html", "Relay 2 turned on.");
  digitalWrite (outputLed, 0);
}

void handleRoot() {
  digitalWrite ( led, 1 );

  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  snprintf ( temp, 400,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>NodeMCU hooked to Relay Board</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
  <h1>Hello from NodeMCU!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",

          hr, min % 60, sec % 60
           );
  server.send ( 200, "text/html", temp );
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 0 );
}

void setup ( void ) {
  pinMode ( led, OUTPUT );
  pinMode(10, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(outputLed, OUTPUT);
  digitalWrite ( led, 0 );
  Serial.begin ( 9600 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );

  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );

  server.on("/relay1", turnOnRelayOne);
  server.on("/relay2", turnOnRelayTwo);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop (void) {
  server.handleClient();
}