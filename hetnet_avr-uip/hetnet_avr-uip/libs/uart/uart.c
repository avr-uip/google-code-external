#include "global-conf.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>

volatile unsigned int buffercounter = 0;

char usart_rx_buffer[50];
char *rx_buffer_pointer_in	= &usart_rx_buffer[0];
char *rx_buffer_pointer_out	= &usart_rx_buffer[0];
	
//http://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html
//Maybe use setbaud instead



void usart_init(uint16_t brate)
{ 
	/*--- setup serial ---*/
	/*baudrate*/
	BAUDH = (uint8_t) (brate>>8);
	BAUDL = (uint8_t) (brate);
	/*enable transmitter and receiver*/
	CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
	/*double speed*/
	CTRLA |= USART_DOUBLE_SPEED_bm;
	/*chsize 8, 1 stop bit*/
	CTRLC |= USART_8CH_SIZE_gm;
}

void 
usart_send(unsigned char c){
	if (c == '\n') c='\r';
	while(!(CTRLA & USART_DATA_EMPTY_bm)) ;
	DATA = c;
	return;
}

void
usart_print(char *buf){
	while (*buf)
		usart_send(*buf++);

	return;
}

/* === USE USART AS STDOUT === */
int rs232_stdout_putchar(char c, FILE *stream);
static FILE usart_stdout = FDEV_SETUP_STREAM(rs232_stdout_putchar,
					     NULL,
					     _FDEV_SETUP_WRITE);

int rs232_stdout_putchar(char c, FILE *stream)
{
  usart_send (c);
  return 0;
}
void usart_redirect_stdout () {
  stdout = &usart_stdout;
}
