#include "oled.h"

#include "fonts.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define OLED_DELAY() mDelayuS(1)

void __oled_start() {
    OLED_DAT_SET();
    OLED_CLK_SET(); OLED_DELAY();
    OLED_DAT_CLR(); OLED_DELAY();
}

void __oled_stop() {
    OLED_DAT_CLR();
    OLED_CLK_SET(); OLED_DELAY();
    OLED_DAT_SET(); OLED_DELAY();
}

void __oled_ack() {
    OLED_CLK_CLR();
    OLED_DAT_SET();
    OLED_CLK_SET(); OLED_DELAY();
    for (uint8_t i = 0; i < 0xFF; i++) {
        if (OLED_DAT_GET() == 0)
            break;
        OLED_DELAY();
    }
    OLED_CLK_CLR(); OLED_DELAY();
}

void __oled_nak() {
    OLED_CLK_CLR();
    OLED_DAT_SET();
    OLED_CLK_SET(); OLED_DELAY();
    OLED_CLK_CLR(); OLED_DELAY();
}

void __oled_wr(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        OLED_CLK_CLR(); OLED_DELAY();
        if (data & (1 << (7 - i)))
            OLED_DAT_SET();
        else
            OLED_DAT_CLR();
        OLED_DELAY();
        OLED_CLK_SET(); OLED_DELAY();
    }
    OLED_CLK_CLR();
    OLED_DAT_SET();
    OLED_DELAY();
}

uint8_t __oled_rd() {
    uint8_t data = 0;
    OLED_DAT_SET();
    OLED_CLK_CLR(); OLED_DELAY();
    for (uint8_t i = 0; i < 8; i++) {
        OLED_CLK_SET(); OLED_DELAY();
        uint8_t tmp = 0;
        if (OLED_DAT_GET() != 0)
            tmp = 1 << (7 - i);
        data |= tmp;
        OLED_DELAY();
        OLED_CLK_CLR(); OLED_DELAY();
    }
    return data;
}

void iic_init() {
    /*
     * CLK PA10
     * DAT PA11
     * */
    uint32_t pins = GPIO_Pin_10 | GPIO_Pin_11;
    GPIOA_SetBits(pins);
    GPIOA_ModeCfg(pins, GPIO_ModeOut_OP_8mA);
    mDelaymS(10);
}


void _oled_trans(pOLED* p, uint8_t addr) {
    p->Addr = addr;
}

void _oled_writeCommand(pOLED* p, uint8_t cmd) {
    __oled_start();
    __oled_wr(p->Addr);
    __oled_ack();
    __oled_wr(0x00);
    __oled_ack();
    __oled_wr(cmd);
    __oled_ack();
    __oled_stop();
}

void _oled_writeData(pOLED* p, uint8_t data) {
    __oled_start();
    __oled_wr(p->Addr);
    __oled_ack();
    __oled_wr(0x40);
    __oled_ack();
    __oled_wr(data);
    __oled_ack();
    __oled_stop();
}

void _oled_setPosition(pOLED* p, uint8_t x, uint8_t y) {
    _oled_writeCommand(p, 0xb0 + y);
    _oled_writeCommand(p, (((x + 2) & 0xf0) >> 4) | 0x10);
    _oled_writeCommand(p, (x + 2) & 0x0f);
}

void _oled_color(pOLED* p, OLEDColor color) { p->Color = color; }

void _oled_font(pOLED* p, OLEDFontSize size) { p->FontSize = size; }

void _oled_clear(pOLED* p) {
    uint8_t max = (p->Addr == OLED_SCREEN_BIG) ? 8 : 4;
    for (uint8_t m = 0; m < max; m++) {
        _oled_setPosition(p, 0, m);
        for (uint8_t n = 0; n < 128; n++) {
            _oled_writeData(p, 0x00);
        }
    }
}

void _oled_init(pOLED* p) {
    iic_init();

    _oled_writeCommand(p, 0xAE);//--display off
    _oled_writeCommand(p, 0x00);//---set low column address
    _oled_writeCommand(p, 0x10);//---set high column address
    _oled_writeCommand(p, 0x40);//--set start line address
    _oled_writeCommand(p, 0xB0);//--set page address
    _oled_writeCommand(p, 0x81); // contract control
    _oled_writeCommand(p, 0xFF);//--128
    _oled_writeCommand(p, 0xA1);//set segment remap
    _oled_writeCommand(p, 0xA6);//--normal / reverse

    _oled_writeCommand(p, 0xA8);//--set multiplex ratio(1 to 64)
    if (p->Addr == OLED_SCREEN_BIG)
        _oled_writeCommand(p, 0x3F);//--1/32 duty
    else _oled_writeCommand(p, 0x1F);

    _oled_writeCommand(p, 0xC8);//Com scan direction
    _oled_writeCommand(p, 0xD3);//-set display offset
    _oled_writeCommand(p, 0x00);//

    _oled_writeCommand(p, 0xD5);//set osc division
    _oled_writeCommand(p, 0x80);//

    _oled_writeCommand(p, 0xD8);//set area color mode off
    _oled_writeCommand(p, 0x05);//

    _oled_writeCommand(p, 0xD9);//Set Pre-Charge Period
    _oled_writeCommand(p, 0xF1);//

    _oled_writeCommand(p, 0xDA);//set com pin configuartion
    if (p->Addr == OLED_SCREEN_BIG)
        _oled_writeCommand(p, 0x12);
    else _oled_writeCommand(p, 0x02);

    _oled_writeCommand(p, 0xDB);//set Vcomh
    _oled_writeCommand(p, 0x30);//

    _oled_writeCommand(p, 0x8D);//set charge pump enable
    _oled_writeCommand(p, 0x14);//

    _oled_writeCommand(p, 0xAF);//--turn on oled panel

    _oled_writeCommand(p, 0x21); //Set column address range
    _oled_writeCommand(p, 0x00);
    _oled_writeCommand(p, 0x7F); //127

    _oled_writeCommand(p, 0x22); //Set page address range
    _oled_writeCommand(p, 0x00);
    if (p->Addr == OLED_SCREEN_BIG)
        _oled_writeCommand(p, 0x07); //7
    else _oled_writeCommand(p, 0x03); //3

    _oled_clear(p);
}

void _oled_bright(pOLED* p, uint8_t brightness) {
    _oled_writeCommand(p, 0x81);
    _oled_writeCommand(p, brightness & 0x7F);
}

void _oled_open(pOLED* p) {
    _oled_writeCommand(p, 0X8D);
    _oled_writeCommand(p, 0X14);
    _oled_writeCommand(p, 0XAF);
}

void _oled_close(pOLED* p) {
    _oled_writeCommand(p, 0X8D);
    _oled_writeCommand(p, 0X10);
    _oled_writeCommand(p, 0XAE);
}

void _oled_flash(pOLED* p, const unsigned char* data) {
    uint32_t i = 0;
    uint8_t max = (p->Addr == OLED_SCREEN_BIG) ? 8 : 4;
    for (uint8_t m = 0; m < max; m++) {
        _oled_setPosition(p, 0, m);
        for (uint8_t n = 0; n < 128; n++) {
            _oled_writeData(p, data[i]);
            i++;
        }
    }
}

void _oled_draw(pOLED* p, uint8_t x, uint8_t y, char character) {
    char c = character - ' ';
    if (p->FontSize == FontBig)
    {
        if (x >= 128) { x = 0; y = y + 2; }
        _oled_setPosition(p, x, y);
        for (uint8_t i = 0; i < 8; i++)
            _oled_writeData(p, (p->Color == White) ? getFont(1)[c * 16 + i] : ~getFont(1)[c * 16 + i]);
        _oled_setPosition(p, x, y + 1);
        for (uint8_t i = 0; i < 8; i++)
            _oled_writeData(p, (p->Color == White) ? getFont(1)[c * 16 + i + 8] : ~getFont(1)[c * 16 + i + 8]);
    } else {
        if (x >= 128) { x = 0; y = y + 1; }
        _oled_setPosition(p, x, y);
        for (uint8_t i = 0; i < 6; i++)
            _oled_writeData(p, (p->Color == White) ? getFont(0)[c * 6 + i] : ~getFont(0)[c * 6 + i]);
    }
}

void _oled_print(pOLED* p, uint8_t x, uint8_t y, char* string) {
    int i = 0;
    if (p->FontSize == FontBig) {
        while (string[i] != '\0') {
            if (string[i] == '\n') {
                i++; x = 0; y += 2;
                continue;
            }
            _oled_draw(p, x, y, string[i]);
            x += 8;
            if (x > 120) { x = 0; y += 2; }
            i++;
        }
    }
    else {
        while (string[i] != '\0') {
            if (string[i] == '\n') {
                i++; x = 0; y++;
                continue;
            }
            _oled_draw(p, x, y, string[i]);
            x += 6;
            if (x > 120) { x = 0; y += 1; }
            i++;
        }
    }
}

int _oled_printf(pOLED* p, uint8_t x, uint8_t y, const char* format, ...) {
    char* iobuf = malloc(sizeof(char) * IOBUF_SIZE);
    va_list args;
    va_start(args, format);
    int result = vsprintf(iobuf, format, args);
    va_end(args);
    _oled_print(p, x, y, iobuf);
    free(iobuf);
    return result;
}

int _oled_printfc(pOLED* p, uint8_t y, const char* format, ...) {
    char* iobuf = malloc(sizeof(char) * IOBUF_SIZE);
    va_list args;
    va_start(args, format);
    int result = vsprintf(iobuf, format, args);
    va_end(args);
    uint8_t x = (128 - strlen(iobuf) * ((p->FontSize == FontBig) ? 8 : 6)) / 2;
    _oled_print(p, x, y, iobuf);
    free(iobuf);
    return result;
}

OLED* OLEDInit(uint8_t address) {
    pOLED* p = malloc(sizeof(pOLED));
    p->Addr = address;
    p->Color = White;
    p->FontSize = FontSmall;

    OLED* c = malloc(sizeof(OLED));
    c->p = p;
    c->trans = &_oled_trans;
    #ifdef OLED_USE_PRIVATE_FUN
    c->writeCommand = &_oled_writeCommand;
    c->writeData = &_oled_writeData;
    c->setPosition = &_oled_setPosition;
    #endif
    c->color = &_oled_color;
    c->font = &_oled_font;
    c->clear = &_oled_clear;
    c->init = &_oled_init;
    c->bright = &_oled_bright;
    c->open = &_oled_open;
    c->close = &_oled_close;
    c->flash = &_oled_flash;
    c->draw = &_oled_draw;
    c->print = &_oled_print;
    c->printf = &_oled_printf;
    c->printfc = &_oled_printfc;

    return c;
}
