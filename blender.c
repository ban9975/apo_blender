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
	void *spiled_base;
	void *parlcd_base;
	void *dcspdrv_base;
	
	spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	dcspdrv_base = map_phys_address(DCSPDRV_REG_BASE_PHYS_0, DCSPDRV_REG_SIZE, 0);
	*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_CR_o) = DCSPDRV_REG_CR_PWM_ENABLE_m;
	*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_PERIOD_o) = 5000;
	*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_DUTY_o) = 0 | DCSPDRV_REG_DUTY_DIR_A_m;
	// parlcd_hx8357_init(parlcd_base);
	
	struct fbuf *fb = fb_init();
	draw_background(fb, 0xff00);
	fb_update(fb);
	uint32_t led_state, knobs_state;
	uint8_t r_speed = 0, r_knob, g_knob, state = STATE_SETTING;
	time_t start_time, g_time = 0, running_time;
	uint32_t motor_speed;
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
		g_knob = g_knob * 60 / 256;

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
				motor_speed = 500 + r_speed * 4500 / 256;
				*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_DUTY_o) = motor_speed | DCSPDRV_REG_DUTY_DIR_A_m;
				// start timer
				start_time = time(NULL);
				running_time = g_time;
				b_started = true;
				b_start = false;
				sleep(1);
			}
			// count down
			// printf("time pass = %d\n", time(NULL) - start_time);
			g_time = running_time - (time(NULL) - start_time);

			if(g_time <= 0) {
				// finish
				g_time = 0;
				state = STATE_SETTING;
				r_ready = false;
				g_ready = false;
				b_started = false;
				*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_DUTY_o) = 0 | DCSPDRV_REG_DUTY_DIR_A_m;
			}
			else if(b_start) {
				// printf("Stopped by user\n");
				// stopped by user
				state = STATE_SETTING;
				r_ready = false;
				g_ready = false;
				b_started = false;
				*(volatile uint32_t *)(dcspdrv_base + DCSPDRV_REG_DUTY_o) = 0 | DCSPDRV_REG_DUTY_DIR_A_m;
			}
			break;
		default:
			break;
		}
		// printf("r_knob = %d, g_knob = %d, r_speed = %d, g_time = %d\n", r_knob, g_knob, r_speed, g_time);
		// printf("r_ready = %d, g_ready = %d\n", r_ready, g_ready);

		// Change 32 bit LED
		led_state = 1;
		for(int i = 0; i <= r_knob>>3; ++i){
			led_state *= 2;
		}
		led_state -= 1;
		*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_LINE_o) = led_state;
		
		// Show time
		if(!g_ready) {
			draw_cur_time(fb, g_knob, 0xf000);
		}
		else {
			draw_cur_time(fb, g_time, 0xff00);
		}
		fb_update(fb);
		//sleep(2);
	}  
 
  return 0;
}
