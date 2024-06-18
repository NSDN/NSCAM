#include "gl.h"

#include "lcd.h"
extern LCD* lcd;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t disp_buf_1[LCD_WIDTH * 10];
static lv_disp_drv_t disp_drv;
static lv_disp_t* disp;

static void _gl_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    lcd->bitmapf(lcd->p, area->x1, area->y1, area->x2, area->y2, (uint16_t*) color_p);
    lv_disp_flush_ready(disp_drv);
}

void glInit() {
    lv_init();
    lv_disp_draw_buf_init(&disp_buf, disp_buf_1, NULL, LCD_WIDTH * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = &_gl_flush_cb;
    disp = lv_disp_drv_register(&disp_drv);
}

void glUpdate() {
    lv_task_handler();
}
