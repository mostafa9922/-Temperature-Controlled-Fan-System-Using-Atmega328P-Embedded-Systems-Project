#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "BitMath.h"
#include "ADC.h"
#include "Timer.h"
#include "UART.h"

#define Max_Temp 110
#define Min_Temp 0

unsigned char MODE_STATUS = Automatic_Mode;// Default Mode (Automatic)
char data[10];// Received Data
int j = 0;// Counter in ISR
char received_Flag = 0;      
char Occurrences_Flag = 0;     
char Disp_value[10];

// Declare a variable to store the previous temperature value
int prev_Temp_Value = -1; // Initialize with an invalid value

int main(void) {
	char *endptr; // Pointer for error handling
	double Temp_Value;

	CLR_BIT(DDRD, DDD2);// PIN INPUT For Button
	SET_BIT(PORTD, PORTD2);// Activate Pull-Up Resistance (Active-Low)

	// Enable external interrupt on INT0 (PD2)
	SET_BIT(EIMSK, INT0);// Enable INT0 (External Interrupt 0)
	
	CLR_BIT(EICRA, ISC00);// Trigger interrupt on falling edge (button press)
	SET_BIT(EICRA, ISC01);
	
	ADC_INIT();
	Timer0_INIT();
	UART_INIT();
	sei(); // Enable global interrupts

	while (1) {
		// Automatic Mode
		if (MODE_STATUS == Automatic_Mode) 
		{
			j = 0;
			data[0] = 0;
			if (Occurrences_Flag == 0) // To display message for the first time
			{
				UART_SEND_DATA("Automatic Mode Is On !\n");
				Occurrences_Flag = 1;
			}
			Temp_Value = ADC_START();
			int scaled_Temp = (int)(Temp_Value * Max_Temp); // Scale and convert to integer
			if(scaled_Temp !=prev_Temp_Value)// Check if the temperature has changed
			{ 
				prev_Temp_Value = scaled_Temp;// Update the previous temperature
				UART_SEND_DATA("Temperature: ");
				itoa(scaled_Temp, Disp_value, 10);// Convert the integer temperature to a string
				UART_SEND_DATA(Disp_value);// Send the temperature value
				UART_SEND_DATA("\n");
				Timer0_Start(Temp_Value);// Adjust LED brightness
			}
		}
		// Manual Mode
		else {
			prev_Temp_Value = -1;
			if (Occurrences_Flag == 1) // To display message for the first time
			{ 
				UART_SEND_DATA("Manual Mode Is On!\n");
				Occurrences_Flag = 0;
			}
			if (received_Flag == 1) // Check if data is ready
			{ 
				Temp_Value = strtod(data, &endptr); // convert to double
				if (*endptr != '\0' || data[0] == '\0')  // Invalid input
				{
					UART_SEND_DATA("Invalid input\n");
				}
				else
				{
					if (Temp_Value > Max_Temp)
					{
						Temp_Value = Max_Temp;
					}
					else if (Temp_Value < Min_Temp)
					{
						Temp_Value = Min_Temp;
					}
					itoa((int)Temp_Value, data, 10); // Convert to string
					UART_SEND_DATA("Temperature: ");
					UART_SEND_DATA(data);
					UART_SEND_DATA("\n");
					Temp_Value = Temp_Value / Max_Temp;
					Timer0_Start(Temp_Value);
				}
				// Initialize after manual mode 
				j = 0;
				data[0] = 0;
				received_Flag = 0;
			}
		}
	}
}

ISR(INT0_vect) {
	// Toggle modes
	while(GET_BIT(PIND,PIND2) == 0);
	MODE_STATUS ^= 1;
}

ISR(USART_RX_vect) {
	char received_char = UDR0; // Read the received character
	// Check for end of message
	if ((received_char == '\n' || received_char == '\r') && MODE_STATUS == 0) 
	{ 
		data[j] = '\0'; // Null-terminate the string
		received_Flag = 1;     // Set flag to indicate data is ready
		} else {
		data[j] = received_char; // Store the character
		j++;
	}
}
