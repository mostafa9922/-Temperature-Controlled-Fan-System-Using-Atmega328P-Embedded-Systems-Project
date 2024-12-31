#include <avr/io.h>
#include "BitMath.h"

void Timer0_INIT(){
	// Using PIN A
	// FAST PWM MODE ,  255 TOP Value
	SET_BIT(TCCR0A,WGM00);
	SET_BIT(TCCR0A,WGM01);
	CLR_BIT(TCCR0B,WGM02);
	
	// Ck=>1024
	SET_BIT(TCCR0B,CS00);
	CLR_BIT(TCCR0B,CS01);
	SET_BIT(TCCR0B,CS02);
	
	// Compare Output Mode  Clear Mode (Non-Inverting Mode) 
	CLR_BIT(TCCR0A,COM0A0);
	SET_BIT(TCCR0A,COM0A1);
	
	// Set OC0A (PD6) as output
	SET_BIT(DDRD, DDD6);
}

void Timer0_Start(double Duty_Cycle_Value){

	OCR0A = Duty_Cycle_Value * 255;
}