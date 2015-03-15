#include <avr/io.h>
#include <util/delay.h>

#define LEDDIR	DDRC
#define LEDPORT PORTC
#define LED_bm (1<<PORTC0)

int main(){
	LEDDIR |= LED_bm;

	for(;;){	
		LEDPORT |= LED_bm;
		_delay_ms(1000);                  // wait for a second
		LEDPORT &= ~LED_bm;
		_delay_ms(1000);                  // wait for a second
	}	
}
