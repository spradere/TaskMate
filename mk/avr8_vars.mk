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

# MCU and Programmer settings
MCU = atmega2560
F_CPU = 16000000UL
PROGRAMMER = avrispmkII
PORT = /dev/ttyU0

# Compiler
CC = avr-gcc

CFLAGS = -mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall
CFLAGS += -I/root/code/TaskMate/TaskMate_current/src -MMD -MP
CFLAGS += -DTM_ARCH_AVR
