#include <avr/io.h>
#include <util/delay.h>

#define LEDDIR	DDRC
#define LEDPORT PORTC
#define LED_bm (1<<PORTC0)

int main(int argc, char *argv[]){
	/* 8Mhz, 9600, 2TX speed*/
	int ubrr = 103;

	LEDDIR |= LED_bm;
	
	/*** setup serial ***/
	UBRR0H =  (unsigned char) (ubrr >> 8);
	UBRR0L =  (unsigned char) ubrr; 
	
	/*double USART speed (error go to 0.2% at 9600baud :) */
	UCSR0A |=  (1<<U2X0);
	/*default is 8n1 format, so is ok for us*/
	UCSR0B |=  (1<<TXEN0); /*enable transmitter*/

	for(;;){	
		while ( UCSR0A & (1<<UDRE0) ) /* busy waiting OLEEE */
				UDR0 = 'a';

	}	
}
