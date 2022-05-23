/*
 * Obstacle.c
 *
 *  Created on: 16 maj 2022
 *      Author: Dawid Łukasiewicz
 */

#include "Obstacle.h"

void Obstacle_Init(ObstacleDef *obstacle, int16_t X, int16_t Y, uint16_t width, uint16_t height)
{
//	if(X >= BSP_LCD_GetXSize() || X + width> BSP_LCD_GetXSize())
//		return;
//	if(Y >= BSP_LCD_GetYSize() || Y + height> BSP_LCD_GetYSize())
//		return;

	obstacle->Xpos = X;
	obstacle->Ypos = Y;
	obstacle->Width = width;
	obstacle->Height = height;
}

void Obstacle_Overflow(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle[i].Ypos <= 0)
			obstacle[i].Ypos = BSP_LCD_GetYSize() - obstacle[i].Height;
	}
}

void Obstacle_OverflowRandom(ObstacleDef *obstacle, uint8_t NumberOfObjects, uint16_t width_limit)
{
	int16_t X, width;
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle[i].Ypos <= 0)
		{
			obstacle[i].Ypos = BSP_LCD_GetYSize() - obstacle[i].Height;
			X = rand() % width_limit + 40;
			if(BSP_LCD_GetXSize() - X < 50)
				width = BSP_LCD_GetXSize() - X;
			else if(BSP_LCD_GetXSize() - X > BSP_LCD_GetXSize() - 50)
			{
				X = 0;
				width = rand() % width_limit;
			}
			else
				width = rand() % width_limit;
			Obstacle_Init(&obstacle[i], X, obstacle[i].Ypos, width, obstacle[i].Height);
		}
	}
}

void ParityObstacle_OverflowRandom(ObstacleDef *obstacle, uint8_t NumberOfObjects, uint16_t width_limit)
{
	uint16_t width;
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle[i].Ypos <= 0)
		{
			obstacle[i].Ypos = BSP_LCD_GetYSize() - obstacle[i].Height;
			if(i % 2 == 0)
			{
				width = (rand() % width_limit) + 20;
				Obstacle_Init(&obstacle[i], 0, obstacle[i].Ypos, width, obstacle[i].Height);
			}
			else
			{
				width = (rand() % width_limit) + 20;
				Obstacle_Init(&obstacle[i], BSP_LCD_GetXSize() - width, obstacle[i].Ypos, width, obstacle[i].Height);
			}
		}
	}
}

void SingleObstacle_Move(ObstacleDef *obstacle, int16_t X, int16_t Y)
{
	obstacle->Xpos += X;
	obstacle->Ypos += Y;
}

void MultiObstacle_Move(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X, int16_t Y)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		SingleObstacle_Move(&obstacle[i], X, Y);
	}
}


void SingleObstacle_Draw(ObstacleDef *obstacle)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	if(obstacle->Ypos + obstacle->Height <= BSP_LCD_GetYSize())
		BSP_LCD_FillRect((uint16_t)obstacle->Xpos, (uint16_t)obstacle->Ypos, obstacle->Width, obstacle->Height);
}

void MultiObstacle_Draw(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle->Ypos + obstacle->Height <= BSP_LCD_GetYSize())
			BSP_LCD_FillRect((uint16_t)obstacle[i].Xpos, (uint16_t)obstacle[i].Ypos, obstacle[i].Width, obstacle[i].Height);
	}
}
