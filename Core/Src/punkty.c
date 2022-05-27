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
	point->zdobyty=0;
}

void Item_Overflow(Item *point, uint8_t NumberOfPoints)
{
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(point[i].Ypos <= 0)
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
	}
}

void Item_OverflowRandom(Item *point, uint8_t NumberOfPoints, uint16_t width_limit)
{
	int16_t X, width;
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(point[i].Ypos <= 0)
		{
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
			X = rand() % width_limit + 40;
			if(BSP_LCD_GetXSize() - X < 50)
				width = 10;
			else
				width =10;
			Item_Init(&point[i], X, point[i].Ypos, width, point[i].Height);
		}
	}
}

void ParityItem_OverflowRandom(Item *point, uint8_t NumberOfPoints, uint16_t width_limit)
{
	uint16_t width,X;
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		X = rand() % width_limit + 40;
		if(point[i].Ypos <= 0)
		{
			point[i].Ypos = BSP_LCD_GetYSize() - point[i].Height;
			if(i % 2 == 0)
			{
				width = 10;
				Item_Init(&point[i], X, point[i].Ypos, width, point[i].Height);
			}
			else
			{
				width = 10;
				Item_Init(&point[i], BSP_LCD_GetXSize() - width - X, point[i].Ypos, width, point[i].Height);
			}
		}
	}
}

void SingleItem_Move(Item *point, int16_t X, int16_t Y)
{
	point->Xpos += X;
	point->Ypos += Y;
}

void MultiItem_Move(Item *point, uint8_t NumberOfPoints, int16_t X, int16_t Y)
{
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		SingleItem_Move(&point[i], X, Y);
	}
}

void SingleItem_Draw(Item *point)
{
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	if(point->zdobyty==0)
	{
		if(point->Ypos + point->Height <= BSP_LCD_GetYSize())
			BSP_LCD_FillRect((uint16_t)point->Xpos, (uint16_t)point->Ypos, point->Width, point->Height);
	}
}

void MultiItem_Draw(Item *point, uint8_t NumberOfPoints)
{
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(point[i].zdobyty==0)
		{
			if(point[i].Ypos + point[i].Height <= BSP_LCD_GetYSize())
				BSP_LCD_FillRect((uint16_t)point[i].Xpos, (uint16_t)point[i].Ypos, point[i].Width, point[i].Height);
		}
	}
}

int length_2(int16_t X, int16_t Y)
{
	return sqrt( (X * X) + (Y * Y) );
}

int IfScore(Item *point, uint8_t NumberOfPoints, int16_t X_ball, int16_t Y_ball, uint16_t Ray)
{

	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{

		if(length_2(point[i].Xpos - X_ball							, point[i].Ypos - Y_ball) 							< Ray)
		{
			point[i].zdobyty=1;
			return 1;
		}

		if(length_2(point[i].Xpos + point[i].Width - X_ball		, point[i].Ypos - Y_ball)							< Ray)
		{
			point[i].zdobyty=1;
			return 1;
		}

		if(length_2(point[i].Xpos - X_ball							, point[i].Ypos + point[i].Height - Y_ball)		< Ray)
		{
			point[i].zdobyty=1;
			return 1;
		}

		if(length_2(point[i].Xpos + point[i].Width - X_ball		, point[i].Ypos + point[i].Height - Y_ball) 		< Ray)
		{
			point[i].zdobyty=1;
			return 1;
		}

		if(Y_ball + Ray > point[i].Ypos && Y_ball - Ray < point[i].Ypos)
		{
			if(X_ball > point[i].Xpos && X_ball < point[i].Xpos + point[i].Width)
			{
				point[i].zdobyty=1;
				return 1;
			}
		}
	}
	return 0;
}

