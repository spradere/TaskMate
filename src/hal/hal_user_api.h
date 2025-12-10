/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_api.h
 * @brief hal header api
 *
 */

#ifndef HAL_API_H
#define HAL_API_H

#include <stdint.h>
#include <stdbool.h>

#include "hal/autoInclude_hal_target.h"
#include "sysCall/error.h"
#include "sysCall/gpio.h"

// mcu
void hal_i2cInit(void);
void hal_i2cStart(void);
void hal_i2cStop(void);
uint8_t hal_i2cCommStart(uint8_t address);
void hal_i2cCommStop(void);
uint8_t hal_i2cWrite(uint8_t data);
uint8_t hal_i2cRead(bool ack);

void hal_lcdInit(void);
void hal_lcdStart(void);
void hal_lcdStop(void);
void hal_lcdClear(void);
void hal_lcdSetCursor(uint8_t row, uint8_t col);
void hal_lcdWriteString(const char *str);

void hal_usartInit(void);
void hal_usartStart(void);
void hal_usartStop(void);
errorCode_t hal_usartRead(uint8_t *data);
errorCode_t hal_usartWriteChar(uint8_t data);
void hal_usartSendTXBuffer(void);
errorCode_t hal_usartTestBufferRx(void);
errorCode_t hal_usartWriteString(const char *str);
errorCode_t hal_usartWriteChar(uint8_t data);

// board

void hal_gpioInitPin(const gpio_pin_item_t *pin);
void hal_gpioWritePin(const gpio_pin_item_t *pin, bool value);
bool hal_gpioReadPin(const gpio_pin_item_t *pin);

typedef struct
{
	uint8_t seconds;  // 0-59
	uint8_t minutes;  // 0-59
	uint8_t hours;    // 0-23
	uint8_t weekday;  // 1-7
	uint8_t day;      // 1-31
	uint8_t month;    // 1-12
	uint8_t year;     // 0-99
} hal_rtc_time_t;

void hal_ZS_042Init();
void hal_ZS_042Start();
void hal_ZS_042Stop();
uint8_t hal_ZS_042Read(hal_rtc_time_t *t);
uint8_t hal_ZS_042Write(const hal_rtc_time_t *t);


#endif
