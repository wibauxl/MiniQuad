#ifdef USE_PWM_SERVO_DRIVER
#include <Adafruit_PWMServoDriver.h>
#define SERVO_MIN_PULSE 150
#define SERVO_MAX_PULSE 150
Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver();
#else
Servo MQServos[8];  
#endif

#define MINI_QUAD_IDLE 0
#define MINI_QUAD_MOVE 1
#define MINI_QUAD_TEST_CENTER 2
#define MINI_QUAD_TEST_HIPS_RANGE 3
#define MINI_QUAD_TEST_LEGS_RANGE 4

int miniQuadStep = 0;

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

// write in degrees -127 to +127
void servoRawWrite(uint8_t servoNb, int8_t position) {
#ifdef USE_PWM_SERVO_DRIVER
  servoDriver.setPWM(servoNb, 0, map(position, -127, 127, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
#else
  MQServos[servoNb].write(position);
#endif  
}

// write in percentage of the calibrated range
void servoWrite(uint8_t servoNb, int8_t percentage) {
  servoRawWrite(servoNb, map(percentage, -100, 100, miniQuadConfig.servoCenter[servoNb]-miniQuadConfig.servoRange[servoNb], miniQuadConfig.servoCenter[servoNb]+miniQuadConfig.servoRange[servoNb])); 
}

void startServos() {
#ifdef USE_PWM_SERVO_DRIVER
  servoDriver.begin();
  servoDriver.setPWMFreq(60);
#else
  for (int i=0; i<NB_SERVOS; i++) MQServos[i].attach(MQServoPins[i]);
#endif  
  for (int i=0; i<NB_SERVOS; i++) servoRawWrite(i, miniQuadConfig.servoCenter[i]);
}

void servoSetCalibration(String params) {
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
    servoWrite(servoNb, servoValue);
  } else {
    miniQuadConfig.servoRange[servoNb] = servoValue;
    servoWrite(servoNb, miniQuadConfig.servoCenter[servoNb] + servoValue);
  }
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
    else if (actionId.equals("forward")) miniQuadState = MINI_QUAD_FORWARD;
    else if (actionId.equals("backward")) miniQuadState = MINI_QUAD_BACKWARD;
    else if (actionId.equals("right")) miniQuadState = MINI_QUAD_RIGHT;
    else if (actionId.equals("left")) miniQuadState = MINI_QUAD_LEFT;
  }
}

void handleServoMove() {
}

void handleMiniQuadMessage(String message) {
  if (message.equals("config:save")) miniQuadConfig.changed = true;
  else if (message.startsWith("servo:calibrate")) servoSetCalibration(message.substring(16));
  else if (message.startsWith("action")) handleServoAction(message.substring(7));
  else if (message.startsWith("battery:get")) broadcastBatteryState();
}

