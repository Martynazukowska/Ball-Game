/*
 * punkty.c
 *
 *  Created on: 27 maj 2022
*      Author: Dawid Łukasiewicz and Martyna Żukowska
 */

#include "punkty.h"
#include "math.h"


void Item_Init(Item *point, int16_t X, int16_t Y, uint16_t width, uint16_t height)
{

	point->Xpos = X;
	point->Ypos = Y;
	point->Width = width;
	point->Height = height;
}

void Item_Overflow(Item *point, uint8_t NumberOfObjects)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(point[i].Ypos <= 0)
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
	}
}

void Item_OverflowRandom(Item *point, uint8_t NumberOfObjects, uint16_t width_limit)
{
	int16_t X, width;
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(point[i].Ypos <= 0)
		{
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
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
			Item_Init(&point[i], X, point[i].Ypos, width, point[i].Height);
		}
	}
}

void ParityItem_OverflowRandom(Item *point, uint8_t NumberOfObjects, uint16_t width_limit)
{
	uint16_t width;
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(point[i].Ypos <= 0)
		{
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
			if(i % 2 == 0)
			{
				width = (rand() % width_limit) + 20;
				Item_Init(&point[i], 0, point[i].Ypos, width, point[i].Height);
			}
			else
			{
				width = (rand() % width_limit) + 20;
				Item_Init(&point[i], BSP_LCD_GetXSize() - width, point[i].Ypos, width, point[i].Height);
			}
		}
	}
}

void SingleItem_Move(Item *point, int16_t X, int16_t Y)
{
	point->Xpos += X;
	point->Ypos += Y;
}

void MultiItem_Move(Item *point, uint8_t NumberOfObjects, int16_t X, int16_t Y)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		SingleItem_Move(&point[i], X, Y);
	}
}

void SingleItem_Draw(Item *point)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	if(point->Ypos + point->Height <= BSP_LCD_GetYSize())
		BSP_LCD_FillRect((uint16_t)point->Xpos, (uint16_t)point->Ypos, point->Width, point->Height);
}

void MultiItem_Draw(Item *point, uint8_t NumberOfObjects)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(point[i].Ypos + point[i].Height <= BSP_LCD_GetYSize())
			BSP_LCD_FillRect((uint16_t)point[i].Xpos, (uint16_t)point[i].Ypos, point[i].Width, point[i].Height);
	}
}

int length_2(int16_t X, int16_t Y)
{
	return sqrt( (X * X) + (Y * Y) );
}

int IfScore(Item *point, uint8_t NumberOfObjects, int16_t X_ball, int16_t Y_ball, uint16_t Ray)
{

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{

		if(length_2(point[i].Xpos - X_ball							, point[i].Ypos - Y_ball) 							< Ray)
		{
			return 1;
		}

		if(length_2(point[i].Xpos + point[i].Width - X_ball		, point[i].Ypos - Y_ball)							< Ray)
		{
			return 1;
		}

		if(length_2(point[i].Xpos - X_ball							, point[i].Ypos + point[i].Height - Y_ball)		< Ray)
		{
			return 1;
		}

		if(length_2(point[i].Xpos + point[i].Width - X_ball		, point[i].Ypos + point[i].Height - Y_ball) 		< Ray)
		{
			return 1;
		}

		if(Y_ball + Ray > point[i].Ypos && Y_ball - Ray < point[i].Ypos)
		{
			if(X_ball > point[i].Xpos && X_ball < point[i].Xpos + point[i].Width)
			{
				return 1;
			}
		}
	}
	return 0;
}
