/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON
 
  include your name there and license for distribution.
 
  Remove next text: This line should not appear in submitted
  work and project name should be change to match real application.
  If this text is there I want 10 points subtracted from final
  evaluation.
 
 *******************************************************************/
 
#define _POSIX_C_SOURCE 200112L
#define STATE_SETTING 0
#define STATE_READY 1
#define STATE_RUNNING 2
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "graphic.h"

#include "serialize_lock.h"

 
int main(int argc, char *argv[]) {
	printf("Hello World\n");
	void *spiled_base;
	void *parlcd_base;
	
	spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	// parlcd_hx8357_init(parlcd_base);
	
	struct fbuf *fb = fb_init();
	draw_background(fb, 0x3f);
	fb_update(fb);
	uint32_t led_state, knobs_state;
	uint8_t r_speed, g_time, state = STATE_SETTING;
	bool r_set_speed, g_set_time, b_start;
	bool r_ready = false, g_ready = false, b_running = false;

	while(1) {
		knobs_state = *(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
		r_speed = (knobs_state >> 16) & 0xff;
		g_time = (knobs_state >> 8) & 0xff;
		r_set_speed = (knobs_state >> 26) & 1;
		g_set_time = (knobs_state >> 25) & 1;
		b_start = (knobs_state >> 24) & 1;

		// Change 32 bit LED
		led_state = 1;
		for(int i = 0; i <= r_speed>>3; ++i){
			led_state *= 2;
		}
		led_state -= 1;
		// printf("r = %d, led_state = %d\n", r, led_state);
		*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_LINE_o) = led_state;
		
		switch(state) {
		case STATE_SETTING:
			break;
		case STATE_READY:
			break;
		case STATE_RUNNING:
			break;
		}

		fb_update(fb);
	}
  
 
  printf("Goodbye\n");
 
  return 0;
}
