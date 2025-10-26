################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
#
################################################################################

CFLAGS  += -DTM_ARCH_AVR8

SRCS += src/hal/hal_timers.c
SRCS += src/arch/avr8/timer1.c
SRCS += src/arch/avr8/timer3.c

SRCS += src/hal/hal_comm.c
SRCS += src/arch/avr8/usart1.c
SRCS += src/arch/avr8/i2c.c

SRCS += src/hal/hal_display.c
#SRCS += src/arch/avr8/led.c
SRCS += src/arch/avr8/lcdAMC2004.c
