/*
   serial.c  - serial com port I/O routines
   Based on code from the AVR documentation
   Note: Uses USART0
*/

#include "serial.h"
#include "avr/io.h"
#include <stdio.h>

static int USART_Transmit(char data , FILE* stream);
static int USART_Receive(FILE* stream );

static FILE stdiostream = FDEV_SETUP_STREAM(USART_Transmit,USART_Receive,_FDEV_SETUP_RW);

static void USART_Init( unsigned int ubrr){
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);
} // USART_Init

static int USART_Transmit(char data , FILE* stream)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
	return 0;
}

static int USART_Receive( FILE* stream )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}

void init_usart(unsigned int baudrate) {
	USART_Init(baudrate);
	stdout = &stdiostream;
	stdin = &stdiostream;
}


