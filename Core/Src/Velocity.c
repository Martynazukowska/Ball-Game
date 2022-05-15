/*
 * Velocity.c
 *
 *  Created on: May 15, 2022
 *      Author: Dawid Łukasiewicz
 */

#include "Velocity.h"

void VelocityStructure_Init(Velocity_t * velocity)
{
	velocity->time_now = 0;
	velocity->time_previous = 0;
	velocity->speed = 0.0f;
}

float VelocityStructure_Update(Velocity_t * velocity, float acceleration)
{
	velocity->time_previous = velocity->time_now;
	velocity->time_now = HAL_GetTick()/100;
	velocity->speed = acceleration * (velocity->time_now - velocity->time_previous) + velocity->speed*0.9;
	return velocity->speed;
}
