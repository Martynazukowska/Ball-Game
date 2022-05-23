/*
 * point.h
 *
 *  Created on: May 23, 2022
 *      Author: Martyna Żukowska and Dawid Łukasiewicz
 */

#ifndef SRC_POINT_H_
#define SRC_POINT_H_

typedef struct
{
	float Y;
	float X;
}point;

float Point_Init(point *punkt, float X, float Y);

#endif /* SRC_POINT_H_ */
