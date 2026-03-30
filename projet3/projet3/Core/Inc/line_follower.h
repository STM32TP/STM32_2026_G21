#ifndef LINE_FOLLOWER_H
#define LINE_FOLLOWER_H

#include "main.h"

#define BASE_SPEED 200
#define KP 35
#define KD 120

#define THRESHOLD 2400 // AJUSTA según tu sensor
#define TARGET_DISTANCE 7


typedef enum{
    LINE_FOLLOW,
    OBSTACLE_DETECTED,
    LEAVE_OBSTACLE,
	REALIGN,
    SEARCH_LINE
} RobotState;

void LineFollower_Update(uint16_t *sensors);
void LineFollower_Init();

#endif
