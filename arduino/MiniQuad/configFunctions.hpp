/**
 * config.json example
{
	"host": "MiniQuad-E34567",
	"servoNames": ["RRL", "RRH", "RLH", "RLL", "FRL", "FRH", "FLH", "BLL"],
	"servoCenters": [90, 80, 90, 90, 90, 90, 90, 90],
	"servoRanges": [40, 30, 40, 40, 40, 40, 40, 40]
}
*/
void saveConfig() {
  if (!miniQuadConfig.changed) return;
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& configJson = jsonBuffer.createObject();
  configJson["host"] = miniQuadConfig.hostName;
  JsonArray& servoNames = configJson.createNestedArray("servoNames");
  JsonArray& servoCenters = configJson.createNestedArray("servoCenters");
  JsonArray& servoRanges = configJson.createNestedArray("servoRanges");
  for (int i=0; i<NB_SERVOS; i++) {
    servoNames.add(miniQuadServoNames[i].c_str());
    servoCenters.add(miniQuadConfig.servoCenter[i]);
    servoRanges.add(miniQuadConfig.servoRange[i]);
  }
  File file = SPIFFS.open(CONFIG_FILE, "w");
  configJson.printTo(file);
  file.flush();
  file.close();
  miniQuadConfig.changed = false;
  Serial.println("/config.json saved");
}

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
  saveConfig();
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
    JsonArray& servoCenters = configJson["servoCenters"];
    JsonArray& servoRanges = configJson["servoRanges"];
    for (int i=0; i<NB_SERVOS; i++) {
      miniQuadConfig.servoCenter[i] = servoCenters[i];
      miniQuadConfig.servoRange[i] = servoRanges[i];
    }
  }
  file.close();
  Serial.println();
  Serial.println();
}
