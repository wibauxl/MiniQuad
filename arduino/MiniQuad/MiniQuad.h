#ifndef _MINI_QUAD_H_
#define _MINI_QUAD_H_

// display extensive debug messages
#define MINIQUAD_DEBUG 1
// the soft AP host name will be HOST_NAME-ChipId
#define HOST_NAME "MiniQuad"
#define MDNS_HOST_NAME "miniquad"
// if the password is NULL, the AP will not have any password
#define MINI_QUAD_PASSWORD NULL

IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);

#define CONFIG_FILE "/config.json"

#define USE_PWM_SERVO_DRIVER 1

#define NB_SERVOS 8

//  -----            -----
// | FLL |          | FRL |
//  ----- ---------- -----
//       | FLH  FRH |
//       |          |
//       | BLH  BRH |
//  ----- ---------- -----
// | BLL |          | BRL |
//  -----            -----

#define FLH   2   /* D4 on NODEMCU */
#define FLL   14  /* D5 */
#define FRH   12  /* D6 */
#define FRL   13  /* D7 */
#define BRH   15  /* D8 */
#define BRL   16  /* D0 */
#define BLH   4   /* D2 */
#define BLL   0   /* D3 */
#define SONAR 5

const int miniQuadServoPins[NB_SERVOS] = { FLL, FLH, FRH, FRL, BLL, BLH, BRH, BRL };
String miniQuadServoNames[NB_SERVOS+1] = { "FLL", "FLH", "FRH", "FRL", "BLL", "BLH", "BRH", "BRL", "time" };

typedef struct MiniQuadConfig {
  // saved item
  char hostName[20];
  int servoCenter[NB_SERVOS];
  int servoRange[NB_SERVOS];

  // runtime items
  bool changed;
  bool wiFiNeedSetup;
  int speed;
  int customSpeed;
};

MiniQuadConfig miniQuadConfig;

// forward, backward, right, left and 8 custom moves
#define NB_MOVES (4+8)
#define MAX_STEPS_PER_MOVE 30
#define MOVE_JSON_BUFFER_SIZE (JSON_ARRAY_SIZE(MAX_STEPS_PER_MOVE) + (1+MAX_STEPS_PER_MOVE)*JSON_ARRAY_SIZE(9) + JSON_OBJECT_SIZE(3))

typedef struct MiniQuadMoveConfig {
  // saved items
  char name[10];
  int8_t steps[MAX_STEPS_PER_MOVE][NB_SERVOS+1];
  // runtime items
  String json;
  uint8_t nbSteps;
  bool changed;
  bool stepChanged;
  bool defined;
};

MiniQuadMoveConfig miniQuadMovesConfig[NB_MOVES];

int32_t batteryTimeStamp;

#define BATTERY_REFRESH 10                /* seconds between succesive battery value measurements */
#define BATTERY_MIN (32*1024*100/10/470)  /* 100/470 divider on A0: min battery is 3.2V => 3.2*100/470*1024 => BATTERY_MIN = 697 */
#define BATTERY_MAX (43*1024*100/10/470)  /* 100/470 divider on A0: max battery is 4.3V => 4.3*100/470*1024 => BATTERY_MAX = 937 */
uint8_t battery = 100;

#endif
