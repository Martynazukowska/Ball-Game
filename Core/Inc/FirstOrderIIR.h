/*
 * FirstOrderIIR.h
 *
 *  Created on: 13 maj 2022
 *      Authors: Dawid Łukasiewicz and Martyna Żukowska
 */

#ifndef INC_FIRSTORDERIIR_H_
#define INC_FIRSTORDERIIR_H_

typedef struct
{
	float alpha;
	float beta_0;
	float beta_1;

	float out;
	float previous_in;
}FirstOrderIIR_t;

int FirstOrderIIR_Init(FirstOrderIIR_t *filter, float alpha, float beta_0, float beta_1);

float FirstOrderIIR_Update(FirstOrderIIR_t *filter, float in);

#endif /* INC_FIRSTORDERIIR_H_ */
