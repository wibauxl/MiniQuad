
void startSPIFFS() {
  SPIFFS.begin();
  Serial.println();
  Serial.print("SPIFFS started.");  
#ifdef MINIQUAD_DEBUG      
  Serial.println(" Contents:");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
  }
#endif
  Serial.println();
}

void startWiFi(char *AP_SSID, char *AP_Password, bool wiFiNeedSetup) {
  Serial.print("Starting AP on ");
  Serial.println(AP_SSID);
  Serial.print("Running WebServer on ");
  Serial.println(apIP.toString());

  // start an http MDNS service called by miniquad.local
  // MDNS.begin(MDNS_HOST_NAME);
  // MDNS.addService("http", "tcp", 80);
  
  if (!wiFiNeedSetup) return;

  WiFi.enableSTA(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  if (AP_Password != NULL) WiFi.softAP(AP_SSID, AP_Password);
  else WiFi.softAP(AP_SSID);
}

