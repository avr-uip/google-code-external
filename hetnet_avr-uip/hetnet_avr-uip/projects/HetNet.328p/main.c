#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>

#include "timer.h"
#include "global-conf.h"
#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "uart.h"
#include "onewire.h"

#include <string.h>
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

/* === MACROS AND DATA ===*/
/* --- LedPort handling ---*/
#define LED_bm (1<<PORTC0)
#define LEDPORT PORTC
#define LEDPORT_D DDRC
#define led_conf()    LEDPORT_D |= LED_bm
#define led_on() 	    LEDPORT |= LED_bm
#define led_off()     LEDPORT &= ~LED_bm
#define led_toggle()  LEDPORT ^= LED_bm

/* --- Protothread and timers ---*/
static struct pt blink_thread;
static struct timer blink_timer;
static struct pt distance_thread;
static struct timer distance_timer;
/* --- measured distance ---*/
uint16_t range=0;
int setup;


/* === PROTOTHREADS  ===*/
/* ---  Blink protothread ---*/
static 
PT_THREAD(blink(void))
{
	PT_BEGIN(&blink_thread);

		led_on();
		printf("led ON\n");
		timer_set(&blink_timer, CLOCK_CONF_SECOND);
		PT_WAIT_UNTIL(&blink_thread, 
				timer_expired(&blink_timer));

		led_off();
		printf("Led OFF\n");
		timer_set(&blink_timer, CLOCK_CONF_SECOND);
		PT_WAIT_UNTIL(&blink_thread,
				timer_expired(&blink_timer));

	PT_END(&blink_thread);
}

/* --- Distance protothread ---*/
PT_THREAD(read_distance(uint8_t setup))
{
	/* --- Start continuous reading --- */
	PT_BEGIN(&distance_thread);

	if (setup){
		/* Setup ADC if requested */
		ADMUX  |= _BV(REFS1) | _BV(REFS0);
		ADMUX  |= _BV(MUX0);
		ADCSRA |= _BV(ADEN);

		timer_set(&distance_timer, (uint8_t)
				(CLOCK_CONF_SECOND/2));
		PT_WAIT_UNTIL(&distance_thread, 
			timer_expired(&distance_timer));
	}

	/* --- reading timer --- */
	timer_set(&distance_timer, CLOCK_CONF_SECOND);
	PT_WAIT_UNTIL(&distance_thread, 
		timer_expired(&distance_timer));

	/* --- get value frome ADC and convert ---
	 * 	- start conversion
	 * 	- wait for conversion end
	 * 	- clear interrupt flag
	 */
	ADCSRA |= _BV(ADSC);
	while ( (ADCSRA & _BV(ADIF))){;}
	ADCSRA |= _BV(ADIF);
 	range =  ((ADCL) | ((ADCH&0x03)<<8));
	/* Let's do the math:
	 * 1 inch = 2.54 cm
	 * ( (Vcc/512)=6.4mV)\1 inch  6.4mV\2.54 cm
	 * distance = adc / (6.4/2.54) = adc * (2.54/6.4)
	 *			   ~= adc*0.40
	 *
	 * Conversion is made by js page because of gcc
	 * bug (__clz_tab linking)
	 */
	printf("Range %d\n", range);

	PT_END(&distance_thread);
}
	


/* === MAIN === */
int main(int argc, char *argv[])
{
	/*9600, 2x, @8Mhz*/
	uint16_t brate = 103; 
	int i;
	uip_ipaddr_t ipaddr;
	struct uip_eth_addr 
		mac = {UIP_ETHADDR0, UIP_ETHADDR1, 
			UIP_ETHADDR2, UIP_ETHADDR3,
			UIP_ETHADDR4, UIP_ETHADDR5};
	struct timer periodic_timer, arp_timer;

	/*--- device setup ---*/
	clock_init();
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	usart_init(brate);
	usart_redirect_stdout();
	printf("\n\n****************************\n\n");
	printf("Starting uIP \n");
	printf("Configuring clock \n");
	led_conf();
	printf("Configuring led \n");
	network_init();
	printf("Configuring network \n");
	uip_init();
	printf("Configuring uIP stack \n");

	printf("Set EHT mac Address\n");
	uip_setethaddr(mac);

	simple_ajax_init();
	printf("Configuring HTTPD daemon \n");
	
  uip_ipaddr(ipaddr, 192,168,5,10);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,5,5);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

	PT_INIT(&blink_thread);
	PT_INIT(&distance_thread);
	blink();
	read_distance(1);

	while(1){
		blink();
		read_distance(0);
		uip_len = network_read();

		if(uip_len > 0) {
			if(BUF->type == htons(UIP_ETHTYPE_IP)){
				uip_arp_ipin();
				uip_input();
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}else if(BUF->type == htons(UIP_ETHTYPE_ARP)){
				uip_arp_arpin();
				if(uip_len > 0){
					network_send();
				}
			}

		}else if(timer_expired(&periodic_timer)) {
			timer_reset(&periodic_timer);

			for(i = 0; i < UIP_CONNS; i++) {
				uip_periodic(i);
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}

			#if UIP_UDP
			for(i = 0; i < UIP_UDP_CONNS; i++) {
				uip_udp_periodic(i);
				if(uip_len > 0) {
					uip_arp_out();
					network_send();
				}
			}
			#endif /* UIP_UDP */

			if(timer_expired(&arp_timer)) {
				timer_reset(&arp_timer);
				uip_arp_timer();
			}
		}
	}
}
