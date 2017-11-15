#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ArduinoJson.h>
#include <Servo.h>

#include "MiniQuad.h"
#include "configHandlers.hpp"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

signed long timeKeeper;
int miniQuadState = MINI_QUAD_IDLE;

#include "helperFunctions.hpp"
#include "miniQuadFunctions.hpp"
#include "startupHandlers.hpp"
#include "eventHandlers.hpp"

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

  server.begin();

  startServos();

  timeKeeper = millis();
}

void loop() {
  // handle servo movements
  if (miniQuadState != MINI_QUAD_IDLE) handleServoMove();

  // save the configuration is it changed
  if (miniQuadConfig.changed) saveConfig();
  
  // send the battery value every BATTERY_REFRESH second
  if (millis() - timeKeeper > (unsigned long)(BATTERY_REFRESH*1000)) {
    timeKeeper = millis();
    updateBatteryState();
  }
}
