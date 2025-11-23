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
 * @file hal_mcu_define.h
 * @brief hal mcu header
 *
 * @todo nothing
 */

#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#define I2C_FREQ 100000UL // Standard mode 100 kHz
#define I2C_TWBR_VALUE ((F_CPU / I2C_FREQ - 16) / 2)

#define USART1_BAUD_RATE 9600

#endif
