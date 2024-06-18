#include "sccb.h"

#define SCCB_DELAY() mDelayuS(5)

void __sccb_start() {
    SCCB_DAT_SET();
    SCCB_CLK_SET(); SCCB_DELAY();
    SCCB_DAT_CLR(); SCCB_DELAY();
}

void __sccb_stop() {
    SCCB_DAT_CLR();
    SCCB_CLK_SET(); SCCB_DELAY();
    SCCB_DAT_SET(); SCCB_DELAY();
}

void __sccb_ack() {
    SCCB_CLK_CLR();
    SCCB_DAT_SET();
    SCCB_CLK_SET(); SCCB_DELAY();
    for (uint8_t i = 0; i < 0xFF; i++) {
        if (SCCB_DAT_GET() == 0)
            break;
        SCCB_DELAY();
    }
    SCCB_CLK_CLR(); SCCB_DELAY();
}

void __sccb_nak() {
    SCCB_CLK_CLR();
    SCCB_DAT_SET();
    SCCB_CLK_SET(); SCCB_DELAY();
    SCCB_CLK_CLR(); SCCB_DELAY();
}

void __sccb_wr(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        SCCB_CLK_CLR(); SCCB_DELAY();
        if (data & (1 << (7 - i)))
            SCCB_DAT_SET();
        else
            SCCB_DAT_CLR();
        SCCB_DELAY();
        SCCB_CLK_SET(); SCCB_DELAY();
    }
    SCCB_CLK_CLR();
    SCCB_DAT_SET();
    SCCB_DELAY();
}

uint8_t __sccb_rd() {
    uint8_t data = 0;
    SCCB_DAT_SET();
    SCCB_CLK_CLR(); SCCB_DELAY();
    for (uint8_t i = 0; i < 8; i++) {
        SCCB_CLK_SET(); SCCB_DELAY();
        uint8_t tmp = 0;
        if (SCCB_DAT_GET() != 0)
            tmp = 1 << (7 - i);
        data |= tmp;
        SCCB_DELAY();
        SCCB_CLK_CLR(); SCCB_DELAY();
    }
    return data;
}

void sccb_init() {
    /*
     * CLK PA8
     * DAT PA7
     * */
    uint32_t pins = GPIO_Pin_7 | GPIO_Pin_8;
    GPIOA_SetBits(pins);
    GPIOA_ModeCfg(pins, GPIO_ModeOut_OP_8mA);
}

void sccb_write(uint8_t addr, uint16_t data) {
    __sccb_start();
    __sccb_wr(SCCB_ADDR & 0xFE);
    __sccb_ack();
    __sccb_wr(addr);
    __sccb_ack();
    __sccb_wr(data >> 8);
    __sccb_ack();
    __sccb_wr(data & 0xFF);
    __sccb_ack();
    __sccb_stop();
}

uint16_t sccb_read(uint8_t addr) {
    __sccb_start();
    __sccb_wr(SCCB_ADDR & 0xFE);
    __sccb_ack();
    __sccb_wr(addr);
    __sccb_ack();
    __sccb_stop();

    uint16_t data = 0;
    __sccb_start();
    __sccb_wr(SCCB_ADDR | 0x01);
    __sccb_ack();
    data = __sccb_rd();
    __sccb_ack();
    data <<= 8;
    data |= __sccb_rd();
    __sccb_nak();
    __sccb_stop();
    return data;
}

void iic_write(uint8_t dev, uint8_t addr, uint8_t data) {
    __sccb_start();
    __sccb_wr(dev & 0xFE);
    __sccb_ack();
    __sccb_wr(addr);
    __sccb_ack();
    __sccb_wr(data);
    __sccb_ack();
    __sccb_stop();
}

uint8_t iic_read(uint8_t dev, uint8_t addr) {
    __sccb_start();
    __sccb_wr(dev & 0xFE);
    __sccb_ack();
    __sccb_wr(addr);
    __sccb_ack();

    uint8_t data = 0;
    __sccb_start();
    __sccb_wr(SCCB_ADDR | 0x01);
    __sccb_ack();
    data = __sccb_rd();
    __sccb_nak();
    __sccb_stop();
    return data;
}
