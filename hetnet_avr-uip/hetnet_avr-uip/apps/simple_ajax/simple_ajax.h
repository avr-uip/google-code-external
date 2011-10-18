#ifndef __SIMPLE_HTTPD_H__
#define __SIMPLE_HTTPD_H__
#include "psock.h"

#define CONNECTED 0x00
#define DATA_SENT 0x01
#define DATA_IN 0x02
/*OPUTPUT state identifiers*/
#define STATE_OUTPUT 0x03 
#define STATE_OUTPUT_TEMP 0x04
#define STATE_OUTPUT_DIST 0x05
#define STATE_OUTPUT_START 0x06

/*Argouments of GET request*/
#define GET_REQ_TEMP "temp"
#define GET_REQ_DIST "dist"
#define GET_REQ_MERDA "merda"

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_percent 0x25
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

struct simple_httpd_state {
	/*data fields*/
	uint8_t buffin[100];
	uint8_t buffout[100];
	uint8_t state;
	/*psock and pthread*/
	struct pt handle_output, handle_input;
	struct psock sockin, sockout;
};

extern uint16_t range;


void simple_ajax_init(void);
void simple_httpd_init(void);

#if defined PORT_APP_MAPPER
	#define SIMPLE_HTTPD_APP_CALL_MAP {simple_httpd_appcall, 80, 0},
	struct simple_httpd_state httpd_state_list[UIP_CONF_MAX_CONNECTIONS];
#else
	#define SIMPLE_HTTPD_APP_CALL_MAP
	#define UIP_APPCALL simple_httpd_appcall
	typedef struct simple_httpd_state uip_tcp_appstate_t;
#endif

#endif /* __SIMPLE_HTTPD_H__ */
