/*******************************************************************
  Blender
 
  (C) Copyright 2024 by Pei-Shin Hwang
      e-mail:   s9960733@gmail.com
 
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
	void *servops2_base;
	
	spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	servops2_base = map_phys_address(SERVOPS2_REG_BASE_PHYS, SERVOPS2_REG_SIZE, 0);
	// parlcd_hx8357_init(parlcd_base);
	
	struct fbuf *fb = fb_init();
	draw_background(fb, 0x3f);
	fb_update(fb);
	uint32_t led_state, knobs_state;
	uint8_t r_speed, g_time, r_knob, g_knob, state = STATE_SETTING;
	time_t start_time;
	bool r_set_speed, g_set_time, b_start;
	bool r_ready = false, g_ready = false, b_started = false;

	while(1) {
		// Read knobs values
		knobs_state = *(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
		r_knob = (knobs_state >> 16) & 0xff;
		g_knob = (knobs_state >> 8) & 0xff;
		r_set_speed = (knobs_state >> 26) & 1;
		g_set_time = (knobs_state >> 25) & 1;
		b_start = (knobs_state >> 24) & 1;

		// convert values
		g_knob = g_knob / 256 * 60;

		switch(state) {
		case STATE_SETTING:
			// yellow
			*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_RGB1_o) = 0xffff00;
			if(r_set_speed) {
				r_speed = r_knob;
				r_ready = true;
			}
			else if(r_speed != r_knob) {
				r_ready = false;
			}
			if(g_set_time) {
				g_time = g_knob;
				g_ready = true;
			}
			else if(g_time != g_knob) {
				g_ready = false;
			}
			if(r_ready && g_ready) {
				state = STATE_READY;
			}
			break;
		case STATE_READY:
			// green
			*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_RGB1_o) = 0x00ff00;
			if(r_speed != r_knob) {
				r_ready = false;
				state = STATE_SETTING;
			}
			if(g_time != g_knob) {
				g_ready = false;
				state = STATE_SETTING;
			}
			if(b_start) {
				state = STATE_RUNNING;
			}
			break;
		case STATE_RUNNING:
			// red
			*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_RGB1_o) = 0xff0000;
			// motor running
			if(!b_started) {
				// set motor speed
				// TODO
				*(volatile uint32_t *)(servops2_base + SERVOPS2_REG_CR_o) = 0x10f;
       	*(volatile uint32_t *)(servops2_base + SERVOPS2_REG_PWMPER_o) = 1000000;
				*(volatile uint32_t *)(servops2_base + SERVOPS2_REG_PWM1_o) = r_speed;
				// start timer
				start_time = time(NULL);
				b_started = true;
			}
			// count down
			g_time -= (time(NULL) - start_time);

			if(g_time <= 0) {
				// finish
				g_time = 0;
				state = STATE_SETTING;
				r_ready = false;
				g_ready = false;
				b_started = false;
			}
			else if(b_start) {
				// stopped by user
				state = STATE_SETTING;
				r_ready = false;
				g_ready = false;
				b_started = false;
			}
			break;
		default:
			break;
		}

		// Change 32 bit LED
		led_state = 1;
		for(int i = 0; i <= r_knob>>3; ++i){
			led_state *= 2;
		}
		led_state -= 1;
		// printf("r = %d, led_state = %d\n", r, led_state);
		*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_LINE_o) = led_state;
		
		// Show time
		draw_cur_time(fb, g_time);
		fb_update(fb);
	}
  
 
  printf("Goodbye\n");
 
  return 0;
}
