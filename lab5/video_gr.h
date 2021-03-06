#ifndef _LCOM_VIDEO_GR_H_
#define _LCOM_VIDEO_GR_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <minix/driver.h>
#include <sys/mman.h>

void *(vg_init)(uint16_t mode);
/**
 * Draw pixel with specified color at position with x and y coordinates
 */
int set_pixel(uint16_t x, uint16_t y, uint32_t color);
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int vg_draw_img(xpm_image_t img, uint16_t x, uint16_t y);
#endif /* _LCOM_VIDEO_GR_H */
