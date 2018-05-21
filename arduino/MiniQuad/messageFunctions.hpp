/**
 * Messages sent by the html application
 *
 *  config:calibration:save             save calibration into config.json
 *  config:calibrate:ID:center:V        set servo ID center calibration to V
 *  config:calibrate:ID:range:V         set servo ID range calibration to V
 *  config:move:M:save:NAME             save move M into moveN.json with name NAME
 *  config:move:M:duplicate             duplicate move M
 *  config:move:M:delete                delete move M
 *  config:move:M:S:save                save step S of move M
 *  config:move:M:S:duplicate           duplicate step S of move M
 *  config:move:M:S:delete              delete step S of move M
 *  config:move:M:S:N:V                 set servo N of step S of move M to value V
 *
 *  action:test:center                  bring all servos to center
 *  action:test:range:hips              go from center to min to max to center for hips servos
 *  action:test:range:legs              go from center to min to max to center for legs servos
 *  action:speed:main:get               get the speed for the 4 main moves
 *  action:speed:main:set:V             set the speed for the 4 main moves
 *  action:speed:custom:get           	get the speed for user moves
 *  action:speed:custom:set:V 	        set the speed for user moves
 *  action:move:stop                    stop moves
 *  action:move:M                       start move number N
 *
 *  battery:get                         send the bettery value
 */
/**
 * Messages sent by the arduino application
 *  battery:V                           return the battery value V
 *  speed:main:V                        return the V speed for the main moves
 *  speed:user:V                       	return the V speed for the user moves
 */


/** handle battery:get
 */
void broadcastBatteryState() {
  wsSendValue("battery", battery);
}

void updateBatteryState() {
  // battery = BATTERY_MAX - timeStamp/4000;
  battery = analogRead(A0);
  battery = map(battery, BATTERY_MIN, BATTERY_MAX, 0, 100);
  if (battery < 0) battery = 0;
  else if (battery > 100) battery = 100;
  broadcastBatteryState();
}

/** handle action:...
 *  action:test:center                  bring all servos to center
 *  action:test:range:hips              go from center to min to max to center for hips servos
 *  action:test:range:legs              go from center to min to max to center for legs servos
 *  action:speed:main:get               get the speed for the 4 main moves
 *  action:speed:main:set:V             set the speed for the 4 main moves
 *  action:speed:custom:get             get the speed for custom moves
 *  action:speed:custom:set:V           set the speed for custom moves
 *  action:move:stop                    stop moves
 *  action:move:M                       start move number N
 */
void handleServoAction(String *params) {
  if (params[1].equals("test")) {
    if (params[2].equals("center")) {
      miniQuadState = MINI_QUAD_IDLE;
      for (int i=0; i<NB_SERVOS; i++) servoWrite(i, 0);
    } else {  // range
      if (params[3].equals("hips")) miniQuadState = MINI_QUAD_TEST_HIPS_RANGE;
      else miniQuadState = MINI_QUAD_TEST_LEGS_RANGE;
    }
  } else if (params[1].equals("speed")) {
    if (params[3].equals("get")) {
      if (params[2].equals("main")) wsSendValue("speed:main:", miniQuadConfig.speed);
      else wsSendValue("speed:custom:", miniQuadConfig.customSpeed);
    } else {  //set
      int speed = params[4].toInt();
      if (params[2].equals("main")) miniQuadConfig.speed = speed;
      else miniQuadConfig.customSpeed = speed;
    }
  } else {
    if (params[2].equals("stop")) miniQuadState = MINI_QUAD_IDLE;
    else {
      miniQuadState = MINI_QUAD_MOVE;
      miniQuadStep = 0;
      setMove(params[2].toInt());
    }
  }
}

/** handle config:calibrate:...
 *  config:calibrate:N:center:V        set servo N center calibration to V
 *  config:calibrate:N:range:V         set servo N range calibration to V
 */
void setServoCalibration(String *params) {
  String servoNb = params[2].toInt();
  int servoValue = params[4].toInt();
  if (params[3].equals("center")) {
    miniQuadConfig.servoCenter[servoNb] = servoValue;
    servoWriteDegrees(servoNb, servoValue-90);
  } else {
    miniQuadConfig.servoRange[servoNb] = servoValue;
    servoWriteDegrees(servoNb, miniQuadConfig.servoCenter[servoNb]-90 + servoValue);
  }
}

/** handle config:move:...
 *  config:move:M:save:NAME             save move M into moveN.json with name NAME
 *  config:move:M:duplicate             duplicate move M into free slot
 *  config:move:M:delete                delete move M
 *  config:move:M:S:duplicate           duplicate step S of move M
 *  config:move:M:S:delete              delete step S of move M
 *  config:move:M:S:N:V                 set servo N of step S of move M to value V
 */
void setMoveConfiguration(String *params) {
  int moveId = params[2].toInt();
  if (params[3].equals("save")) {
    const char *name = params[4].c_str();
    if (!strcmp(miniQuadMovesConfig[moveId].name, name)) {
      strcpy(miniQuadMovesConfig[moveId].name, name);
      miniQuadMovesConfig[moveId].changed = true;
    }
  } else if (params[3].equals("duplicate")) {
    if (miniQuadMovesConfig[moveId].stepChanged) {
      miniQuadMovesConfig[moveId].changed = true;
      setMoveJson(moveId);
    }
    // find a free slot
    int duplicateId = 0;
    for (; duplicateId<NB_MOVES; duplicateId++) {
      if (!miniQuadMovesConfig[duplicateId].defined) break;
    }
    // if one exists, copy moveId move date into duplicateId move
    if (!miniQuadMovesConfig[duplicateId].defined) {
      miniQuadMovesConfig[duplicateId].defined = true;
      miniQuadMovesConfig[duplicateId].changed = true;
      miniQuadMovesConfig[duplicateId].nbSteps = miniQuadMovesConfig[moveId].nbSteps;
      strcpy(miniQuadMovesConfig[moveId].name, miniQuadMovesConfig[duplicateId].name);
      miniQuadMovesConfig[duplicateId].json = miniQuadMovesConfig[moveId].json;
      memcpy(miniQuadMovesConfig[duplicateId].steps, miniQuadMovesConfig[moveId].steps, MAX_STEPS_PER_MOVE*(NB_SERVOS+1));
    }
    // since we already redefined the json, we can exit
    if (miniQuadMovesConfig[moveId].stepChanged) return;
  } else if (params[3].equals("delete")) {
    miniQuadMovesConfig[moveId].nbSteps = 0;
    miniQuadMovesConfig[moveId].changed = true;
  } else {
    int stepId = params[3].toInt();
    if (params[4].equals("save")) {
      if (miniQuadMovesConfig[moveId].stepChanged) miniQuadMovesConfig[moveId].changed = true;
    } else if (params[4].equals("delete")) {
      if (stepId < miniQuadMovesConfig[moveId].nbSteps-1)
        // shift down all steps
        memcpy(miniQuadMovesConfig[moveId].steps[stepId], miniQuadMovesConfig[moveId].steps[stepId+1], miniQuadMovesConfig[moveId].nbSteps-stepId-1);
      if (miniQuadMovesConfig[moveId].nbSteps > 0) miniQuadMovesConfig[moveId].nbSteps --;
      miniQuadMovesConfig[moveId].changed = true;
    } else if (params[4].equals("duplicate")) {
      // shift up all steps
      for (int step=miniQuadMovesConfig[moveId].nbSteps; step>stepId; step--)
        memcpy(miniQuadMovesConfig[moveId].steps[step], miniQuadMovesConfig[moveId].steps[step-1], NB_SERVOS+1);
      miniQuadMovesConfig[moveId].nbSteps ++;
      miniQuadMovesConfig[moveId].changed = true;
    } else {
      int servoNb = params[4].toInt();
      int value = params[5].toInt();
      if (miniQuadMovesConfig[moveId].steps[stepId][servoNb] != value) {
        miniQuadMovesConfig[moveId].steps[stepId][servoNb] = value;
        miniQuadMovesConfig[moveId].stepChanged = true;
        servoWrite(servoNb, value);
      }
    }
  }
  if (miniQuadMovesConfig[moveId].changed) setMoveJson(moveId);
}

void handleMiniQuadMessage(String message) {
  if (message.equals("config:calibration:save")) miniQuadConfig.changed = true;
  else if (message.startsWith("battery:get")) broadcastBatteryState();
  else {
    String params[6];
    splitString(message, ":", params);
    if (message.startsWith("config:calibrate")) setServoCalibration(params);
    else if (message.startsWith("action")) handleServoAction(params);
    else if (message.startsWith("config:move")) setMoveConfiguration(params);
  }
}
