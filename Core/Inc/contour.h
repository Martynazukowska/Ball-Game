/*
 * contour.h
 *
 *  Created on: May 23, 2022
 *      Author: Martyna Żukowska and Dawid Łukasiewicz
 */

#include "point.h"

#ifndef INC_CONTOUR_H_
#define INC_CONTOUR_H_



typedef struct
{
	point up_right;
	point up_left;
	point down_right;
	point down_left;
}contour;

float Contour_Init(contour *obrys, point up_right, point up_left, point down_right, point down_left);

#endif /* INC_CONTOUR_H_ */
