/*
 * bigpoint.c
 *
 *  Created on: 31 maj 2022
 *      Author: Dawid Łukasiewicz and Martyna Żukowska
 */

#include "bigpoint.h"
#include "math.h"


void BItem_Init(BigItem *Bpoint, int16_t X, int16_t Y, uint16_t width, uint16_t height)
{

	Bpoint->Xpos = X;
	Bpoint->Ypos = Y;
	Bpoint->Width = width;
	Bpoint->Height = height;
	Bpoint->zdobyty=0;
}

void BItem_Overflow(BigItem *Bpoint, uint8_t NumberOfPoints)
{
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(Bpoint[i].Ypos <= 0)
			Bpoint[i].Ypos = BSP_LCD_GetYSize() - Bpoint[i].Height;
	}
}

void BItem_OverflowRandom(BigItem *Bpoint, uint8_t NumberOfPoints, uint16_t width_limit)
{
	int16_t X, width;
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(Bpoint[i].Ypos <= 0)
		{
			Bpoint[i].Ypos = BSP_LCD_GetYSize() - Bpoint[i].Height;
			X = rand() % width_limit + 40;
			if(BSP_LCD_GetXSize() - X < 50)
				width = 10;
			else
				width =10;
			BItem_Init(&Bpoint[i], X, Bpoint[i].Ypos, width, Bpoint[i].Height);
		}
	}
}

void ParityBItem_OverflowRandom(BigItem *Bpoint, uint8_t NumberOfPoints, uint16_t width_limit)
{
	uint16_t width,X;
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		X = rand() % width_limit + 40;
		if(Bpoint[i].Ypos <= 0)
		{
			Bpoint[i].Ypos = BSP_LCD_GetYSize() - Bpoint[i].Height;
			if(i % 2 == 0)
			{
				width = 10;
				BItem_Init(&Bpoint[i], X, Bpoint[i].Ypos, width, Bpoint[i].Height);
			}
			else
			{
				width = 10;
				BItem_Init(&Bpoint[i], BSP_LCD_GetXSize() - width - X, Bpoint[i].Ypos, width, Bpoint[i].Height);
			}
		}
	}
}

void SingleBItem_Move(BigItem *Bpoint, int16_t X, int16_t Y)
{
	Bpoint->Xpos += X;
	Bpoint->Ypos += Y;
}

void MultiBItem_Move(BigItem *Bpoint, uint8_t NumberOfPoints, int16_t X, int16_t Y)
{
	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		SingleBItem_Move(&Bpoint[i], X, Y);
	}
}

void SingleBItem_Draw(BigItem *Bpoint)
{
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	if(Bpoint->zdobyty==0)
	{
		if(Bpoint->Ypos + Bpoint->Height <= BSP_LCD_GetYSize())
			BSP_LCD_FillRect((uint16_t)Bpoint->Xpos, (uint16_t)Bpoint->Ypos, Bpoint->Width, Bpoint->Height);
	}
}

void MultiBItem_Draw(BigItem *Bpoint, uint8_t NumberOfPoints)
{
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);

	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{
		if(Bpoint[i].zdobyty==0)
		{
			if(Bpoint[i].Ypos + Bpoint[i].Height <= BSP_LCD_GetYSize())
				BSP_LCD_FillRect((uint16_t)Bpoint[i].Xpos, (uint16_t)Bpoint[i].Ypos, Bpoint[i].Width, Bpoint[i].Height);
		}
	}
}

int length_3(int16_t X, int16_t Y)
{
	return sqrt( (X * X) + (Y * Y) );
}

int IfScoreB(BigItem *Bpoint, uint8_t NumberOfPoints, int16_t X_ball, int16_t Y_ball, uint16_t Ray)
{

	for(uint8_t i = 0; i < NumberOfPoints; ++i)
	{

		if(Bpoint[i].zdobyty==0 && length_3(Bpoint[i].Xpos - X_ball							, Bpoint[i].Ypos - Y_ball) 							< Ray)
		{
			Bpoint[i].zdobyty=1;
			return 1;
		}

		if(Bpoint[i].zdobyty==0 && length_3(Bpoint[i].Xpos + Bpoint[i].Width - X_ball		, Bpoint[i].Ypos - Y_ball)							< Ray)
		{
			Bpoint[i].zdobyty=1;
			return 1;
		}

		if(Bpoint[i].zdobyty==0 && length_3(Bpoint[i].Xpos - X_ball							, Bpoint[i].Ypos + Bpoint[i].Height - Y_ball)		< Ray)
		{
			Bpoint[i].zdobyty=1;
			return 1;
		}

		if(Bpoint[i].zdobyty==0 && length_3(Bpoint[i].Xpos + Bpoint[i].Width - X_ball		, Bpoint[i].Ypos + Bpoint[i].Height - Y_ball) 		< Ray)
		{
			Bpoint[i].zdobyty=1;
			return 1;
		}

		if(Bpoint[i].zdobyty==0 && Y_ball + Ray > Bpoint[i].Ypos && Y_ball - Ray < Bpoint[i].Ypos)
		{
			if(X_ball > Bpoint[i].Xpos && X_ball < Bpoint[i].Xpos + Bpoint[i].Width)
			{
				Bpoint[i].zdobyty=1;
				return 1;
			}
		}
	}
	return 0;
}

