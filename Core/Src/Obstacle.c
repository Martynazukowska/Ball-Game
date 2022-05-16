/*
 * Obstacle.c
 *
 *  Created on: 16 maj 2022
 *      Author: Dawid Łukasiewicz
 */

#include "Obstacle.h"

void Obstacle_Init(ObstacleDef *obstacle, uint16_t X, uint16_t Y, uint16_t width, uint16_t height)
{
	if(X >= BSP_LCD_GetXSize() || X + width> BSP_LCD_GetXSize())
		return;
	if(Y >= BSP_LCD_GetYSize() || Y + height> BSP_LCD_GetYSize())
		return;

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

void SingleObstacle_Move(ObstacleDef *obstacle, int X, int Y)
{
//	if(obstacle->Xpos + X > BSP_LCD_GetXSize() || obstacle->Ypos + Y > BSP_LCD_GetYSize())
//		return;
	obstacle->Xpos += X;
	obstacle->Ypos += Y;
}

void MultiObstacle_Move(ObstacleDef *obstacle, uint8_t NumberOfObjects, int X, int Y)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		SingleObstacle_Move(&obstacle[i], X, Y);
	}
}

int IfCollisionDetect(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	return 0;
}

void SingleObstacle_Draw(ObstacleDef *obstacle)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	BSP_LCD_FillRect(obstacle->Xpos, obstacle->Ypos, obstacle->Width, obstacle->Height);
}

void MultiObstacle_Draw(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		BSP_LCD_FillRect(obstacle[i].Xpos, obstacle[i].Ypos, obstacle[i].Width, obstacle[i].Height);
	}
}
