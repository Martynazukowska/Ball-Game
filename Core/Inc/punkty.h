/*
 * punkty.h
 *
 *  Created on: 27 maj 2022
 *      Author: Dawid Łukasiewicz and Martyna Żukowska
 */
#include <stdlib.h>
#include <time.h>
#include "stm32f429i_discovery_lcd.h"

#ifndef INC_POINT_H_
#define INC_POINT_H_


typedef struct
{
	int16_t Xpos;
	int16_t Ypos;
	uint16_t Width;
	uint16_t Height;
}Item;


void Item_Init(Item *point, int16_t X, int16_t Y, uint16_t width, uint16_t height);

void Item_Overflow(Item *point, uint8_t NumberOfPoints);

void Item_OverflowRandom(Item *point, uint8_t NumberOfPoints, uint16_t width_limit);

void ParityItem_OverflowRandom(Item *point, uint8_t NumberOfPoints, uint16_t width_limit);

void SingleItem_Move(Item *point, int16_t X, int16_t Y);

void MultiItem_Move(Item *point, uint8_t NumberOfPoints, int16_t X, int16_t Y);

void SingleItem_Draw(Item *point);

void MultiItem_Draw(Item *point, uint8_t NumberOfPoints);

int IfScore(Item *point, uint8_t NumberOfPoints, int16_t X_ball, int16_t Y_ball, uint16_t Ray);

#endif /* INC_POINT_H_ */

