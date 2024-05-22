#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "graphic.h"

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

void draw_background(struct fbuf *fb, int c) {
	int x, y, i = 0;
	char title[10] = {'M', 'Y', ' ', 'B', 'L', 'E', 'N', 'D', 'E', 'R'};
	
	// draw frame
	for(y = 0; y < 320; ++y) {
		for(x = 0; x < 480; ++x, ++i) {
			if((y > 19 && y < 40) || (y > 279 && y < 300)) {
				if (x > 19 && x < 460) {
					fb->data[i] = c;
				}
				else {
					fb->data[i] = 0;
				}
			}
			else if (y > 39 && y < 280) {
				if ((x > 19 && x < 40) || (x > 439 && x < 460)) {
					fb->data[i] = c;
				}
				else {
					fb->data[i] = 0;
				}
			}
			else {
				fb->data[i] = 0;
			}
		}
	}

	// draw title
	for(i = 0; i < 10; ++i) {
		if(i == 2) continue;
		fb_draw_char(fb, 200 + 8 * i, 80, &font_rom8x16, title[i], c);
	}
}

void draw_cur_time(struct fbuf *fb, int t) {
}