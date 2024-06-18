#ifndef __MT9M001_H_
#define __MT9M001_H_


#include "sccb.h"

#define MT9M001_COLS 1280
#define MT9M001_ROWS 1024

void sensor_init();
void sensor_shutter(float width);
void sensor_gain(float g);
uint16_t* sensor_get_buf(uint16_t* ypos);
void sensor_req_buf();

#endif
