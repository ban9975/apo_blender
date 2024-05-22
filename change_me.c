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
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

#include "serialize_lock.h"

struct fbuf {
	void *parlcd_base;
	int w, h;
	uint16_t *data;
};

struct fbuf *fb_init() {
	struct fbuf *fb;
	fb = malloc(sizeof(struct fbuf));
	if(!fb) {
		printf("frame buffer allocation failed\n");
		exit(1);
	}
	fb->parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	
	fb->w = 480;
	fb->h = 320;
	fb->data = malloc(fb->w * fb->h * sizeof(uint16_t));
	if(!fb->data) {
		printf("frame buffer datat allocation failed\n");
		exit(1);
	}
	return fb;
}

void fb_clear(struct fbuf *fb, int color) {
	int l = fb->w * fb->h;
	for(int i = 0; i < l; ++i) {
		fb->data[i] = color;
	}
}

void fb_update(struct fbuf *fb) {
	int l = fb->w * fb->h;
	parlcd_write_cmd(fb->parlcd_base, 0x2c);
	for(int i = 0; i < l; ++i) {
		parlcd_write_data(fb->parlcd_base, fb->data[i]);
	}
}

void fill_lcd(void *parlcd_base, int color) {
	int x, y;
	// 480x320
	parlcd_write_cmd(parlcd_base, 0x2c);
	for(y = 0; y < 320; ++y) {
		for(x = 0; x < 480; ++x) {
			if(x > 20 && x < 460 && y > 20 && y < 300) {
				parlcd_write_data(parlcd_base, color);
			}
			else {
				parlcd_write_data(parlcd_base, 0);
			}
		}
	}
}

void fb_draw_pixel(struct fbuf *fb, int x, int y, int c) {
	if(x < 0 || x >= fb->w)
		return;
	if(y < 0 || y >= fb->h)
		return;
	fb->data[x + y * fb->w] = c;
}

int fb_draw_char(struct fbuf *fb, int x, int y, font_descriptor_t *font, char ch, int c) {
	int h, w, i, j;
	font_bits_t *p;
	int m;
	
	h = font->height;
	w = font->maxwidth;
	if(ch < font->firstchar)
		return 0;
	ch -= font->firstchar;
	if(ch >= font->size)
		return 0;
	if(font->width)
		w = font->width[ch];
		
	m = (w + 15) / 16;
	
	if(font->offset) {
		p = font->bits + font->offset[ch];
	}
	else {
		p = font->bits + m * h * ch;
	}
	
	for(j = 0; j < h; ++j) {
		for(i = 0; i < w; ++i) {
			if((i & 0xf) == 0)
				m = *(p++);
			if(m & 0x8000) {
				fb_draw_pixel(fb, x + i, y + j, c);
				// printf("draw pixel x+i %d, y+j %d, c %d\n", x + i, y + j, c);
			}
			m <<= 1;
		}
	}
	return w;
}
 
int main(int argc, char *argv[]) {
	printf("Hello World\n");
	void *spiled_base;
	void *parlcd_base;
	
	spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	// parlcd_hx8357_init(parlcd_base);
	
	struct fbuf *fb = fb_init();
	fb_clear(fb, 0);
	fb_update(fb);
	uint32_t led_state = 1;
	uint8_t word = 0;

	while(1) {
		uint32_t rgb888, rgb565;
		uint8_t r, g, b;
		
		rgb888 = *(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
		r = (rgb888 >> 16) & 0xff;
		g = (rgb888 >> 8) & 0xff;
		b = rgb888 & 0xff;
		
		// Change 32 bit LED
		led_state = 1;
		for(int i = 0; i <= r>>3; ++i){
			led_state *= 2;
		}
		led_state -= 1;
		// printf("r = %d, led_state = %d\n", r, led_state);
		*(volatile uint32_t*)(spiled_base + SPILED_REG_LED_LINE_o) = led_state;
		
		// Draw Character
		rgb565 = (b >> 3) | ((g >> 2) << 5) | ((r >> 3) << 11);
		printf("Draw pixels r%d, g%d, b%d\n", r >> 3, g >> 2, b >> 3);
		fb_clear(fb, 0);
		
		if(word < 25) {
			++word;
		}
		else {
			word = 0;
		}
		fb_draw_char(fb, 0, 0, &font_rom8x16, 'A' + word, rgb565);
		fb_update(fb);
		
		sleep(1);
		// fill_lcd(parlcd_base, rgb565);		
	}
  
 
  printf("Goodbye\n");
 
  return 0;
}
