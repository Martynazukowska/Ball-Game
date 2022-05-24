/*
 * Obstacle.h
 *
 *  Created on: 16 maj 2022
 *      Author: Dawid Łukasiewicz and Martyna Żukowska
 */
#include <stdlib.h>
#include <time.h>
#include "stm32f429i_discovery_lcd.h"

#ifndef INC_OBSTACLE_H_
#define INC_OBSTACLE_H_

/**
 * @brief struct of an obstacle

 * @det has X, Y beginnig positions, width and height
 */
typedef struct
{
	int16_t Xpos;
	int16_t Ypos;
	uint16_t Width;
	uint16_t Height;
}ObstacleDef;

/**
 * @brief initialize one object with given params
 */
void Obstacle_Init(ObstacleDef *obstacle, int16_t X, int16_t Y, uint16_t width, uint16_t height);

void Obstacle_Overflow(ObstacleDef *obstacle, uint8_t NumberOfObjects);

void Obstacle_OverflowRandom(ObstacleDef *obstacle, uint8_t NumberOfObjects, uint16_t width_limit);

void ParityObstacle_OverflowRandom(ObstacleDef *obstacle, uint8_t NumberOfObjects, uint16_t width_limit);

void SingleObstacle_Move(ObstacleDef *obstacle, int16_t X, int16_t Y);

void MultiObstacle_Move(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X, int16_t Y);

void SingleObstacle_Draw(ObstacleDef *obstacle);

void MultiObstacle_Draw(ObstacleDef *obstacle, uint8_t NumberOfObjects);

int IfCollisionDetect(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X_ball, int16_t Y_ball, uint16_t Ray);

#endif /* INC_OBSTACLE_H_ */
