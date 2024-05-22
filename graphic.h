#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fbuf {
	void *parlcd_base;
	int w, h;
	uint16_t *data;
};

struct fbuf *fb_init();

void fb_clear(struct fbuf *, int);

void fb_update(struct fbuf *);

void fb_draw_pixel(struct fbuf *, int, int, int);

int fb_draw_char(struct fbuf *, int, int, font_descriptor_t *, char, int);

void draw_background(struct fbuf *, int);

void draw_cur_time(struct fbuf *, int, int);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /*GRAPHIC_H*/
