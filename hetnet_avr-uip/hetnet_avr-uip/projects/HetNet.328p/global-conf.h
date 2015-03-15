//Project specific configurations
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

  #include <stdio.h>

/*
 * === uIP Config data ===
 */
/*No uIP reassembly*/
#define UIP_REASSEMBLY 0
//Mac adress definition for enc28j60
#define ETHADDR0		0x00
#define ETHADDR1		0xbd
#define ETHADDR2		0x3b
#define ETHADDR3		0x33
#define ETHADDR4		0x05
#define ETHADDR5		0x71
//Mac adress definition for uip
#define UIP_ETHADDR0    ETHADDR0
#define UIP_ETHADDR1    ETHADDR1
#define UIP_ETHADDR2    ETHADDR2
#define UIP_ETHADDR3    ETHADDR3
#define UIP_ETHADDR4    ETHADDR4
#define UIP_ETHADDR5    ETHADDR5
/*Ip Address for uIP*/
#define UIP_IPADDR0	192
#define UIP_IPADDR1	168
#define UIP_IPADDR2	5
#define UIP_IPADDR3	10
/*Destination address for uIP*/
#define UIP_DRIPADDR0 192
#define UIP_DRIPADDR1 168
#define UIP_DRIPADDR2 5
#define UIP_DRIPADDR3 5
/*Netmask for uIP*/
#define UIP_NETMASK0 255
#define UIP_NETMASK1 255
#define UIP_NETMASK2 255
#define UIP_NETMASK3 0

/*
 * === SPI Port data ===
 */
// ENC28J60 SPI port
#define ENC28J60_SPI_PORT		PORTB
#define ENC28J60_SPI_DDR		DDRB
// pre ATMega328p 
#ifdef PB5
#define ENC28J60_SPI_SCK		PB5
#define ENC28J60_SPI_MOSI		PB3
#define ENC28J60_SPI_MISO		PB4
#define ENC28J60_SPI_SS			PB2
#define ENC28J60_CONTROL_CS		PB2
#else
#define ENC28J60_SPI_SCK        PORTB5
#define ENC28J60_SPI_MOSI       PORTB3
#define ENC28J60_SPI_MISO       PORTB4
#define ENC28J60_SPI_SS         PORTB2
#define ENC28J60_CONTROL_CS      PORTB2
#endif
// ENC28J60 control port
#define ENC28J60_CONTROL_PORT	PORTB
#define ENC28J60_CONTROL_DDR	DDRB

//Include uip.h gives all the uip configurations in uip-conf.h
#include "uip.h"

#endif /*__GLOBAL_CONF_H__*/
