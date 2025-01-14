#include "sd.h"

#include <stdlib.h>

/*
 * DI DO SCK SPI0
 * NCS PA12
 * CDT PB8
 * */

void _sd_select(pSD* p) {
	GPIOA_ResetBits(GPIO_Pin_12);
}

void _sd_deselect(pSD* p) {
    GPIOA_SetBits(GPIO_Pin_12);
}

uint8_t _sd_avail(pSD* p) {
    return (GPIOB_ReadPortPin(GPIO_Pin_8) == 0) ? 1 : 0;
}

uint8_t _sd_transfer(pSD* p, uint8_t x) {
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R8_SPI0_BUFFER = x;
    while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE));
    return R8_SPI0_BUFFER;
}

void _sd_blk_transfer(uint8_t* dat, uint16_t len) {
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R16_SPI0_TOTAL_CNT = len;
    R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END;
    while (len != 0) {
       if (R8_SPI0_FIFO_COUNT < SPI_FIFO_SIZE) {
           R8_SPI0_FIFO = *dat;
           dat++;
           len--;
       }
    }
    while (R8_SPI0_FIFO_COUNT != 0);
}

uint8_t _sd_wait_ack_(pSD* p) {
    uint8_t response = 0; uint16_t retry = 0;
    do {
        response = _sd_transfer(p, 0xFF);
        retry += 1;
        mDelayuS(1);
        if (retry > 0x01FF) return 0xFF;
    }  while ((response & 0x80) != 0);
    return response;
}

uint8_t _sd_wait_ready_(pSD* p) {
    uint8_t response = 0; uint16_t retry = 0;
    do {
        response = _sd_transfer(p, 0xFF);
        retry += 1;
        mDelayuS(1);
        if (retry > 0xFFFE) return 1;
    }  while (response != 0xFF);
    return 0;
}

uint8_t _sd_cmd(pSD* p, uint8_t cmd, uint32_t arg) {
    uint8_t response = 0;

    _sd_transfer(p, 0xFF);
    _sd_select(p);

    _sd_transfer(p, cmd | 0x40);
    _sd_transfer(p, arg >> 24);
    _sd_transfer(p, arg >> 16);
    _sd_transfer(p, arg >> 8);
    _sd_transfer(p, arg);
    uint8_t crc = 0xFF;
    if (cmd == CMD_RESET) crc = 0x95;
    _sd_transfer(p, crc);

    response = _sd_wait_ack_(p);

    _sd_deselect(p);
    _sd_transfer(p, 0xFF);
    return response;
}

uint8_t _sd_cmd_ncs_(pSD* p, uint8_t cmd, uint32_t arg, uint8_t crc) {
    uint8_t response = 0;

    _sd_transfer(p, cmd | 0x40);
    _sd_transfer(p, arg >> 24);
    _sd_transfer(p, arg >> 16);
    _sd_transfer(p, arg >> 8);
    _sd_transfer(p, arg);
    _sd_transfer(p, crc);

    response = _sd_wait_ack_(p);
    return response;
}

void spiInit(uint8_t v) {
    if (v != 0)
        R8_SPI0_CLOCK_DIV = FREQ_SYS / NSCAM_FSDH;
    else
        R8_SPI0_CLOCK_DIV = FREQ_SYS / NSCAM_FSDL;
    mDelaymS(10);
}

uint8_t _sd_init(pSD* p) {
    // SPI0
    GPIOA_SetBits(GPIO_Pin_12);
    GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, GPIO_Highspeed_PP_8mA);
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_Floating);
    SPI0_MasterDefInit();
    SPI0_DataMode(Mode0_HighBitINFront);

    // CDT
    GPIOB_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_Floating);

    spiInit(0);

    _sd_deselect(p);
    for (uint8_t i = 0; i < 10; i++) {
        _sd_transfer(p, 0xFF);
    }

    uint8_t response = 0;
    uint8_t buf[6];

    for (uint16_t i = 0; ; i++) {
        response = _sd_cmd(p, CMD_RESET, 0);
        if (response == STATE_IDLE) break;
        if (i == 0x01FF) {
            return 7;
        }
    }

    _sd_select(p);
    response = _sd_cmd_ncs_(p, CMD_VER, 0x1AA, 0x87);

    if (response == VER_1) {
        p->type = TYPE_V1;

        _sd_deselect(p);
        _sd_transfer(p, 0xFF);

        for (uint16_t i = 0; ; i++) {
            mDelaymS(5);
            if (i < 0x01FF) {
                _sd_select(p);
                response = _sd_cmd_ncs_(p, CMD_ACMD, 0, 0);
                _sd_deselect(p);
                if (response == STATE_IDLE) {
                    _sd_select(p);
                    response = _sd_cmd_ncs_(p, CMD_AINIT, 0, 0);
                    _sd_deselect(p);
                    if (response == STATE_READY) break;
                } else {
                    return 6;
                }
            } else if (i < 0x03FF) {
                _sd_select(p);
                response = _sd_cmd_ncs_(p, CMD_INIT, 0, 0);
                _sd_deselect(p);
                if (response == STATE_READY) {
                    p->type = TYPE_MMC;
                    break;
                }
            } else {
                return 5;
            }
        }
        _sd_deselect(p);

        spiInit(1);
        _sd_transfer(p, 0xFF);

        response = _sd_cmd(p, CMD_SECTOR, 512);

        if (response != STATE_READY) {
            return 4;
        }
    } else if (response == VER_2) {
        buf[0] = _sd_transfer(p, 0xFF);
        buf[1] = _sd_transfer(p, 0xFF);
        buf[2] = _sd_transfer(p, 0xFF);
        buf[3] = _sd_transfer(p, 0xFF);

        _sd_deselect(p);
        _sd_transfer(p, 0xFF);

        if (buf[2] == 0x01 && buf[3] == 0xAA) {
            uint8_t retry = 0;
            for (uint16_t i = 0; ; i++) {
                mDelaymS(5);
                if (i < 0x01FF) {
                    _sd_select(p);
                    response = _sd_cmd_ncs_(p, CMD_ACMD, 0, 0);
                    _sd_deselect(p);
                    if (response == STATE_IDLE) {
                        _sd_select(p);
                        response = _sd_cmd_ncs_(p, CMD_AINIT, 0x40000000, 0);
                        _sd_deselect(p);
                        if (response == STATE_READY) break;
                    } else {
                        if (retry < 10) {
                            retry += 1;
                            continue;
                        }
                        return 3;
                    }
                } else {
                    return 2;
                }
            }

            _sd_select(p);
            response = _sd_cmd_ncs_(p, CMD_OCR, 0, 0);
            if (response != STATE_READY) {
                _sd_deselect(p);
                return 1;
            }
            buf[0] = _sd_transfer(p, 0xFF);
            buf[1] = _sd_transfer(p, 0xFF);
            buf[2] = _sd_transfer(p, 0xFF);
            buf[3] = _sd_transfer(p, 0xFF);

            _sd_deselect(p);
            _sd_transfer(p, 0xFF);

            p->type = (buf[0] & 0x40) ? TYPE_V2_SDHC : TYPE_V2;

            spiInit(1);
        }
    }

    _sd_deselect(p);

    return 0;
}

uint8_t _sd_receive_ncs_(pSD* p, uint8_t* data, uint16_t length) {
    uint16_t retry = 0;

    while (_sd_transfer(p, 0xFF) != 0xFE) {
        mDelayuS(1);
        if (++retry > 0xFFFE) {
            _sd_deselect(p);
            _sd_transfer(p, 0xFF);
            return 1;
        }
    }
    for (uint16_t i = 0; i < length; i++) {
        *data++ = _sd_transfer(p, 0xFF);
    }

    _sd_transfer(p, 0xFF); //CRC1
    _sd_transfer(p, 0xFF); //CRC2

    return 0;
}

uint8_t _sd_cid(pSD* p, uint8_t* cid) {
    uint8_t response = 0;

    response = _sd_cmd(p, CMD_CID, 0);
    if (response != STATE_READY) {
        _sd_deselect(p);
        _sd_transfer(p, 0xFF);
        return response;
    }

    _sd_transfer(p, 0xFF);
    _sd_select(p);
    _sd_receive_ncs_(p, cid, 16);
    _sd_deselect(p);
    _sd_transfer(p, 0xFF);

    return 0;
}

uint8_t _sd_csd(pSD* p, uint8_t* csd) {
    uint8_t response = 0;

    response = _sd_cmd(p, CMD_CSD, 0);
    if (response != STATE_READY) {
        _sd_deselect(p);
        _sd_transfer(p, 0xFF);
        return response;
    }

    _sd_transfer(p, 0xFF);
    _sd_select(p);
    _sd_receive_ncs_(p, csd, 16);
    _sd_deselect(p);
    _sd_transfer(p, 0xFF);

    return 0;
}

uint32_t _sd_size(pSD* p) {
    uint8_t csd[16]; uint32_t size;
    uint8_t response; uint16_t i, tmp;

    if (_sd_csd(p, csd) != 0) {
        return 0;
    }

    if ((csd[0] & 0xC0) == 0x40) {
        size = ((((uint32_t)csd[8]) << 8) + (uint32_t)csd[9] + 1) * 0x00000400;
    } else {
        //C_SIZE
        i = csd[6] & 0x03;
        i <<= 8;
        i += csd[7];
        i <<= 2;
        i += ((csd[8] & 0xc0) >> 6);

        //C_SIZE_MULT
        response = csd[9] & 0x03;
        response <<= 1;
        response += ((csd[10] & 0x80) >> 7);

        //BLOCKNR
        response+=2;
        tmp = 1;
        while(response) {
            tmp*=2;
            response--;
        }
        size = ((uint32_t)(i + 1)) * ((uint32_t)tmp);

        // READ_BL_LEN
        i = csd[5] & 0x0f;

        //BLOCK_LEN
        tmp = 1;
        while(i) {
            tmp *= 2;
            i--;
        }

        //The final result
        size *= (uint32_t)tmp;
    }

    return size;
}

uint8_t _sd_read(pSD* p, uint32_t sector, uint8_t* buf) {
    uint8_t response = 0;

    if (p->type != TYPE_V2_SDHC) sector = sector << 9;

    _sd_transfer(p, 0xFF);
    _sd_select(p);
    response = _sd_cmd_ncs_(p, CMD_READ, sector, 0);
    _sd_deselect(p);
    if (response != STATE_READY) {
        _sd_transfer(p, 0xFF);
        return 2;
    }

    _sd_select(p);
    response = _sd_receive_ncs_(p, buf, 512);
    _sd_deselect(p);
    if (response != STATE_READY) {
        _sd_transfer(p, 0xFF);
        return 1;
    }

    _sd_transfer(p, 0xFF);
    return 0;
}

uint8_t _sd_write(pSD* p, uint32_t sector, uint8_t* buf) {
    uint8_t response = 0; uint16_t retry = 0;

    if (p->type != TYPE_V2_SDHC) sector = sector << 9;

    _sd_transfer(p, 0xFF);
    _sd_select(p);
    response = _sd_cmd_ncs_(p, CMD_WRITE, sector, 0);
    _sd_deselect(p);
    if (response != STATE_READY) {
        _sd_transfer(p, 0xFF);
        return 3;
    }

    _sd_select(p);
    _sd_transfer(p, 0xFF);
    _sd_transfer(p, 0xFF);
    _sd_transfer(p, 0xFF);
    _sd_transfer(p, 0xFE);
    _sd_blk_transfer(buf, 512);
    _sd_transfer(p, 0xFF); //CRC1
    _sd_transfer(p, 0xFF); //CRC2

    retry = 0;
    while ((_sd_transfer(p, 0xFF) & 0x1F) != 0x05) {
        mDelayuS(1);
        if (++retry > 0xFFFE) {
            _sd_deselect(p);
            _sd_transfer(p, 0xFF);
            return 2;
        }
    }

    retry = 0;
    while (!_sd_transfer(p, 0xFF)) {
        mDelayuS(1);
        if (++retry > 0xFFFE) {
            _sd_deselect(p);
            _sd_transfer(p, 0xFF);
            return 1;
        }
    }
    _sd_deselect(p);
    _sd_transfer(p, 0xFF);
    return 0;
}

uint8_t _sd_reads(pSD* p, uint32_t sector, uint8_t* buf, uint16_t count) {
    uint8_t response = 0;

    if (p->type != TYPE_V2_SDHC) sector = sector << 9;

    _sd_transfer(p, 0xFF);
    _sd_select(p);
    response = _sd_cmd_ncs_(p, CMD_READS, sector, 0);
    _sd_deselect(p);
    if (response != STATE_READY) {
        _sd_transfer(p, 0xFF);
        return 1;
    }

    for (uint16_t i = 0; i < count; i++) {
        _sd_select(p);
        response = _sd_receive_ncs_(p, buf, 512);
        _sd_deselect(p);
        if (response != STATE_READY) break;
        buf += 512;
    }

    _sd_select(p);
    _sd_cmd_ncs_(p, CMD_RSE, 0, 0);
    _sd_deselect(p);
    _sd_transfer(p, 0xFF);
    return 0;
}

uint8_t _sd_writes(pSD* p, uint32_t sector, uint8_t* buf, uint16_t count) {
    uint8_t response = 0;

    _sd_transfer(p, 0xFF);
    if (p->type != TYPE_V2_SDHC) sector = sector << 9;

    if (p->type != TYPE_MMC) {
        _sd_select(p);
        _sd_cmd_ncs_(p, CMD_APE, count, 0);
        _sd_deselect(p);
    }

    _sd_select(p);
    response = _sd_cmd_ncs_(p, CMD_WRITES, sector, 0);
    _sd_deselect(p);
    if (response != STATE_READY) {
        _sd_transfer(p, 0xFF);
        return 4;
    }

    _sd_select(p);
    _sd_transfer(p, 0xFF);
    _sd_transfer(p, 0xFF);
    for (uint16_t i = 0; i < count; i++) {
        _sd_transfer(p, 0xFC);

        _sd_blk_transfer(buf, 512);
        buf += 512;
        _sd_transfer(p, 0xFF); //CRC1
        _sd_transfer(p, 0xFF); //CRC2

        uint16_t retry = 0;
        while ((_sd_transfer(p, 0xFF) & 0x1F) != 0x05) {
            mDelayuS(1);
            if (++retry > 0xFFFE) {
                _sd_deselect(p);
                _sd_transfer(p, 0xFF);
                return 3;
            }
        }

        if (_sd_wait_ready_(p) == 1) {
            _sd_deselect(p);
            _sd_transfer(p, 0xFF);
            return 2;
        }
    }

    response = _sd_transfer(p, 0xFD);
    if (response == STATE_READY) count = 0xFE;

    if (_sd_wait_ready_(p) == 1) {
        _sd_deselect(p);
        _sd_transfer(p, 0xFF);
        return 1;
    }

    _sd_deselect(p);
    _sd_transfer(p, 0xFF);
    return 0;
}

SD* SDInit() {
    pSD* p = malloc(sizeof(pSD));

    SD* c = malloc(sizeof(SD));
    c->p = p;

    c->init = &_sd_init;
    c->avail = &_sd_avail;
#ifdef SD_USE_PRIVATE
    c->select = &_sd_select;
    c->deselect = &_sd_deselect;
#endif
    c->cid = &_sd_cid;
    c->csd = &_sd_csd;
    c->cmd = &_sd_cmd;
    c->size = &_sd_size;
    c->read = &_sd_read;
    c->write = &_sd_write;
    c->reads = &_sd_reads;
	c->writes = &_sd_writes;

    return c;
}
