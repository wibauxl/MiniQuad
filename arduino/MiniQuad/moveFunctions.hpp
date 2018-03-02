/**
 * Pages will call moves.json and m1 to m13.json, these are generated on the fly from the moves to avoid reading from flash all the time
 *    moves.json   
 *    [
 *      {"n":"Danse","id":2},
 *      {"n":"Forward","id":10}
 *    ]
 *    
 *    m1.json
 *    {
 *      "name" : "Salute",
 *      "steps" : [
 *         [0, 0, 0, 0, 0, 0, 0, 0, 0],
 *         [-100, -100, -100, -100, 0, 0, 0, 0, 20]
 *       ]
 *    }
 *
 * Moves are stored as move1.json to move13.json where move10 to move13 are forward, right, backward and left
 */

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

uint8_t miniQuadState = MINI_QUAD_IDLE;
int miniQuadStep = 0;
String movesJson;

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

void setServoCalibration(String params) {
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

void handleServoMoves() {
  if (miniQuadState == MINI_QUAD_IDLE) return;
}


void setMove(int moveId) {
}

void setMoveConfiguration(String params) {
  
}

void loadMovesConfig() {

  // create a json file containing
  movesJson = "[";
  bool hasMove = false;
  for (int i=0; i<NB_MOVES; i++) {
    if (miniQuadMovesConfig[i].name.equals("")) continue;
    if (hasMove) movesJson += ",";
    movesJson += "{\"n\":\"" + miniQuadMovesConfig[i].name + "\",\"id\":" + i + "}";
    hasMove = true;
  }
  movesJson += "]";
}

void saveMovesConfig() {
}
}

