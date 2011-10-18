/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 								 Jacopo Mondi (2011)
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    RS232 Routinen

 Dieses Programm ist freie Software. Sie k�nnen es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation ver�ffentlicht, 
 weitergeben und/oder modifizieren, entweder gem�� Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder sp�teren Version. 

 Die Ver�ffentlichung dieses Programms erfolgt in der Hoffnung, 
 da� es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 F�R EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/
#include	<avr/io.h>

#ifndef _UART_H
	#include <stdio.h>
	#define _UART_H

	#define USART_ECHO	1
	#ifndef SYSCLK
		#define SYSCLK F_CPU
	#endif //SYSCLK	

/*--- PIN name definition ---*/
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	/*register names*/
	#define DATA UDR0
	#define CTRLA	UCSR0A
	#define CTRLB UCSR0B
	#define CTRLC UCSR0C
	#define BAUDH UBRR0H
	#define BAUDL UBRR0L
	/*bitmasks*/
	#define USART_DATA_EMPTY_bm (1<<UDRE0)
	#define USART_RXEN_bm	(1<<RXEN0)
	#define USART_TXEN_bm	(1<<TXEN0)
	#define USART_DOUBLE_SPEED_bm (1<<U2X0)
	/*groupmasks*/
	#define USART_ASYNC_gm (0<<UMSEL00) /*placeholder*/
	#define USART_SYNC_gm	(1<<UMSEL00)
	#define USART_SPI_MASTER_gm	(3<<UMSEL00)
	#define USART_EVEN_PARITY_gm (1<<UPM01)
	#define USART_ODD_PARITY_gm (3<<UPM00)
	#define USART_DOUBLE_STOP_BIT_gm (1<<USBS0)
	#define USART_5CH_SIZE_gm (0<0) /*placeholder*/
	#define USART_6CH_SIZE_gm	(1<<UCSZ00)
	#define USART_7CH_SIZE_gm	(2<<UCSZ00)
	#define USART_8CH_SIZE_gm (3<<UCSZ00)
	#define USART_9CH_SIZE_gm (7<<UCSZ00)
#else
#error Define you pin mapping here
#endif

/*--- prototypes ---*/
void usart_init(uint16_t);
void usart_redirect_stdout(void);
void usart_print(char*);
#endif //_UART_H
