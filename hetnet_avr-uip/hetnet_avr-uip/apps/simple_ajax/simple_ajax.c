#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "uip.h"
#include "simple_ajax.h"
#include "http-strings.h"

const char http_get[5] = 
/* "GET " */
{0x47, 0x45, 0x54, 0x20, };

static int handle_connection(struct simple_httpd_state *s);
static char to_write[10];

static  
PT_THREAD(handle_output(struct simple_httpd_state *s))
{
	PSOCK_BEGIN(&s->sockout);

	if (s->state == STATE_OUTPUT_START){
		printf("html content\n");
		/* Write WEB Page Here*/
  	PSOCK_SEND_STR(&s->sockout, "HTTP/1.0 200 OK\r\n");
  	PSOCK_SEND_STR(&s->sockout, "Content-Type: text/html\r\n");
  	PSOCK_SEND_STR(&s->sockout, "\r\n");
		PSOCK_SEND_STR(&s->sockout, "<html><head><title>:: A web page in an AVR ::</title> </head><body>\r\n");
		PSOCK_SEND_STR(&s->sockout, "<script type=\"text/javascript\">\r\n");
		PSOCK_SEND_STR(&s->sockout, "var http = false;\n");
		PSOCK_SEND_STR(&s->sockout, "http = new XMLHttpRequest();\r\n");
		PSOCK_SEND_STR(&s->sockout, "function mul(val) {res = parseInt(val); if(res>0){ return \"Distance is: \"+res*0.4+\" cm\";}else{return \"Out of bounds\"}};\r\n");
		PSOCK_SEND_STR(&s->sockout, "function replace() {\r\n");
		PSOCK_SEND_STR(&s->sockout, "http.open(\"GET\", \"192.168.5.10/?args=dist\", true);\r\n");
		PSOCK_SEND_STR(&s->sockout,"http.onreadystatechange=function() {\nif(http.readyState == 4) {\ndocument.getElementById('foo').innerHTML = mul(http.responseText);\n}\n}\nhttp.send(null);\r\n");
		PSOCK_SEND_STR(&s->sockout,	"setTimeout (\"replace()\", 500)};\n</script>");
		PSOCK_SEND_STR(&s->sockout,	"<script type=\"text/javascript\">\n setTimeout (\"replace()\", 1000);\r\n</script>\r\n");
		PSOCK_SEND_STR(&s->sockout, "<div id=\"foo\">\nDistance is: --\n</div>\r\n");
		PSOCK_SEND_STR(&s->sockout, "</body></html>\r\n");
	}else if (s->state == STATE_OUTPUT_TEMP){
		printf("Temp Content\n");
  	PSOCK_SEND_STR(&s->sockout, "HTTP/1.0 200 OK\r\n");
		PSOCK_SEND_STR(&s->sockout, "Content-Type: text/plain\r\n");
  	PSOCK_SEND_STR(&s->sockout, "\r\n");

		memset(to_write, 0, sizeof(to_write));
		sprintf(to_write, "Temperature is: %d\r\n", range);
		PSOCK_SEND_STR(&s->sockout, to_write);
	}else if (s->state == STATE_OUTPUT_DIST){
		printf("Dist Content\n");
  	PSOCK_SEND_STR(&s->sockout, "HTTP/1.0 200 OK\r\n");
		PSOCK_SEND_STR(&s->sockout, "Content-Type: text/html\r\n");
  	PSOCK_SEND_STR(&s->sockout, "\r\n");

		memset(to_write, 0, sizeof(to_write));
		if (range < 200 && range > 20)
			sprintf(to_write, "%d\r\n", range);
		else 
			sprintf(to_write, "0\r\n");

		PSOCK_SEND_STR(&s->sockout, to_write);
		PSOCK_SEND_STR(&s->sockout, "</body></html>\r\n");
	}


	PSOCK_CLOSE(&s->sockout);
	s->state=DATA_SENT;
	PSOCK_END(&s->sockout);

}
 
static 
PT_THREAD(handle_input(struct simple_httpd_state *s))
{
  PSOCK_BEGIN(&s->sockin);

  PSOCK_READTO(&s->sockin, ISO_space);
//	printf("---Buffin---\n%s\n----End Buffin----\n\n", s->buffin);
  
  if(strncmp(s->buffin, http_get, 4) != 0) {
    PSOCK_CLOSE_EXIT(&s->sockin);
  }
	/*here we get the GET parameter (to next space)*/
  PSOCK_READTO(&s->sockin, ISO_space);

  if(s->buffin[0] != ISO_slash) {
    PSOCK_CLOSE_EXIT(&s->sockin);
  }

	/* Parse the string until we find the '=' char*/
	char *get_arg = (char *) &s->buffin;
	while ( *(get_arg++) != '=' );
	//printf("string parsed: %s\n", get_arg);
  
	/* Determinate the output state*/
	if ( !strncmp(get_arg, GET_REQ_TEMP,
				strlen(GET_REQ_TEMP) )){
		s->state = STATE_OUTPUT_TEMP;
		printf("Temp State\n");

	}else if ( !strncmp(get_arg, GET_REQ_DIST,
				strlen(GET_REQ_DIST) )){
		s->state = STATE_OUTPUT_DIST;
		printf("Dist State\n");

	}else{
		s->state = STATE_OUTPUT_START;
		printf("Default State\n");
	}

  while(1) {
    PSOCK_READTO(&s->sockin, ISO_nl);
  }

  PSOCK_END(&s->sockin);
}

void simple_ajax_init(void)
{
	uip_listen(HTONS(80));
}

#if defined PORT_APP_MAPPER
void simple_httpd_appcall(void)
{
	struct simple_httpd_state *s = &(httpd_state_list[0]);
#else
void simple_httpd_appcall(void)
{
	struct simple_httpd_state *s = &(uip_conn->appstate);
#endif
	if (uip_closed() || uip_aborted() || uip_timedout()){
	}else if(uip_connected()) {
		printf("Connected\n");
		/*
		 *  - Init protosocket
		 *  - Init input and output threads
		 *  - Init range finder sensor thread
		 */
		PSOCK_INIT(&s->sockin, s->buffin, sizeof(s->buffin));
		PSOCK_INIT(&s->sockout, s->buffout, sizeof(s->buffout));

		PT_INIT(&s->handle_output);
		PT_INIT(&s->handle_input);

		s->state = CONNECTED;
  }else if(s != NULL){
		if (uip_poll())
			{;} /*implement a timer*/
 		handle_connection(s);
	}else
		uip_abort();
}

int
handle_connection(struct simple_httpd_state *s)
{	
	if (uip_aborted() || uip_timedout() || uip_closed()) {;
	}
	else if (uip_rexmit()){
		//printf("rexmit\n");
	}else if (uip_newdata()){
		//printf("newdata\n");
	}else if (uip_acked()){
	//	printf("ack\n");
	}else if (uip_connected()){
	//	printf("connected 2\n");
	}else if (uip_poll()){
		//printf("poll\n");
	}

  handle_input(s);
  if(s->state >= STATE_OUTPUT) {
    handle_output(s);
 }
	return 0;
}
