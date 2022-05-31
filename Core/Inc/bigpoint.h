/*
 * bigpoint.h
 *
 *  Created on: 31 maj 2022
 *
 *      Author: Dawid Łukasiewicz and Martyna Żukowska
 */
#include <stdlib.h>
#include <time.h>
#include "stm32f429i_discovery_lcd.h"

#ifndef INC_BIGPOINT_H_
#define INC_BIGPOINT_H_


typedef struct
{
	int16_t Xpos;
	int16_t Ypos;
	uint16_t Width;
	uint16_t Height;
	int16_t zdobyty;
}BigItem;


void BItem_Init(BigItem *Bpoint, int16_t X, int16_t Y, uint16_t width, uint16_t height);

void BItem_Overflow(BigItem *BigItem, uint8_t NumberOfPoints);

void BItem_OverflowRandom(BigItem *BigItem, uint8_t NumberOfPoints, uint16_t width_limit);

void ParityBItem_OverflowRandom(BigItem *BigItem, uint8_t NumberOfPoints, uint16_t width_limit);

void SingleBItem_Move(BigItem *BigItem, int16_t X, int16_t Y);

void MultiBItem_Move(BigItem *BigItem, uint8_t NumberOfPoints, int16_t X, int16_t Y);

void SingleBItem_Draw(BigItem *BigItem);

void MultiBItem_Draw(BigItem *BigItem, uint8_t NumberOfPoints);

int IfScoreB(BigItem *BigItem, uint8_t NumberOfPoints, int16_t X_ball, int16_t Y_ball, uint16_t Ray);

#endif /* INC_POINT_H_ */
