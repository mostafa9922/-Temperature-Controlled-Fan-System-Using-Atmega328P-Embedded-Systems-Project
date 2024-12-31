#include <avr/io.h>
#include "BitMath.h"



void UART_INIT()
{
	SET_BIT(UCSR0B,RXEN0); // To Enable Receiver
	SET_BIT(UCSR0B,TXEN0); // To Enable Transmitter
	
	// Mode Select (Asynchronous)
	CLR_BIT(UCSR0C,UMSEL00);
	CLR_BIT(UCSR0C,UMSEL01);
	
	// Character Size => 8 BIT
	SET_BIT(UCSR0C,UCSZ00);
	SET_BIT(UCSR0C,UCSZ01);
	CLR_BIT(UCSR0B,UCSZ02);
	
	// Baud Rate = 9600bps
	UBRR0L = 103;
	
	// Enable Interrupt 
	SET_BIT(UCSR0B,RXCIE0);
}

void UART_SEND(char Value)
{
	while (! (UCSR0A & (1<<UDRE0))); //wait until UDR0 is empty
		UDR0 = Value;
}

void UART_SEND_DATA(char data[]){
	unsigned char i = 0;
	for(i = 0; data[i] != '\0';i++)
	{
		UART_SEND(data[i]);
	}
}


