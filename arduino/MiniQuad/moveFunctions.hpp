/**
 * Moves are stored as move1.json to move12.json where move1 to move4 are forward, right, backward and left
 *    move1.json
 *    {
 *      "name" : "Forward",
 *      "ids" : ["RRL", "RRH", "RLH", "RLL", "FRL", "FRH", "FLH', "BLL", "time"],
 *      "steps" : [
 *         [0, 0, 0, 0, 0, 0, 0, 0, 0],
 *         [-100, -100, -100, -100, 0, 0, 0, 0, 20]
 *       ]
 *    }
 *
 * The remote application will call moves.json and m1 to m12.json, these are generated on the fly
 * from the moves to avoid reading from flash all the time.
 *    moves.json
 *    [
 *      {"n":"Forward","id":1}
 *      {"n":"Danse","id":8},
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
 */

#define SERVO_SPEED 120       // ms per 60°

#ifdef USE_PWM_SERVO_DRIVER
#include <Adafruit_PWMServoDriver.h>
#define SERVO_MIN_PULSE 150   // ms
#define SERVO_MAX_PULSE 150   // ms
Adafruit_PWMServoDriver miniQuadServoDriver = Adafruit_PWMServoDriver();
#else
Servo miniQuadServos[NB_SERVOS];
#endif

#define MINI_QUAD_IDLE -1
#define MINI_QUAD_MOVE 1
#define MINI_QUAD_TEST_CENTER 100
#define MINI_QUAD_TEST_HIPS_RANGE 101
#define MINI_QUAD_TEST_LEGS_RANGE 102

int8_t miniQuadMove = MINI_QUAD_IDLE;
int miniQuadStep = 0;
int miniQuadSubStep = 0;
String movesJson;

// write in degrees from -90 to 90
void servoWriteDegrees(uint8_t servoNb, int8_t degrees) {
  #ifdef USE_PWM_SERVO_DRIVER
    miniQuadServoDriver.setPWM(servoNb, 0, map(degrees, -90, 90, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
  #else
    miniQuadServos[servoNb].write(90+position);
  #endif
}

// write in percentage of the calibrated range
void servoWrite(uint8_t servoNb, int8_t percentage) {
  servoWriteDegrees(servoNb, map(percentage, -100, 100, miniQuadConfig.servoCenter[servoNb]-miniQuadConfig.servoRange[servoNb], miniQuadConfig.servoCenter[servoNb]+miniQuadConfig.servoRange[servoNb]));
}

void startServos() {
  #ifdef USE_PWM_SERVO_DRIVER
    miniQuadServoDriver.begin();
    miniQuadServoDriver.setPWMFreq(60);
  #else
    for (int i=0; i<NB_SERVOS; i++) miniQuadServos[i].attach(miniQuadServoPins[i]);
  #endif
  for (int i=0; i<NB_SERVOS; i++) servoWriteDegrees(i, miniQuadConfig.servoCenter[i]);
}

void handleServoMoves() {
  if (miniQuadMove == MINI_QUAD_IDLE) return;
  // 
}

void setMove(int moveId) {
  miniQuadMove = moveId;
  miniQuadStep = 0;
  miniQuadSubStep = 0;
}

void setMoveJson(int moveId) {
  if (miniQuadMovesConfig[moveId].nbSteps == 0) {
    miniQuadMovesConfig[moveId].defined = false;
    return;
  }
  StaticJsonBuffer<MOVE_JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& moveJson = jsonBuffer.createObject();
  moveJson["name"] = miniQuadMovesConfig[moveId].name;
  JsonArray& ids = moveJson.createNestedArray("ids");
  for (int servo=0; servo<NB_SERVOS+1; servo++) ids.add(miniQuadServoNames[servo]);
  JsonArray& steps = moveJson.createNestedArray("steps");
  for (int stepId=0; stepId<miniQuadMovesConfig[moveId].nbSteps; stepId++) {
    JsonArray& step = steps.createNestedArray();
    for (int servo=0; servo<NB_SERVOS+1; servo++) step.add(miniQuadMovesConfig[moveId].steps[stepId][servo]);
  }
  moveJson.prettyPrintTo(miniQuadMovesConfig[moveId].json);
}

void loadMovesConfig() {
  StaticJsonBuffer<MOVE_JSON_BUFFER_SIZE> jsonBuffer;
  // parse files from move1.json to move13.json, skipping move9.json
  for (int moveId=0; moveId<NB_MOVES; moveId++) {
    File file = SPIFFS.open(String("move")+String(moveId)+String(".json"), "r");
    if (file) {
      JsonObject& moveJson = jsonBuffer.parseObject(file);
      if (moveJson.success()) {
        moveJson.prettyPrintTo(miniQuadMovesConfig[moveId].json);
        miniQuadMovesConfig[moveId].defined = true;
        miniQuadMovesConfig[moveId].changed = false;
        strcpy(miniQuadMovesConfig[moveId].name, moveJson["name"]);
        JsonArray& steps = moveJson["steps"];
        for (int stepId=0; stepId<steps.size(); stepId++) {
          for (int servo=0; servo<NB_SERVOS+1; servo++)
            miniQuadMovesConfig[moveId].steps[stepId][servo] = steps[stepId][servo];
        }
      }
      file.close();
    }
  }
  // create a json String containing moves and their ids
  JsonArray& moveListJson = jsonBuffer.createArray();
  for (int moveId=0; moveId<4+NB_MOVES; moveId++) {
    if (!miniQuadMovesConfig[moveId].defined) continue;
    JsonObject& move = moveListJson.createNestedObject();
    move["n"] = miniQuadMovesConfig[moveId].name;
    move["id"] = moveId;
  }
  moveListJson.printTo(movesJson);
}

void saveMovesConfig() {
  for (int moveId=0; moveId<NB_MOVES; moveId++) {
    if (!miniQuadMovesConfig[moveId].defined) continue;
    if (!miniQuadMovesConfig[moveId].changed) continue;
    File file = SPIFFS.open(String("move")+String(moveId+1)+String(".json"), "w");
    file.print(miniQuadMovesConfig[moveId].json);
    file.flush();
    file.close();
  }
}
