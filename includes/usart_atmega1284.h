
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef USART_ATMEGA1284_H
#define USART_ATMEGA1284_H

// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

////////////////////////////////////////////////////////////////////////////////
//Functionality - Initializes TX and RX on PORT D
//Parameter: None
//Returns: None
void initUSART(unsigned char usartNum)
{
	// Turn on the reception circuitry
	// Use 8-bit character sizes - URSEL bit set to select the UCRSC register
	// Turn on receiver and transmitter
	if(usartNum == 0)
	{
		UCSR0B = (1 << RXEN0)  | (1 << TXEN0);
		UCSR0C = (1 << USBS0) | (3 << UCSZ00);
		// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
		UBRR0L = (unsigned char)BAUD_PRESCALE;
		// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
		UBRR0H = (unsigned char)(BAUD_PRESCALE >> 8);
	}
	else if(usartNum == 1)
	{
		UCSR1B = (1 << RXEN1)  | (1 << TXEN1);
		UCSR1C = (1 << USBS1) | (3 << UCSZ10);
		// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
		UBRR1L = (unsigned char)BAUD_PRESCALE;
		// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
		UBRR1H = (unsigned char)(BAUD_PRESCALE >> 8);
	}
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART is ready to send
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_IsSendReady(unsigned char usartNum)
{
	if(usartNum == 0){
		return (UCSR0A & (1 << UDRE0));
	}
	else if(usartNum == 1)
	{
		return (UCSR1A & (1 << UDRE1));
	}
	else
	{
		return 0x00;
	}

}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasTransmitted(unsigned char usartNum)
{
	if(usartNum == 0){
		return (UCSR0A & (1 << TXC0));
	}
	else if(usartNum == 1)
	{
		return (UCSR1A & (1 << TXC1));
	}
	else
	{
		return 0x00;
	}
	

}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasReceived(unsigned char usartNum)
{
	if(usartNum == 0){
		return (UCSR0A & (1 << RXC0));
	}
	else if(usartNum == 1)
	{
		return (UCSR1A & (1 << RXC1));
	}
	else
	{
		return 0x00;
	}
	
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the data register
//Parameter: None
//Returns: None
void USART_Flush(unsigned char usartNum)
{
	if(usartNum == 0){
		static unsigned char dummy;
		while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
	}
	else if(usartNum == 1)
	{
		static unsigned char dummy;
		while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
	}

}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - Sends an 8-bit char value
//Parameter: Takes a single unsigned char value
//Returns: None
void USART_Send(unsigned char sendMe, unsigned char usartNum)
{
	if(usartNum == 0){
		while( !(UCSR0A & (1 << UDRE0)) );
		UDR0 = sendMe;
	}
	else if(usartNum == 1)
	{
		while(!(UCSR1A & (1 << UDRE1)));
		UDR1 = sendMe;
	}
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - receives an 8-bit char value
//Parameter: None
//Returns: Unsigned char data from the receive buffer
unsigned char USART_Receive(unsigned char usartNum)
{
	if(usartNum == 0)
	{
		while ( !(UCSR0A & (1 << RXC0)) ); // Wait for data to be received
		return UDR0; // Get and return received data from buffer
	}
	else if(usartNum == 1)
	{
		while ( !(UCSR1A & (1 << RXC1)) ); // Wait for data to be received
		return UDR1; // Get and return received data from buffer
	}
	else
	{
		return 0x00;
	}
}

#endif //USART_ATMEGA1284_H