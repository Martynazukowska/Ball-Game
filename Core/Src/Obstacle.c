/*
 * Obstacle.c
 *
 *  Created on: 16 maj 2022
 *      Author: Dawid Łukasiewicz and Martyna Żukowska
 */

#include "Obstacle.h"
#include "math.h"

/**
 * @brief Initialazing one obstacle with given parameters
 *
 * @param X coordinate of first node
 *
 * @param Y coordinate of first node
 *
 * @param width of obstacle
 *
 * @param height of obstacle
 *
 * @note 3 other nodes are calculated by BSP_LCD_FillRect function called by SingleObstacle_Draw and MultiObstacle_Draw
 */
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

/**
 * @brief Checking all obstacles if need go to beginning location
 *
 * @param obstacle is address of array of ObstacleDef instance
 *
 * @param NumberOfObjects number of ObstacleDef in the array
 */
void Obstacle_Overflow(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle[i].Ypos <= 0)
			obstacle[i].Ypos = BSP_LCD_GetYSize() - obstacle[i].Height;
	}
}

/**
 * @brief Checking all obstacles if need go to beginning location but giving them randomed width
 *
 * @param obstacle is address of array of ObstacleDef instance
 *
 * @param NumberOfObjects number of ObstacleDef in the array
 *
 * @param width_limit max width for obstacle
 */
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

/**
 * @brief Checking all obstacles if need go to beginning location but giving them randomed width
 *
 * @note obstacles are generated in way to always stick to one of the vertical walls depending on their index in array
 *
 * @param obstacle is address of array of ObstacleDef instance
 *
 * @param NumberOfObjects number of ObstacleDef in the array
 *
 * @param width_limit max width for obstacle
 */
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

/**
 * @brief Moving obstacle by given X and Y parameters
 *
 * @param obstacle is address of one ObstacleDef instance
 *
 * @param X for how many to move from current position in X coordinate
 *
 * @param Y for how many to move from current position in Y coordinate
 */
void SingleObstacle_Move(ObstacleDef *obstacle, int16_t X, int16_t Y)
{
	obstacle->Xpos += X;
	obstacle->Ypos += Y;
}

/**
 * @brief Moving all obstacles in array by given X and Y parameters
 *
 * @param obstacle is address of array for ObstacleDef instances
 *
 * @param X for how many to move from current position in X coordinate
 *
 * @param Y for how many to move from current position in Y coordinate
 */
void MultiObstacle_Move(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X, int16_t Y)
{
	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		SingleObstacle_Move(&obstacle[i], X, Y);
	}
}

/**
 * @brief Drawing an single obstacle if not exceedeing LCD max height
 *
 * @note The obstacle is still in memory if not drawn but not to produce any troubles in displaying
 *
 * @param obstacle is address of one ObstacleDef instance
 */
void SingleObstacle_Draw(ObstacleDef *obstacle)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	if(obstacle->Ypos + obstacle->Height <= BSP_LCD_GetYSize())
		BSP_LCD_FillRect((uint16_t)obstacle->Xpos, (uint16_t)obstacle->Ypos, obstacle->Width, obstacle->Height);
}

/**
 * @brief Drawing all obstacle if not exceedeing LCD max height
 *
 * @note The obstacle is still in memory if not drawn but not to produce any troubles in displaying
 *
 * @param obstacle is address of one ObstacleDef instance
 *
 * @param NumberOfObjects number of ObstacleDef instances in the array
 */
void MultiObstacle_Draw(ObstacleDef *obstacle, uint8_t NumberOfObjects)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		if(obstacle[i].Ypos + obstacle[i].Height <= BSP_LCD_GetYSize())
			BSP_LCD_FillRect((uint16_t)obstacle[i].Xpos, (uint16_t)obstacle[i].Ypos, obstacle[i].Width, obstacle[i].Height);
	}
}


int max( int16_t a, int16_t b)
{
    return (a < b) ? b : a;
}

int min( int16_t a, int16_t b)
{
    return (a < b) ? b : a;
}



int length(int16_t X, int16_t Y)
{
	return sqrt( (X * X) + (Y * Y) );
}

int IfCollisionDetect(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X_ball)
{
	int16_t Ray = 20;
	int16_t Y_ball = 80;

	for(uint8_t i = 0; i < NumberOfObjects; ++i)
	{
		int16_t X1 = obstacle[i].Xpos, X2 = obstacle[i].Xpos + obstacle[i].Width;
		int16_t Y1 = obstacle[i].Ypos, Y2 = obstacle[i].Ypos + obstacle[i].Height;

		int16_t Ball_length = length(X_ball, Y_ball);
		int16_t len1 = length(obstacle[i].Xpos, obstacle[i].Ypos);
		int16_t len2 = length(obstacle[i].Xpos + obstacle[i].Width, obstacle[i].Ypos);
		int16_t len3 = length(obstacle[i].Xpos, obstacle[i].Ypos + obstacle[i].Height);
		int16_t len4 = length(obstacle[i].Xpos + obstacle[i].Width, obstacle[i].Ypos + obstacle[i].Height);
		if(len1 - Ball_length < Ray)
		{
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2, (uint8_t*)"1", CENTER_MODE);
			return 2;
		}
		if(len2 - Ball_length < Ray)
		{
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2, (uint8_t*)"2", CENTER_MODE);
			return 2;
		}
		if(len3 - Ball_length  < Ray)
		{
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2, (uint8_t*)"3", CENTER_MODE);
			return 2;
		}
		if(len4 - Ball_length < Ray)
		{
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2, (uint8_t*)"4", CENTER_MODE);
			return 2;
		}
	}
	return 0;
}

//
//
//
//int IfCollisionDetect(ObstacleDef *obstacle, uint8_t NumberOfObjects, int16_t X_ball)
//{
//  int16_t R= 20;
//	for(uint8_t i = 0; i < NumberOfObjects; ++i)
//	{
//		int16_t obx=obstacle[i].Xpos;
//		int16_t oby=obstacle[i].Ypos;
//		int16_t obw=obstacle[i].Width;
//		int16_t obh=obstacle[i].Height;
//		// center point circle X_ball
//
//	    // calculate Obstacle info (center, half-extents)
//		int16_t Obstacle_half_extents_x = obstacle[i].Width / 2;
//		int16_t Obstacle_half_extents_y = obstacle[i].Height/ 2;
//
//		int16_t Obstacle_center_x=obstacle[i].Xpos + Obstacle_half_extents_x;
//		int16_t Obstacle_center_y=obstacle[i].Ypos + Obstacle_half_extents_y;
//
//
//	    // get difference vector between both centers
//		int16_t difference = sqrt((X_ball - Obstacle_half_extents_x)*(X_ball - Obstacle_half_extents_x) +(80 - Obstacle_half_extents_y)*(80 - Obstacle_half_extents_y));
//
//		int16_t clamped_x = max(-Obstacle_half_extents_x, min(Obstacle_half_extents_x, difference));
//
//		int16_t clamped_y = max(-Obstacle_half_extents_y, min(Obstacle_half_extents_y, difference));
//
//	    // add clamped value to Obstacle_center and we get the value of box closest to circle
//		int16_t closest_x = Obstacle_center_x + clamped_x;
//		int16_t closest_y = Obstacle_center_y + clamped_y;
//
//	    // retrieve vector between center circle and closest point Obstacle and check if length <= radius
//	    difference = sqrt((X_ball - closest_x)*(X_ball - closest_x)+(80 - closest_y)*(80 - closest_y));
//	    if(  difference < R)
//	    {
//	    	return 1;
//	    }
//	}
//	return 0;
//}

