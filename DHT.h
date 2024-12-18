/*
 * DHT.h
 *
 *  Created on: Jun 28, 2020
 *      Author: Controllerstech.com
 */

#ifndef DHT_H_
#define DHT_H_

#include "sl_udelay.h"
#include "sl_sleeptimer.h"

#include "em_cmu.h"
#include "em_gpio.h"

typedef struct
{
	uint32_t Temperature;
	uint32_t Humidity;
}DHT_DataTypedef;


void DHT_GetData (DHT_DataTypedef *DHT_Data);

#endif /* INC_DHT_H_ */
