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

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;

#include "MiniQuad.h"
#include "configFunctions.hpp"
#include "helperFunctions.hpp"
#include "moveFunctions.hpp"
#include "messageFunctions.hpp"
#include "startupFunctions.hpp"
#include "webHandlers.hpp"

void setup() {
  Serial.begin(115200);

  // start the file system
  startSPIFFS();
  // load the configurations
  loadConfig();
  loadMovesConfig();

  // start the WiFi
  startWiFi(miniQuadConfig.hostName, MINI_QUAD_PASSWORD, miniQuadConfig.wiFiNeedSetup);

  // create an event handler for the websocket server
  ws.onEvent(handleWebSocketEvent);

  // add handlers to the server
  server.addHandler(&ws);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(handleNotFound);

  // start a DNS server covering all IPs
  dnsServer.start(53, "*", WiFi.softAPIP());

  // start the web server
  server.begin();

  // start the servos
  startServos();

  batteryTimeStamp = millis();
}

void loop() {
  // handle the DNS
  dnsServer.processNextRequest();

  // handle servo movements
  handleServoMoves();

  // save the configuration is they have changed
  saveConfig();
  saveMovesConfig();

  // update the battery state every BATTERY_REFRESH second
  if (millis() - batteryTimeStamp > (unsigned long)(BATTERY_REFRESH*1000)) {
    batteryTimeStamp = millis();
    updateBatteryState();
  }
}
