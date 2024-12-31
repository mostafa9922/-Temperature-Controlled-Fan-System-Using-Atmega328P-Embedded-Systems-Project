#include <avr/io.h>

#include "BitMath.h"

void ADC_INIT(){
	CLR_BIT(DDRC,DDC0); // Make PINC0 INPUT
	ADCSRA = 0x87; // Enable ADC and select ck/128
	ADMUX = 0x40; // AVCC, ADC0 single ended input, data will be right-justified
}
double ADC_START(){
	double Copy_Result;
	SET_BIT(ADCSRA,ADSC); // start conversion
	while (GET_BIT(ADCSRA,ADIF) != 1); // wait for conversion to finish
	Copy_Result = ADC;
	SET_BIT(ADCSRA,ADIF); // clear the ADIF flag by writing 1 on it
	Copy_Result /= 1023; // Make It Percentage Value (e.g. 0.5)
	
	return Copy_Result;
}
