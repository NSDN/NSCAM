/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 		 : 
*******************************************************************************/

#include <string.h>
#include "CH56x_common.h"

uint8_t DEBUG = 0;

#include "lcd.h"
LCD* lcd;
#include "oled.h"
OLED* oled;
#include "nyaops.h"

#include "sd.h"
SD* sd;
BOOL SD_OK = 0;

#include "ff.h"
FATFS fs;

#include "mt9m001.h"
uint16_t snap_count = 0;
uint8_t snapshot = 0;

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main()
{
    // Close LCD BLK
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_Slowascent_PP_8mA);
    GPIOA_ResetBits(GPIO_Pin_9);

	SystemInit(FREQ_SYS);
	Delay_Init(FREQ_SYS);
	mDelaymS(500);

	/*
	 * LED PB9
	 * RX0 PB5
	 * TX0 PB6
	 * */
	GPIOB_ResetBits(GPIO_Pin_9);
	GPIOB_ModeCfg(GPIO_Pin_9, GPIO_Slowascent_PP_8mA);
	GPIOB_ResetBits(GPIO_Pin_5);
	GPIOB_ModeCfg(GPIO_Pin_5, GPIO_Slowascent_PP_8mA);
	GPIOB_ResetBits(GPIO_Pin_6);
	GPIOB_ModeCfg(GPIO_Pin_6, GPIO_Slowascent_PP_8mA);

	/*
	 * CHG PA16
	 * STD PB7
	 * */
	GPIOA_ModeCfg(GPIO_Pin_16, GPIO_ModeIN_Floating);
	GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_Floating);

	/*
	 * KRUN PB0
	 * KFL PA23 (UP)
	 * KFR PA17 (DOWN)
	 * KUP PB1
	 * KLF PB2
	 * KDN PA20
	 * KRT PB3
	 * KPU PB4
	 * */
	GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeIN_Floating);
	GPIOA_ModeCfg(GPIO_Pin_23, GPIO_ModeIN_Floating);
	GPIOA_ModeCfg(GPIO_Pin_17, GPIO_ModeIN_Floating);
	GPIOB_ModeCfg(GPIO_Pin_1, GPIO_ModeIN_Floating);
	GPIOB_ModeCfg(GPIO_Pin_2, GPIO_ModeIN_Floating);
	GPIOA_ModeCfg(GPIO_Pin_20, GPIO_ModeIN_Floating);
	GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_Floating);
	GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_Floating);

	/*
     * TRIG PA19
     * */
	GPIOA_ResetBits(GPIO_Pin_19);
    GPIOA_ModeCfg(GPIO_Pin_19, GPIO_Highspeed_PP_8mA);

    /*
     * STROBE PA18
     * */
    GPIOA_ModeCfg(GPIO_Pin_18, GPIO_ModeIN_Floating);

	lcd = LCDInit();
    lcd->init(lcd->p);
    lcd->colorb(lcd->p, 0x000000); // 00BCD4
    lcd->colorf(lcd->p, 0xFFFFFF);
    lcd->clear(lcd->p);
    lcd->light(lcd->p, 1);

    oled = OLEDInit(OLED_SCREEN_SMALL);
    oled->init(oled->p);
    oled->clear(oled->p);
    oled->font(oled->p, FontBig);
    oled->printfc(oled->p, 1, "NSCAM-M2");

    int k;

    if (DEBUG) lcd->printfa(lcd->p, "\nNSCAM-M2, by NSDN\n\n");
    else lcd->bitmapsc(lcd->p, 160, 120, 152, 32, get_nyaops_logo());
    mDelaymS(1000);

    if (DEBUG) lcd->printfa(lcd->p, "sd init...\n");
    sd = SDInit();
    uint8_t sds = sd->init(sd->p);
    if (sd->avail(sd->p)) {
        uint32_t siz = sd->size(sd->p);
        if (DEBUG) lcd->printfa(lcd->p, "sd state: %02X, type:%d\n", sds, sd->p->type);
        if (DEBUG) lcd->printfa(lcd->p, "size: %d MB\n", siz / 2048);
        SD_OK = (sds == 0) && (siz != 0);
    } else {
        if (DEBUG) lcd->printfa(lcd->p, "where is my card?\n");
    }
    if (DEBUG) lcd->printfa(lcd->p, "\n");

    if (SD_OK) {
        if (DEBUG) lcd->printfa(lcd->p, "init fatfs...\n");
        k = f_mount(&fs, "", 1);
        if (k == FR_OK) {
            if (DEBUG) lcd->printfa(lcd->p, "fs mounted.\n");
            FIL info = { 0 };
            f_open(&info, "INFO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
            f_printf(&info, "NSCAM-M2 with CH565W\n");
            f_printf(&info, "by NyaSama Developer Network\n");
            f_printf(&info, "Firmware Version: %s\n", "0.01a");
            f_printf(&info, "\nFreq Info:\n");
            f_printf(&info, "System: %8dHz\n", NSCAM_FSYS);
            f_printf(&info, "Screen: %8dHz\n", NSCAM_FLCD);
            f_printf(&info, "SDCard: %8dHz\n", NSCAM_FSDH);
            f_printf(&info, "Sensor: %8dHz\n", NSCAM_FCMOSH);
            f_close(&info);

            if (DEBUG) {
                lcd->printfa(lcd->p, "testing");
                uint16_t buff[512] = { 0 }; UINT v;
                FIL test = { 0 };
                f_open(&test, "TEST.BIN", FA_WRITE | FA_CREATE_ALWAYS);
                for (int i = 0; i < 1024; i++) {
                    buff[0] = 0xAA55;
                    buff[511] = i;
                    k = f_write(&test, buff, 1024, &v);
                    f_sync(&test);
                    if (k != FR_OK) {
                        lcd->printfa(lcd->p, "\nerr: %d\n", k);
                        break;
                    }
                    if ((i % 64) == 0)
                        lcd->printfa(lcd->p, ".");
                }
                f_close(&test);
                lcd->printfa(lcd->p, "\n");
            }

            FIL count = { 0 };
            k = f_open(&count, "COUNT.BIN", FA_READ);
            if (k == FR_OK) {
                f_read(&count, (void*) &snap_count, 4, &k);
                if (DEBUG) lcd->printfa(lcd->p, "we have %d snapshots.\n", snap_count);
            } else {
                f_open(&count, "COUNT.BIN", FA_WRITE | FA_CREATE_ALWAYS);
                f_write(&count, (void*) &snap_count, 4, &k);
            }
            f_close(&count);
        } else {
            if (DEBUG) lcd->printfa(lcd->p, "fs fail, res: %02X\n", k);
        }
        if (DEBUG) lcd->printfa(lcd->p, "\n");
    }

    if (DEBUG) lcd->printfa(lcd->p, "init sensor...\n");
    float gain = 1, shutter = 0.025f;
    sensor_init();
    sensor_shutter(shutter);
    sensor_gain(gain);

    mDelaymS(1000);

    lcd->colorb(lcd->p, 0x000000);
    lcd->colorf(lcd->p, 0xFFFFFF);
    lcd->clear(lcd->p);
    oled->clear(oled->p);

    uint8_t chg = 0, std = 0;
    uint8_t ku = 0, kd = 0, kl = 0, kr = 0;

    GPIOB_ModeCfg(GPIO_Pin_9, GPIO_ModeIN_Floating);
    oled->printf(oled->p, 4, 0, "G %-2d", (int16_t) gain);
    oled->printf(oled->p, 4, 2, "1/%-4d", (int16_t) (1 / shutter));
    //oled->printf(oled->p, 100, 2, "%03d", snap_count);
    oled->printf(oled->p, 84, 2, "IR-TI");

    BOOL state = TRUE;
	while (1) {
	    if (state) {
	        uint16_t pos = 0;
	        uint16_t* ptr = sensor_get_buf(&pos);
            if (ptr != NULL) {
                if (pos >= 8 && pos < 248)
                    lcd->row(lcd->p, pos - 8, ptr);
                sensor_req_buf();
            }
	    }

        if (GPIOB_ReadPortPin(GPIO_Pin_0) == 0 && snapshot == 0) {
            snapshot = 1;
            //GPIOA_SetBits(GPIO_Pin_19);
            GPIOB_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_OP_8mA);

            if (state) {
                state = FALSE;
                lcd->light(lcd->p, 0);
                lcd->ctrl(lcd->p, 0);
            } else {
                state = TRUE;
                lcd->ctrl(lcd->p, 1);
                lcd->light(lcd->p, 1);
            }
        }

        if (GPIOB_ReadPortPin(GPIO_Pin_0) != 0 && snapshot == 1) {
            snapshot = 0;
            //GPIOA_ResetBits(GPIO_Pin_19);
            GPIOB_ModeCfg(GPIO_Pin_9, GPIO_ModeIN_Floating);
        }

	    if (GPIOA_ReadPortPin(GPIO_Pin_16) == 0 && chg == 0)
	        chg = 1;
        if (GPIOB_ReadPortPin(GPIO_Pin_7) == 0 && std == 0)
            std = 1;
        if (chg == 1) {
            chg = 2;
            oled->printf(oled->p, 100, 0, "CHG");
        }
        if (std == 1) {
            std = 2;
            oled->printf(oled->p, 100, 0, "STD");
        }
        if (GPIOA_ReadPortPin(GPIO_Pin_16) != 0 && chg == 2) {
            chg = 0;
            oled->printf(oled->p, 100, 0, "   ");
        }
        if (GPIOB_ReadPortPin(GPIO_Pin_7) != 0 && std == 2) {
            std = 0;
            oled->printf(oled->p, 100, 0, "   ");
        }

		if (GPIOB_ReadPortPin(GPIO_Pin_1) == 0 && ku == 0)
		    ku = 1;
		if (GPIOA_ReadPortPin(GPIO_Pin_20) == 0 && kd == 0)
            kd = 1;
		if (GPIOB_ReadPortPin(GPIO_Pin_2) == 0 && kl == 0)
            kl = 1;
        if (GPIOB_ReadPortPin(GPIO_Pin_3) == 0 && kr == 0)
            kr = 1;

		if (GPIOB_ReadPortPin(GPIO_Pin_1) != 0 && ku == 1) {
		    ku = 0;
		    gain += 1;
            if (gain > 15)
                gain = 15;
		    sensor_gain(gain);
		    oled->printf(oled->p, 4, 0, "G %-2d", (int16_t) gain);
		}
		if (GPIOA_ReadPortPin(GPIO_Pin_20) != 0 && kd == 1) {
            kd = 0;
            gain -=1;
            if (gain < 1)
                gain = 1;
            sensor_gain(gain);
            oled->printf(oled->p, 4, 0, "G %-2d", (int16_t) gain);
        }
		if (GPIOB_ReadPortPin(GPIO_Pin_2) != 0 && kl == 1) {
            kl = 0;
            shutter *= 2;
            if (shutter > 0.1f)
                shutter = 0.1f;
            sensor_shutter(shutter);
            oled->printf(oled->p, 4, 2, "1/%-4d", (int16_t) (1 / shutter));
        }
        if (GPIOB_ReadPortPin(GPIO_Pin_3) != 0 && kr == 1) {
            kr = 0;
            shutter /= 2;
            if (shutter < 0.1f / 512)
                shutter = 0.1f / 512;
            sensor_shutter(shutter);
            oled->printf(oled->p, 4, 2, "1/%-4d", (int16_t) (1 / shutter));
        }
	}
}
