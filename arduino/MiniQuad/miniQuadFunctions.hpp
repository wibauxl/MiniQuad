#define MINI_QUAD_IDLE 0
#define MINI_QUAD_FORWARD 1
#define MINI_QUAD_BACKWARD 2
#define MINI_QUAD_LEFT 3
#define MINI_QUAD_RIGHT 4
#define MINI_QUAD_TEST_CENTER 5
#define MINI_QUAD_TEST_HIPS_RANGE 6
#define MINI_QUAD_TEST_LEGS_RANGE 7

Servo MQServos[8];  

int miniQuadStep = 0;

int battery = 100;

void broadcastBatteryState() {
  wsSendValue("battery", battery);
}

void updateBatteryState() {
  // int battery = analogRead(A0);
  battery = BATTERY_MAX - timeKeeper/4000;
  battery = map(battery, BATTERY_MIN, BATTERY_MAX, 0, 100);
  if (battery < 0) battery = 0;
  else if (battery > 100) battery = 100;
  broadcastBatteryState();
}

void startServos() {
  for (int i=0; i<NB_SERVOS; i++) {
    MQServos[i].attach(MQServoPins[i]);
    MQServos[i].write(miniQuadConfig.servoCenter[i]);
  }
}

void servoSet(String params) {
  // getting the servo to set as id:[center | range]:value
  String servoId = params.substring(0, params.indexOf(":"));
  int servoNb = 0;
  for (; servoNb<NB_SERVOS; servoNb++) {
    if (MQServoNames[servoNb].equals(servoId)) break;
  }
  params =  params.substring(params.indexOf(":") + 1);
  bool center = params.startsWith("center");
  int servoValue = params.substring(params.indexOf(":") + 1).toInt();
  if (center) {
    miniQuadConfig.servoCenter[servoNb] = servoValue;
    MQServos[servoNb].write(servoValue);
  } else {
    miniQuadConfig.servoRange[servoNb] = servoValue;
    MQServos[servoNb].write(miniQuadConfig.servoCenter[servoNb] + servoValue);
  }
}

void handleServoAction(String params) {
  // getting the action as [test | move | speed]:actionId;
  String actionType = params.substring(0, params.indexOf(":"));
  String actionId = params.substring(params.indexOf(":") + 1);
  if (actionType.equals("test")) {
    if (actionId.equals("center")) {
      miniQuadState = MINI_QUAD_IDLE;
      for (int i=0; i<NB_SERVOS; i++) MQServos[i].write(miniQuadConfig.servoCenter[i]);
    } else if (actionId.equals("hipsRange")) miniQuadState = MINI_QUAD_TEST_HIPS_RANGE;
    else if (actionId.equals("legsRange")) miniQuadState = MINI_QUAD_TEST_LEGS_RANGE;
  } else if (actionType.equals("speed")) {
    if (actionId.equals("get")) wsSendValue("speed", miniQuadConfig.speed);
    else miniQuadConfig.speed = actionId.substring(4).toInt();
  } else if (actionType.equals("moveSpeed")) {
    if (actionId.equals("get")) wsSendValue("moveSpeed", miniQuadConfig.moveSpeed);
    else miniQuadConfig.moveSpeed = actionId.substring(4).toInt();
  } else {
    if (actionId.equals("stop")) miniQuadState = MINI_QUAD_IDLE;
    else if (actionId.equals("forward")) miniQuadState = MINI_QUAD_FORWARD;
    else if (actionId.equals("backward")) miniQuadState = MINI_QUAD_BACKWARD;
    else if (actionId.equals("right")) miniQuadState = MINI_QUAD_RIGHT;
    else if (actionId.equals("left")) miniQuadState = MINI_QUAD_LEFT;
  }
}

void handleServoMove() {
}


