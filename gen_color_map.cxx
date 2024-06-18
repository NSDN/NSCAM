#include <stdio.h>
#include <stdint.h>
#include <math.h>

uint32_t hsv_rgb(float h, float s, float v) {
    h = fmodf(h, 360);
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60, 2) - 1));
    float m = v - c;
    float rt, gt, bt;
    if (h >= 0 && h < 60) {
        rt = c; gt = x; bt = 0;
    } else if (h >= 60 && h < 120) {
        rt = x; gt = c; bt = 0;
    } else if (h >= 120 && h < 180) {
        rt = 0; gt = c; bt = x;
    } else if (h >= 180 && h < 240) {
        rt = 0; gt = x; bt = c;
    } else if (h >= 240 && h < 300) {
        rt = x; gt = 0; bt = c;
    } else if (h >= 300 && h < 360) {
        rt = c; gt = 0; bt = x;
    } else {
        rt = gt = bt = 0;
    }

    return (uint32_t) ((rt + m) * 255) << 16 | (uint32_t) ((gt + m) * 255) << 8 | (uint32_t) ((bt + m) * 255);
}

uint16_t rgb_565(uint32_t color) {
	uint16_t c;
	c =  (((color & 0xFF0000) >> 16) & 0xFF) >> 3;
	c <<= 6;
	c |= (((color & 0x00FF00) >> 8 ) & 0xFF) >> 2;
	c <<= 5;
	c |= (((color & 0x0000FF)      ) & 0xFF) >> 3;

	return c >> 8 | c << 8;
}

#define SEN_LCD_MAP_SIZ 1024
volatile uint16_t sensor_lcd_map[SEN_LCD_MAP_SIZ];
static void gen_sensor_lcd_map() {
    for (uint16_t i = 0; i < SEN_LCD_MAP_SIZ; i++) {
        float k = (float) i / (float) SEN_LCD_MAP_SIZ;
        //k = 256.0f * k;//pow((k - 0.1) * 1.2, 1 / 2.2);
        //if (k < 0) k = 0;
        //if (k > 255) k = 255;
        //uint32_t c = (uint8_t) ((int) k);
        //printf("%f->%d\n", k, c);
        //sensor_lcd_map[i] = rgb_565(c << 16 | c << 8 | c);

        sensor_lcd_map[i] = rgb_565(hsv_rgb((1 - k) * 300.0f, 1, 1));
    }
}

void main() {
    gen_sensor_lcd_map();
    int j = 0;
    for (int i = 0; i < SEN_LCD_MAP_SIZ; i++) {
        printf("0x%04X, ", sensor_lcd_map[i]);
        j += 1;
        if (j >= 16) {
            printf("\n");
            j = 0;
        }
    }
}
