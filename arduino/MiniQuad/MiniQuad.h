#ifndef _MINI_QUAD_H_
#define _MINI_QUAD_H_

#define MINIQUAD_DEBUG 1

#define DNS_PORT = 53;

#define HOST_NAME "MiniQuad"
#define MDNS_HOST_NAME "miniquad"
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

#define FLH D4
#define FLL D5
#define FRH D6
#define FRL D7
#define BRH D8
#define BRL D0
#define BLH D2
#define BLL D3

const int MQServoPins[NB_SERVOS] = { FLL, FLH, FRH, FRL, BLL, BRH, BLH, BLL };
String MQServoNames[NB_SERVOS] = { "FLH", "FLL", "FRH", "FRL", "BLL", "BLH", "BLH", "BLL" };

typedef struct MiniQuadConfig {
  // saved item
  char hostName[20];
  int servoCenter[NB_SERVOS];
  int servoRange[NB_SERVOS];
  int speed;
  int moveSpeed;
  
  // runtime items
  bool changed;
  bool wiFiNeedSetup;
};

MiniQuadConfig miniQuadConfig;

// forward, backward, right, left and 8 user moves
#define NB_MOVES (4+8)
#define MAX_STEPS_PER_MOVE 30

typedef struct MiniQuadMoveConfig {
  String name;
  String json;
  uint8_t nbSteps;
  bool changed;
  uint8_t steps[MAX_STEPS_PER_MOVE][NB_SERVOS+1];
};

MiniQuadMoveConfig miniQuadMovesConfig[NB_MOVES];

int32_t timeStamp;

#define BATTERY_REFRESH 10                /* seconds between 2 battery value measurements */
#define BATTERY_MIN (32*1024*100/10/570)  /* 3.2V : 100/570 divider : scale of 0 to 1024 */ 
#define BATTERY_MAX (43*1024*100/10/570)  /* 4.3V : 100/570 divider : scale of 0 to 1024 */ 
uint8_t battery = 100;

#endif
