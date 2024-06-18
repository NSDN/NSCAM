#ifndef __SCCB_H_
#define __SCCB_H_


#include "CH56x_common.h"

#define SCCB_ADDR 0xBA

/*
 * CLK PA8
 * DAT PA7
 * */
#define SCCB_DAT_SET() GPIOA_SetBits(GPIO_Pin_7)
#define SCCB_DAT_CLR() GPIOA_ResetBits(GPIO_Pin_7)
#define SCCB_CLK_SET() GPIOA_SetBits(GPIO_Pin_8)
#define SCCB_CLK_CLR() GPIOA_ResetBits(GPIO_Pin_8)

#define SCCB_DAT_GET() GPIOA_ReadPortPin(GPIO_Pin_7)

void sccb_init();
void sccb_write(uint8_t addr, uint16_t data);
uint16_t sccb_read(uint8_t addr);


#endif
