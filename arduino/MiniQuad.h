#ifndef _MINI_QUAD_H_
#define _MINI_QUAD_H_

#define MINIQUAD_DEBUG 1

#define HOST_NAME "MiniQuad"
#define MINI_QUAD_PASSWORD "miniquad"
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);

#define CONFIG_FILE "/config.json"

#define BATTERY_REFRESH 10                /* seconds between 2 battery value measurements */
#define BATTERY_MIN (32*1024*100/10/570)  /* 3.2V : 100/570 divider : scale of 0 to 1024 */ 
#define BATTERY_MAX (43*1024*100/10/570)  /* 4.3V : 100/570 divider : scale of 0 to 1024 */ 

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

const int MQServoPins[NB_SERVOS] = { FLH, FLL, FRH, FRL, BRH, BRL, BLH, BLL };
String MQServoNames[NB_SERVOS] = { "FLH", "FLL", "FRH", "FRL", "BRH", "BRL", "BLH", "BLL" };

#define MINI_QUAD_IDLE 0

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

#endif
