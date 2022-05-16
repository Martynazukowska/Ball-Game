/*
 * Obstacle.h
 *
 *  Created on: 16 maj 2022
 *      Author: Dawid Łukasiewicz
 */
#include <stdlib.h>
#include "stm32f429i_discovery_lcd.h"

#ifndef INC_OBSTACLE_H_
#define INC_OBSTACLE_H_

/**
 * @brief struct of an obstacle
 * @det has X, Y beginnig positions, width and height
 */
typedef struct
{
	uint16_t Xpos;
	uint16_t Ypos;
	uint16_t Width;
	uint16_t Height;
}ObstacleDef;

void Obstacle_Init(ObstacleDef *obstacle, uint16_t X, uint16_t Y, uint16_t width, uint16_t height);

void Obstacle_Overflow(ObstacleDef *obstacle, uint8_t NumberOfObjects);

void Obstacle_OverflowNew(ObstacleDef *obstacle, uint8_t NumberOfObjects);

void SingleObstacle_Move(ObstacleDef *obstacle, int X, int Y);

void MultiObstacle_Move(ObstacleDef *obstacle, uint8_t NumberOfObjects, int X, int Y);

int IfCollisionDetect(ObstacleDef *obstacle, uint8_t NumberOfObjects);

void SingleObstacle_Draw(ObstacleDef *obstacle);

void MultiObstacle_Draw(ObstacleDef *obstacle, uint8_t NumberOfObjects);

#endif /* INC_OBSTACLE_H_ */
