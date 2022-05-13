/*
 * FirstOrderIIR.c
 *
 *  Created on: 13 maj 2022
 *      Authors: Dawid Łukasiewicz and Martyna Żukowska
 */

#include "FirstOrderIIR.h"

int FirstOrderIIR_Init(FirstOrderIIR_t *filter, float alpha)
{
	if(alpha > 1.0f || alpha < 0.0f)
		return -1;

	filter->alpha = alpha;
	filter->out = 0.0f;
	filter->previous_in = 0.0f;

	return 0;
}

float FirstOrderIIR_Update(FirstOrderIIR_t *filter, float in)
{
	filter->out = filter->alpha * (in - filter->previous_in + filter->out);
	filter->previous_in = in;

	return filter->out;
}
