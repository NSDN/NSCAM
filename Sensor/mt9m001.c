#include "mt9m001.h"

#include "CH56x_common.h"

#define HBLK 2047

static float FMOS = 1;
#define TROW ((MT9M001_COLS + HBLK) / (FMOS / 1000000) / 1000000)

#include "color_map.h"
#define SEN_LCD_MAP_SIZ 1024
volatile uint16_t sensor_lcd_map[SEN_LCD_MAP_SIZ] __attribute__ ((section(".DMADATA")));
static void gen_sensor_lcd_map() {
    for (uint16_t i = 0; i < SEN_LCD_MAP_SIZ; i++) {
        sensor_lcd_map[i] = __color_map[i];
    }
}

void DVP_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));
volatile uint16_t sensor_buf_0[MT9M001_COLS / 2] __attribute__ ((section(".DMADATA")));
volatile uint16_t sensor_buf_1[MT9M001_COLS / 2] __attribute__ ((section(".DMADATA")));
volatile uint16_t lcd_row_buf[320] = { 0 };
volatile uint16_t row_count = 0;
volatile uint16_t curr_ypos = 0;

void sensor_clock(uint32_t clk) {
    FMOS = clk;
    TMR1_PWMCycleCfg(FREQ_SYS / clk);
    TMR1_PWMActDataWidth(FREQ_SYS / clk / 2);
}

void sensor_init() {
    /*
     * TMR1 PB15
     * */
    GPIOB_ResetBits(GPIO_Pin_15);
    GPIOB_ModeCfg(GPIO_Pin_15, GPIO_Highspeed_PP_8mA);
    TMR1_PWMInit(high_on_low, PWM_Times_1);
    sensor_clock(NSCAM_FCMOSH);
    mDelaymS(100);

    gen_sensor_lcd_map();

    R8_DVP_CR0 |= (RB_DVP_V_POLAR | RB_DVP_D10_MOD);
    R8_DVP_CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);
    R16_DVP_COL_NUM = MT9M001_COLS / 2;
    R16_DVP_ROW_NUM = MT9M001_ROWS / 2;
    R32_DVP_DMA_BUF0 = (uint32_t) &sensor_buf_0;
    R32_DVP_DMA_BUF1 = (uint32_t) &sensor_buf_1;
    R8_DVP_INT_EN |= (RB_DVP_IE_ROW_DONE | RB_DVP_IE_FRM_DONE | RB_DVP_IE_STR_FRM | RB_DVP_IE_STP_FRM | RB_DVP_IE_FIFO_OV);

    sccb_init();

    sccb_write(0x0D, 0x0001);               //Reset
    sccb_write(0x0D, 0x0000);
    mDelaymS(100);

    sccb_write(0x01, 12);                   //Row Start,        Default 0x000C, 12+(1024-720)/2=164
    sccb_write(0x02, 20);                   //Column Start,     Default 0x0014
    sccb_write(0x03, MT9M001_ROWS - 1);     //Row Width,        Default 0x03FF, 0x02CF = 720  - 1
    sccb_write(0x04, MT9M001_COLS - 1);     //Column Width,     Default 0x04FF, 0x3FFF = 1280 - 1
    sccb_write(0x05, HBLK);                 //Hor Blanking,     Default 0x0009, 0x0009 = 9
    sccb_write(0x06, 0x0019);               //Ver Blanking,     Default 0x0019, 26 rows

    sccb_write(0x09, 0x0419);               //Shutter Width L,  Default 0x0419 = 1049
    sccb_write(0x0C, 0);                    //Shutter Delay,    Default 0x0000
    /*
     * 0x1E=0x800C, 0x20=0x9104, FCMOS=12MHz, 实时显示是320x256，实际分辨率为160x128
     * 0x1E=0x8000, 0x20=0x911C, FCMOS=24MHz, 实时显示是640x512降采样到320x256
     * 二者速度好像没差别
     * */
    sccb_write(0x1E, 0x8000);               //Read Mode 1
    sccb_write(0x20, 0x911C);               //Read Mode 2
    sccb_write(0x35, 0x0008);               //Global Gain,      Default 0x0008; 0x08-0x20:1-4;0x51-0x60:4.25-8;0x61-0x67:9-15
    mDelaymS(100);

    PFIC_EnableIRQ(DVP_IRQn);
    R8_DVP_CR1 |= RB_DVP_DMA_EN;
    R8_DVP_CR0 |= RB_DVP_ENABLE;
}

void sensor_shutter(float width) {
    if (width < 0) width = -width;
    if (width == 0) width = 1;
    int value = (int) (width / TROW);
    if (value > 0xFFFF) value = 0xFFFF;
    sccb_write(0x09, ((uint32_t) value) & 0xFFFF);
}

void sensor_gain(float g) {
    uint16_t dat = 0x0008;
    if (g < 0) g = 1;
    if (g <= 4)
        dat = (uint16_t) (g / 0.125f);
    else if (g <= 8) {
        dat = (uint16_t) (g / 2 / 0.125f);
        dat |= 0x40;
    } else {
        float gd = g / 8;
        dat = ((uint16_t)((gd - 1) * 8) & 0x7F) << 8;
        dat |= 0x7F;
    }
    sccb_write(0x35, dat);
}

void sensor_live() {
    sensor_clock(NSCAM_FCMOSH);
    sccb_write(0x1E, 0x8000);
}

void sensor_shot() {
    sensor_clock(NSCAM_FCMOSL);
    sccb_write(0x1E, 0x8100);
}

uint16_t* sensor_get_buf(uint16_t* ypos) {
    if (curr_ypos != 0xAAAA) {
        *ypos = curr_ypos;
        return (uint16_t*) &lcd_row_buf[0];
    }
    return NULL;
}

void sensor_req_buf() {
    curr_ypos = 0xAAAA;
}

void DVP_IRQHandler() {
    if (R8_DVP_INT_FLAG & RB_DVP_IF_ROW_DONE) {
        R8_DVP_INT_FLAG = RB_DVP_IF_ROW_DONE;

        uint16_t* ptr = (uint16_t*) (((row_count & 0x0001) != 0) ? sensor_buf_1 : sensor_buf_0);

        if (curr_ypos == 0xAAAA && (row_count & 0x0001) != 0) {
            uint16_t k;
            for (uint16_t i = 0; i < 320; i++) {
                k = 640 - 1 - (i << 1);
                k = ptr[k] & 0x3FF;
                lcd_row_buf[i] = sensor_lcd_map[k];
            }

            curr_ypos = row_count >> 1;
        }

        row_count += 1;
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_FRM_DONE) {
        R8_DVP_INT_FLAG = RB_DVP_IF_FRM_DONE;

    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_STR_FRM) {
        R8_DVP_INT_FLAG = RB_DVP_IF_STR_FRM;

        row_count = 0;
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_STP_FRM) {
        R8_DVP_INT_FLAG = RB_DVP_IF_STP_FRM;

    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_FIFO_OV) {
        R8_DVP_INT_FLAG = RB_DVP_IF_FIFO_OV;

    }
}
