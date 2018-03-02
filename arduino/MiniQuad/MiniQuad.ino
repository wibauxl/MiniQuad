#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Hash.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <Wire.h>

#include "MiniQuad.h"
#include "configFunctions.hpp"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;

signed long timeStamp;
int miniQuadState = MINI_QUAD_IDLE;

#include "helperFunctions.hpp"
#include "moveFunctions.hpp"
#include "messagingFunctions.hpp"
#include "startupFunctions.hpp"
#include "webHandlers.hpp"

void setup() {

  Serial.begin(115200);

  // start the file system
  startSPIFFS();
  // load the configuration
  loadConfig();
  // start the WiFi
  startWiFi(miniQuadConfig.hostName, MINI_QUAD_PASSWORD, miniQuadConfig.wiFiNeedSetup);

  // create an event handler for the websocket server
  ws.onEvent(handleWebSocketEvent);
  server.addHandler(&ws);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  
  // deal with the other cases
  server.onNotFound(handleNotFound);

  // start a DNS server covering all IPs
  dnsServer.start(53, "*", WiFi.softAPIP());

  // start the web server
  server.begin();

  // start the servos
  startServos();

  timeStamp = millis();
}

void loop() {
  // handle the DNS
  dnsServer.processNextRequest();
  
  // handle servo movements
  if (miniQuadState != MINI_QUAD_IDLE) handleServoMoves();

  // save the configuration is it changed
  if (miniQuadConfig.changed) saveConfig();
  
  // update the battery state every BATTERY_REFRESH second
  if (millis() - timeStamp > (unsigned long)(BATTERY_REFRESH*1000)) {
    timeStamp = millis();
    updateBatteryState();
  }
}
