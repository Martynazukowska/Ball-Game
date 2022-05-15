/*
 * Velocity.h
 *
 *  Created on: May 15, 2022
 *      Author: Dawid Łukasiewicz
 */
#include "stm32f4xx_hal.h"

#ifndef INC_VELOCITY_H_
#define INC_VELOCITY_H_

typedef struct
{
	float speed;
	uint32_t time_now;
	uint32_t time_previous;
}Velocity_t;

void VelocityStructure_Init(Velocity_t * velocity);

float VelocityStructure_Update(Velocity_t * velocity, float acceleration);
#endif /* INC_VELOCITY_H_ */
