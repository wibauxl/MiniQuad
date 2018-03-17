
void initConfig() {
  miniQuadConfig.changed = true;
  String hostName = String(ESP.getChipId(), HEX);
  hostName.toUpperCase();
  hostName = String(HOST_NAME) + "-" + hostName;
  strcpy(miniQuadConfig.hostName, hostName.c_str());
  for (int i=0; i<NB_SERVOS; i++) {
      miniQuadConfig.servoCenter[i] = 90;
      miniQuadConfig.servoRange[i] = 40;
  }
  miniQuadConfig.wiFiNeedSetup = true;
}

void loadConfig() {
  miniQuadConfig.wiFiNeedSetup = false;
  miniQuadConfig.speed = 50;
  miniQuadConfig.customSpeed = 50;
  Serial.print("Loading configuration -> ");
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("/config.json not found, creating default configuration");
    initConfig();
    return;
  }
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& configJson = jsonBuffer.parseObject(file);
  if (!configJson.success()) {
    Serial.println("impossible to parse /config.json, using default configuration");
    initConfig();
  } else {
    Serial.println("using /config.json");
#ifdef MINIQUAD_DEBUG    
    configJson.prettyPrintTo(Serial);
#endif
    strcpy(miniQuadConfig.hostName, configJson["host"]);
    JsonArray& servo = configJson["servo"];
    for (int i=0; i<NB_SERVOS; i++) {
      miniQuadConfig.servoCenter[i] = servo[i][1];
      miniQuadConfig.servoRange[i] = servo[i][2];
    }
  }
  file.close();
  Serial.println();
  Serial.println();
}

void saveConfig() {
  if (!miniQuadConfig.changed) return;
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& configJson = jsonBuffer.createObject();
  configJson["host"] = miniQuadConfig.hostName;
  JsonArray& servos = configJson.createNestedArray("servo");
  for (int i=0; i<NB_SERVOS; i++) {
    JsonArray& servo = servos.createNestedArray();
    servo.add(miniQuadServoNames[i].c_str());
    servo.add(miniQuadConfig.servoCenter[i]);
    servo.add(miniQuadConfig.servoRange[i]);
  }
  File file = SPIFFS.open(CONFIG_FILE, "w");
  configJson.printTo(file);
  file.flush();
  file.close();
  miniQuadConfig.changed = false;
  Serial.println("/config.json saved");
}


