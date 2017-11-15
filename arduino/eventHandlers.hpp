void handleWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
#ifdef MINIQUAD_DEBUG
    Serial.println("ws <> connected");
#endif
    client->ping();
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len) {
      String message = "";
      if (info->opcode == WS_TEXT) {
        for (size_t i=0; i<info->len; i++) message += (char)data[i];
      } else {
        char buff[3];
        for (size_t i=0; i<info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          message += buff ;
        }
      }
#ifdef MINIQUAD_DEBUG
      Serial.print("/ws/ > ");
      Serial.println(message);
#endif
      if (message.equals("config:save")) miniQuadConfig.changed = true;
      else if (message.startsWith("servo")) servoSet(message.substring(6));
      else if (message.startsWith("action")) handleServoAction(message.substring(7));
      else if (message.startsWith("battery:get")) sendBatteryState();
    }
  } else if (type == WS_EVT_DISCONNECT) {
  } else if (type == WS_EVT_ERROR) {
  } else if (type == WS_EVT_PONG) {
  }
}

void handleNotFound(AsyncWebServerRequest *request) {
  Serial.printf("NOT_FOUND: ");
  if (request->method() == HTTP_GET)
    Serial.printf("GET");
  else if (request->method() == HTTP_POST)
    Serial.printf("POST");
  else if (request->method() == HTTP_DELETE)
    Serial.printf("DELETE");
  else if (request->method() == HTTP_PUT)
    Serial.printf("PUT");
  else if (request->method() == HTTP_PATCH)
    Serial.printf("PATCH");
  else if (request->method() == HTTP_HEAD)
    Serial.printf("HEAD");
  else if (request->method() == HTTP_OPTIONS)
    Serial.printf("OPTIONS");
  else
    Serial.printf("UNKNOWN");
  Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
  
  request->send(404);
}

