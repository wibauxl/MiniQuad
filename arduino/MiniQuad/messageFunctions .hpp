int battery = 100;

void broadcastBatteryState() {
  wsSendValue("battery", battery);
}

void updateBatteryState() {
  // int battery = analogRead(A0);
  battery = BATTERY_MAX - timeStamp/4000;
  battery = map(battery, BATTERY_MIN, BATTERY_MAX, 0, 100);
  if (battery < 0) battery = 0;
  else if (battery > 100) battery = 100;
  broadcastBatteryState();
}

void handleServoAction(String params) {
  // getting the action as [test | move | speed]:actionId;
  String actionType = params.substring(0, params.indexOf(":"));
  String actionId = params.substring(params.indexOf(":") + 1);
  if (actionType.equals("test")) {
    if (actionId.equals("center")) {
      miniQuadState = MINI_QUAD_IDLE;
      for (int i=0; i<NB_SERVOS; i++) servoRawWrite(i, miniQuadConfig.servoCenter[i]);
    } else if (actionId.equals("hips:range")) miniQuadState = MINI_QUAD_TEST_HIPS_RANGE;
    else if (actionId.equals("legs:range")) miniQuadState = MINI_QUAD_TEST_LEGS_RANGE;
  } else if (actionType.equals("speed")) {
    if (actionId.equals("get")) wsSendValue("speed", miniQuadConfig.speed);
    else miniQuadConfig.speed = actionId.substring(4).toInt();
  } else if (actionType.equals("moveSpeed")) {
    if (actionId.equals("get")) wsSendValue("moveSpeed", miniQuadConfig.moveSpeed);
    else miniQuadConfig.moveSpeed = actionId.substring(4).toInt();
  } else {
    if (actionId.startsWith("stop")) miniQuadState = MINI_QUAD_IDLE;
    else {
      miniQuadState = MINI_QUAD_MOVE;
      miniQuadStep = 0;
      loadMoveConfig(actionId);
    }
  }
}

void handleMiniQuadMessage(String message) {
  if (message.equals("config:save")) miniQuadConfig.changed = true;
  else if (message.startsWith("servo:calibrate")) servoSetCalibration(message.substring(16));
  else if (message.startsWith("action")) handleServoAction(message.substring(7));
  else if (message.startsWith("battery:get")) broadcastBatteryState();
}

