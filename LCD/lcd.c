#include "inc/lcd.h"
#include "inc/fonts.h"
#include "inc/regscreen.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

static uint16_t _buf[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * DO SCK SPI1
 * NCS PB11
 * DC  PB24
 * RST PA21
 * BLK PA9
 * */

void __io_init() {
    // BLK
    GPIOA_ResetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_Slowascent_PP_8mA);
    // RST
    GPIOA_SetBits(GPIO_Pin_21);
    GPIOA_ModeCfg(GPIO_Pin_21, GPIO_Slowascent_PP_8mA);
    // DC
    GPIOB_SetBits(GPIO_Pin_24);
    GPIOB_ModeCfg(GPIO_Pin_24, GPIO_Slowascent_PP_8mA);
    // SPI1
    GPIOB_SetBits(GPIO_Pin_11);
    GPIOB_ModeCfg(GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13, GPIO_Highspeed_PP_8mA);
    SPI1_MasterDefInit();
    R8_SPI1_CLOCK_DIV = FREQ_SYS / NSCAM_FLCD;
    SPI1_DataMode(Mode0_HighBitINFront);

    mDelaymS(5);
}

void _lcd_writeCommand(pLCD* p, uint8_t cmd) {
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_ResetBits(GPIO_Pin_24);
    SPI1_MasterSendByte(cmd);
    GPIOB_SetBits(GPIO_Pin_11);
}

void _lcd_writeData(pLCD* p, uint8_t data) {
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    SPI1_MasterSendByte(data);
    GPIOB_SetBits(GPIO_Pin_11);
}

void _lcd_writeDatas(pLCD* p, uint8_t* data, uint16_t length) {
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    SPI1_MasterTrans(data, length);
    GPIOB_SetBits(GPIO_Pin_11);
}

void _lcd_writeData16(pLCD* p, uint16_t data) {
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    SPI1_MasterTrans((uint8_t*) &data, 2);
    GPIOB_SetBits(GPIO_Pin_11);
}

void _lcd_writeData16s(pLCD* p, uint16_t* data, uint32_t length) {
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    SPI1_MasterTrans((uint8_t*) data, length * 2);
    GPIOB_SetBits(GPIO_Pin_11);
}

void __spi1_blk_trans(uint16_t* data, uint32_t size) {
    const uint32_t blk = 512;
    uint16_t blks = size / blk, last = size % blk;
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    if (blks > 0)
        for (uint16_t i = 0; i < blks; i++)
            SPI1_MasterTrans((uint8_t*) (data + i * blk), blk * 2);
    if (last > 0)
        SPI1_MasterTrans((uint8_t*) (data + blks * blk), last * 2);
    GPIOB_SetBits(GPIO_Pin_11);
}

void __spi1_flash(uint16_t data, uint16_t size) {
    uint16_t len = size * 2;
    uint8_t* pbuf = (uint8_t*) &data;
    uint8_t i = 0;
    R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI1_TOTAL_CNT = len;
    R8_SPI1_INT_FLAG = RB_SPI_IF_CNT_END;
    while (len) {
        if (R8_SPI1_FIFO_COUNT < SPI_FIFO_SIZE) {
            R8_SPI1_FIFO = pbuf[i];
            i = (i == 0) ? 1 : 0;
            len--;
        }
    }
    while( R8_SPI1_FIFO_COUNT != 0 );
}

void _lcd_flashData16(pLCD* p, uint16_t data, uint32_t count) {
    const uint32_t blk = 512;
    uint32_t loop = count / blk, last = count % blk;
    GPIOB_ResetBits(GPIO_Pin_11);
    GPIOB_SetBits(GPIO_Pin_24);
    if (loop > 0)
        for (uint16_t i = 0; i < loop; i++)
            __spi1_flash(data, blk);
    __spi1_flash(data, last);
    GPIOB_SetBits(GPIO_Pin_11);
}

void _lcd_writeReg8(pLCD* p, uint8_t cmd, uint32_t data) {
	_lcd_writeCommand(p, cmd);
	_lcd_writeData(p, data);
}

void _lcd_writeReg32(pLCD* p, uint8_t cmd, uint32_t data) {
	_lcd_writeCommand(p, cmd);
	_lcd_writeData(p, (data >> 24) & 0xFF);
	_lcd_writeData(p, (data >> 16) & 0xFF);
	_lcd_writeData(p, (data >> 8 ) & 0xFF);
	_lcd_writeData(p, (data      ) & 0xFF);
}

void _lcd_setPosition(pLCD* p, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { 
    _lcd_writeReg32(p, ILI9341_CASET, (x1 << 16) | x2);
    _lcd_writeReg32(p, ILI9341_PASET, (y1 << 16) | y2);
}

void _lcd_reset(pLCD* p) {
    GPIOA_ResetBits(GPIO_Pin_21);
	mDelaymS(100);
	GPIOA_SetBits(GPIO_Pin_21);
	mDelaymS(50);
}

void _lcd_backLight(pLCD* p, uint8_t state) {
    if (state)
        GPIOA_SetBits(GPIO_Pin_9);
    else
        GPIOA_ResetBits(GPIO_Pin_9);
}

void _lcd_dispControl(pLCD* p, uint8_t state) {
    if (state)
        _lcd_writeCommand(p, ILI9341_DISPON);
    else
        _lcd_writeCommand(p, ILI9341_DISPOFF);
}

void _lcd_rotate(pLCD* p, uint8_t r) {
	uint8_t t = 0;
	r %= 4;
	p->rotate = r;
	switch (r) {
    case 0:
        t = (MADCTL_MX | MADCTL_BGR);
        p->width = ILI9341_TFTWIDTH;
        p->height = ILI9341_TFTHEIGHT;
        break;
    case 1:
        t = (MADCTL_MV | MADCTL_BGR);
        p->width = ILI9341_TFTHEIGHT;
        p->height = ILI9341_TFTWIDTH;
        break;
    case 2:
        t = (MADCTL_MY | MADCTL_BGR);
        p->width = ILI9341_TFTWIDTH;
        p->height = ILI9341_TFTHEIGHT;
        break;
    case 3:
        t = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        p->width = ILI9341_TFTHEIGHT;
        p->height = ILI9341_TFTWIDTH;
        break;
    }
	_lcd_writeReg8(p, ILI9341_MADCTL, t);
	_lcd_setPosition(p, 0, 0, p->width - 1, p->height - 1);
}

void _lcd_init(pLCD* p) {
    __io_init();

    _lcd_backLight(p, 0);
	_lcd_reset(p);
	
	uint8_t cmd, x, numArgs;
    uint8_t* cmds = getRegValues();
    while ((cmd = *(cmds++)) > 0) {
        x = *(cmds++);
        numArgs = x & 0x7F;
        _lcd_writeCommand(p, cmd);
        _lcd_writeDatas(p, cmds, numArgs);
        cmds += numArgs;
        if (x & 0x80)
            mDelaymS(150);
    }

	_lcd_rotate(p, p->rotate);
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

void _lcd_back_color(pLCD* p, uint32_t color) { p->backColor = rgb_565(color); }

void _lcd_fore_color(pLCD* p, uint32_t color) { p->foreColor = rgb_565(color); }

void _lcd_font(pLCD* p, LCDFont f) { p->Font = f; }

void _lcd_clear(pLCD* p) {
	_lcd_setPosition(p, 0, 0, p->width - 1, p->height - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	_lcd_flashData16(p, p->backColor, p->width * p->height);
	p->ptrX = p->ptrY = 0;
}

float _lcd_abs(float v) {
	return v > 0 ? v : -v;
}

void _lcd_pixel(pLCD* p, uint16_t x, uint16_t y) {  
	_lcd_setPosition(p, x, y, x, y);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	_lcd_writeData16(p, p->foreColor);
}

void _lcd_row(pLCD* p, uint16_t y, uint16_t* data) {
    _lcd_setPosition(p, 0, y, LCD_WIDTH, y);
    _lcd_writeCommand(p, ILI9341_RAMWR);
    __spi1_blk_trans(data, LCD_WIDTH);
}

void _lcd_line(pLCD* p, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	if (x1 == x2) {
		float absY = _lcd_abs(y2 - y1), sig = (y2 - y1) / absY;
		for (float dy = 0; _lcd_abs(dy) <= absY; dy += sig) {
			_lcd_setPosition(p, x1, y1 + dy, x1, y1 + dy);
			_lcd_writeCommand(p, ILI9341_RAMWR);
			_lcd_writeData16(p, p->foreColor);
		}
	} else {
		float k = (float)(y2 - y1) / (float)(x2 - x1), absX = _lcd_abs(x2 - x1);
		float sig = (x2 - x1) / absX;
		for (float dx = 0, dy = 0; _lcd_abs(dx) <= absX; dx += sig, dy += k) {
			for (float dk = 0; _lcd_abs(dk) <= _lcd_abs(k); dk += (k > 0 ? 1 : -1)) {
				_lcd_setPosition(p, x1 + dx, y1 + dy + dk, x1 + dx, y1 + dy + dk);
				_lcd_writeCommand(p, ILI9341_RAMWR);
				_lcd_writeData16(p, p->foreColor);
			}
		}
	}
}

void _lcd_tri(pLCD* p, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t fill) {
	if (fill) {
		uint16_t maxy = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y1 > y3) ? (y2) : ((y2 > y3) ? y2 : y3));
		uint16_t miny = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y1 < y3) ? (y2) : ((y2 < y3) ? y2 : y3));
		uint16_t midy = (y1 == maxy) ? ((y2 == miny) ? y3 : y2) : ((y1 == miny) ? ((y2 == maxy) ? y3 : y2) : y1);
		uint16_t maxx = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x1 > x3) ? (x2) : ((x2 > x3) ? x2 : x3));
		uint16_t minx = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x1 < x3) ? (x2) : ((x2 < x3) ? x2 : x3));
		uint16_t midx = (x1 == maxx) ? ((x2 == minx) ? x3 : x2) : ((x1 == minx) ? ((x2 == maxx) ? x3 : x2) : x1);
		
		float k1, k2; uint16_t xs, xe, tmp;
		k1 = (float)(maxy - miny) / (float)(maxx - minx);
		
		k2 = (float)(midy - miny) / (float)(midx - minx);
		for (uint16_t i = miny; i <= midy - miny; i++) {
			xs = (float)(i - miny) / k1 + (float)minx;
			xe = (float)(i - miny) / k2 + (float)minx;
			if (xe < xs) { tmp = xe; xe = xs; xs = tmp; }
			_lcd_setPosition(p, xs, i, xe, i);
			_lcd_writeCommand(p, ILI9341_RAMWR);
			for (uint16_t j = 0; j <= xe - xs; j++) {
				_lcd_writeData16(p, p->foreColor);
			}
		}
		
		k2 = (float)(maxy - midy) / (float)(maxx - midx);
		for (uint16_t i = midy; i <= maxy - midy; i++) {
			xs = (i - miny) / k1 + minx;
			xe = (i - midy) / k2 + midx;
			if (xe < xs) { tmp = xe; xe = xs; xs = tmp; }
			_lcd_setPosition(p, xs, i, xe, i);
			_lcd_writeCommand(p, ILI9341_RAMWR);
			for (uint16_t j = 0; j <= xe - xs; j++) {
				_lcd_writeData16(p, p->foreColor);
			}
		}
	} else {
		_lcd_line(p, x1, y1, x2, y2);
		_lcd_line(p, x2, y2, x3, y3);
		_lcd_line(p, x3, y3, x1, y1);
	}
}

void _lcd_rect(pLCD* p, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t fill) {
	if (fill) {
		_lcd_setPosition(p, x1, y1, x2, y2);
		_lcd_writeCommand(p, ILI9341_RAMWR);
		_lcd_flashData16(p, p->foreColor,
		(_lcd_abs((uint16_t)x2 - (uint16_t)x1) + 1) * (_lcd_abs((uint16_t)y2 - (uint16_t)y1) + 1));
	} else {
		_lcd_line(p, x1, y1, x2, y1);
		_lcd_line(p, x2, y1, x2, y2);
		_lcd_line(p, x2, y2, x1, y2);
		_lcd_line(p, x1, y2, x1, y1);
	}
}

void _lcd_bitmap(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* data) {
	_lcd_setPosition(p, x, y, x + w - 1, y + h - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	uint32_t c = 0;
	for (uint16_t i = 0; i < w; i++) {
		for (uint16_t j = 0; j < h; j++) {
			_lcd_writeData16(p, data[c]);
			c += 1;
		}
	}
}

void _lcd_bitmapf(pLCD* p, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* data) {
    _lcd_setPosition(p, x1, y1, x2, y2);
    _lcd_writeCommand(p, ILI9341_RAMWR);
    __spi1_blk_trans(data, (x2 - x1 + 1) * (y2 - y1 + 1));
}

void _lcd_bitmapc(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* data) {
	_lcd_setPosition(p, x - w / 2, y - h / 2, x + w / 2 - 1, y + h / 2 - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	uint32_t c = 0;
	for (uint16_t i = 0; i < w; i++) {
		for (uint16_t j = 0; j < h; j++) {
			_lcd_writeData16(p, data[c]);
			c += 1;
		}
	}
}

void _lcd_bitmapt(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t trans, uint16_t* data) {
	_lcd_setPosition(p, x, y, x + w - 1, y + h - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	uint32_t c = 0; uint32_t colort = rgb_565(trans & 0xFFFFFF);
	for (uint16_t i = 0; i < w; i++) {
		for (uint16_t j = 0; j < h; j++) {
			if (data[c] != colort) _lcd_writeData16(p, data[c]);
			c += 1;
		}
	}
}

void _lcd_bitmaptc(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t trans, uint16_t* data) {
	_lcd_setPosition(p, x - w / 2, y - h / 2, x + w / 2 - 1, y + h / 2 - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	uint32_t c = 0; uint32_t colort = rgb_565(trans & 0xFFFFFF);
	for (uint16_t i = 0; i < w; i++) {
		for (uint16_t j = 0; j < h; j++) {
			if (data[c] != colort) _lcd_writeData16(p, data[c]);
			c += 1;
		}
	}
}

void _lcd_bitmaps(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char* data) {
	uint32_t c = 0;
	for (uint16_t m = 0; m < h; m += 8) {
		for (uint16_t n = 0; n < w; n += 8) {
			for (uint8_t i = 0; i < 8; i++) {
				for (uint8_t j = 0; j < 8; j++)
					if (data[c] & (1 << j))
						_buf[i + j * 8] = p->foreColor;
					else 
						_buf[i + j * 8] = p->backColor;
				c++;
			}
			_lcd_setPosition(p, x + n, y + m, x + n + 7, y + m + 7);
			_lcd_writeCommand(p, ILI9341_RAMWR);
			_lcd_writeData16s(p, _buf, 64);
		}
	}
}

void _lcd_bitmapsc(pLCD* p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char* data) {
	_lcd_bitmaps(p, x - w / 2, y - h / 2, w, h, data);
}

void _lcd_draw(pLCD* p, uint16_t x, uint16_t y, char character) {
	char c = character - ' ';
	if (p->Font == Big) {
		if (x >= p->width) { x = 0; y = y + 16; }
		for (uint8_t i = 0; i < 8; i++) {
			for (uint8_t j = 0; j < 8; j++) {
				if (getFont(1)[c * 16 + i] & (1 << j))
					_buf[i + j * 8] = p->foreColor;
				else 
					_buf[i + j * 8] = p->backColor;
			}
		}
		for (uint8_t i = 0; i < 8; i++) {
			for (uint8_t j = 0; j < 8; j++) {
				if (getFont(1)[c * 16 + i + 8] & (1 << j))
					_buf[i + j * 8 + 64] = p->foreColor;
				else 
					_buf[i + j * 8 + 64] = p->backColor;
			}
		}
		_lcd_setPosition(p, x, y, x + 7, y + 15);
		_lcd_writeCommand(p, ILI9341_RAMWR);
		_lcd_writeData16s(p, _buf, 128);
	} else {
		if (x >= p->width) { x = 0; y = y + 8; }
		for (uint8_t i = 0; i < 6; i++) {
			for (uint8_t j = 0; j < 8; j++) {
				if (getFont(0)[c *6 + i] & (1 << j))
					_buf[i + j * 6] = p->foreColor;
				else 
					_buf[i + j * 6] = p->backColor;
			}
		}
		_lcd_setPosition(p, x, y, x + 5, y + 7);
		_lcd_writeCommand(p, ILI9341_RAMWR);
		_lcd_writeData16s(p, _buf, 48);
	}
}

void _lcd_scroll(pLCD* p, uint16_t pos) {
	_lcd_setPosition(p, 0, 0, p->width - 1, p->height - 1);
	_lcd_writeCommand(p, ILI9341_RAMWR);
	_lcd_flashData16(p, p->backColor, p->width * p->height);
	if (pos > 60) pos = 60;
	if (p->Font == Big) {
		for (uint16_t i = 0; i < p->height / 16 - pos; i++) {
			memcpy(p->buffer[i], p->buffer[i + pos], IOBUF_WIDTH);
			for (uint16_t j = 0; p->buffer[i][j] != '\0'; j++) {
				if (j >= IOBUF_WIDTH) break;
				_lcd_draw(p, j * 8, i * 16, p->buffer[i][j]);
			}
		}
		memset(p->buffer[p->height / 16 - pos], 0, IOBUF_WIDTH * pos);
	} else {
		for (uint16_t i = 0; i < p->height / 8 - pos; i++) {
			memcpy(p->buffer[i], p->buffer[i + pos], IOBUF_WIDTH);
			for (uint16_t j = 0; p->buffer[i][j] != '\0'; j++) {
				if (j >= IOBUF_WIDTH) break;
				_lcd_draw(p, j * 6, i * 8, p->buffer[i][j]);
			}
		}
		memset(p->buffer[p->height / 8 - pos], 0, IOBUF_WIDTH * pos);
	}
}

void _lcd_printa_(pLCD* p, char* string) {
	int i = 0;
	if (p->Font == Big) {
		while (string[i] != '\0') {
			if (p->ptrY > p->height - 16) {
				p->ptrX = 0;
				p->ptrY = p->height - 16;
				_lcd_scroll(p, 1);
			}
			if (string[i] == '\n') {
				i++;
				p->ptrX = 0;
				p->ptrY += 16;
				continue;
			}
			if (string[i] == 0x08) {
				i++;
				if (p->ptrX >= 8) p->ptrX -= 8;
				else {
					p->ptrX = p->width - 8;
					p->ptrY -= 16;
				}
				_lcd_draw(p, p->ptrX, p->ptrY, ' ');
				continue;
			}
			_lcd_draw(p, p->ptrX, p->ptrY, string[i]);
			p->buffer[p->ptrY / 16][p->ptrX / 8] = string[i];
			p->ptrX += 8;
			if (p->ptrX > p->width - 8) {
				p->ptrX = 0;
				p->ptrY += 16;
			}
			i++;
		}
		if (string[i] == '\0') p->buffer[p->ptrY / 16][p->ptrX / 8] = string[i];
	} else {
		while (string[i] != '\0') {
			if (p->ptrY > p->height - 8) {
				p->ptrX = 0;
				p->ptrY = p->height - 8;
				_lcd_scroll(p, 1);
			}
			if (string[i] == '\n') {
				i++;
				p->ptrX = 0;
				p->ptrY += 8;
				continue;
			}
			if (string[i] == 0x08) {
				i++;
				if (p->ptrX >= 6) p->ptrX -= 6;
				else {
					p->ptrX = p->width - 6;
					p->ptrY -= 8;
				}
				_lcd_draw(p, p->ptrX, p->ptrY, ' ');
				continue;
			}
			_lcd_draw(p, p->ptrX, p->ptrY, string[i]);
			p->buffer[p->ptrY / 8][p->ptrX / 6] = string[i];
			p->ptrX += 6;
			if (p->ptrX >= p->width - 6) {
				p->ptrX = 0;
				p->ptrY += 8;
			}
			i++;
		}
		if (string[i] == '\0') p->buffer[p->ptrY / 8][p->ptrX / 6] = string[i];
	}
}

void _lcd_print(pLCD* p, uint16_t x, uint16_t y, char* string) {
	int i = 0;
	if (p->Font == Big) {
		while (string[i] != '\0') {
			if (y > p->height - 16) {
				x = 0;
				y = 0;
				_lcd_clear(p);
			}
			if (string[i] == '\n') {
				i++;
				x = 0;
				y += 16;
				continue;
			}
			if (string[i] == 0x08) {
				i++;
				if (x >= 8) x -= 8;
				else {
					x = p->width - 8;
					y -= 16;
				}
				_lcd_draw(p, x, y, ' ');
				continue;
			}
			_lcd_draw(p, x, y, string[i]);
			x += 8;
			if (x > p->width - 8) {
				x = 0;
				y += 16;
			}
			i++;
		}
	} else {
		while (string[i] != '\0') {
			if (y > p->height - 8) {
				x = 0;
				y = 0;
				_lcd_clear(p);
			}
			if (string[i] == '\n') {
				i++;
				x = 0;
				y += 8;
				continue;
			}
			if (string[i] == 0x08) {
				i++;
				if (x >= 6) x -= 6;
				else {
					x = p->width - 6;
					y -= 8;
				}
				_lcd_draw(p, x, y, ' ');
				continue;
			}
			_lcd_draw(p, x, y, string[i]);
			x += 6;
			if (x >= p->width - 6) {
				x = 0;
				y += 8;
			}
			i++;
		}
	}
}

int _lcd_printf(pLCD* p, uint16_t x, uint16_t y, const char* format, ...) {
	char* iobuf = malloc(sizeof(char) * IOBUF_SIZE);
	va_list args;
	va_start(args, format);
	int result = vsprintf(iobuf, format, args);
	va_end(args);
	_lcd_print(p, x, y, iobuf);
	free(iobuf);
	return result;
}

int _lcd_printfc(pLCD* p, uint16_t y, const char* format, ...) {
	char* iobuf = malloc(sizeof(char) * IOBUF_SIZE);
	va_list args;
	va_start(args, format);
	int result = vsprintf(iobuf, format, args);
	va_end(args);
	uint16_t x = (p->width - strlen(iobuf) * ((p->Font == Big) ? 8 : 6)) / 2;
	_lcd_print(p, x, y, iobuf);
	free(iobuf);
	return result;
}

int _lcd_printfa(pLCD* p, const char* format, ...) {
	char* iobuf = malloc(sizeof(char) * IOBUF_SIZE);
	va_list args;
	va_start(args, format);
	int result = vsprintf(iobuf, format, args);
	va_end(args);
	_lcd_printa_(p, iobuf);
	free(iobuf);
	return result;
}

LCD* LCDInit() {
	pLCD* p = malloc(sizeof(pLCD));
	p->width = ILI9341_TFTWIDTH;
	p->height = ILI9341_TFTHEIGHT;
	p->Font = Small;
	p->backColor = 0x0000;
	p->foreColor = 0xFFFF;
	p->rotate = 3;
	p->ptrX = p->ptrY = 0;
	memset(p->buffer, 0, IOBUF_WIDTH * IOBUF_HEIGHT);
			
	LCD* c = malloc(sizeof(LCD));
	c->p = p;
	#ifdef LCD_USE_PRIVATE_FUN
	c->writeCommand = &_lcd_writeCommand;
	c->writeData = &_lcd_writeData;
	c->writeData16 = &_lcd_writeData16;
	c->setPosition = &_lcd_setPosition;
	#endif
	c->init = &_lcd_init;
	c->reset = &_lcd_reset;
	c->light = &_lcd_backLight;
	c->ctrl = &_lcd_dispControl;
	c->colorb = &_lcd_back_color;
	c->colorf = &_lcd_fore_color;
	c->font = &_lcd_font;
	c->clear = &_lcd_clear;
	c->scroll = *_lcd_scroll;
	c->rotate = &_lcd_rotate;
	c->pixel = &_lcd_pixel;
	c->row = &_lcd_row;
	c->line = &_lcd_line;
	c->tri = &_lcd_tri;
	c->rect = &_lcd_rect;
	c->bitmap = &_lcd_bitmap;
	c->bitmapf = &_lcd_bitmapf;
	c->bitmapc = &_lcd_bitmapc;
	c->bitmapt = &_lcd_bitmapt;
	c->bitmaptc = &_lcd_bitmaptc;
	c->bitmaps = &_lcd_bitmaps;
	c->bitmapsc = &_lcd_bitmapsc;
	c->draw = &_lcd_draw;
	c->print = &_lcd_print;
	c->printf = &_lcd_printf;
	c->printfc = &_lcd_printfc;
	c->printfa = &_lcd_printfa;
	
	return c;
}
