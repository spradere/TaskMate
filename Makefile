################################################################################
#
# TaskMate Project
# {c} 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, {c} 2025 PRADERE Sebastien
#
################################################################################

################################################################################
# Variables
################################################################################

# Compiler 
CC = avr-gcc
CLANG = clang

# MCU and Programmer settings
MCU = atmega2560
F_CPU = 16000000UL
PROGRAMMER = avrispmkII
PORT = /dev/ttyU0


# Source directories
SRC_DIRS = drivers tasks sysCore
# Automatically gather all C files
SRCS != find ${SRC_DIRS} -name "*.c"
# Files
TARGET = TaskMate
OBJ = ${SRCS:.c=.o} 
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf
DEP = ${OBJ:.o=.d}

# Compiler flags
CFLAGS = -mmcu=${MCU} -DF_CPU=${F_CPU} -O2 -Wall -I/root/code/TaskMate/TaskMate_current -MMD -MP

# Task/Driver list files
TASK_LIST_FILE = task_list
DRIVER_LIST_FILE = driver_list


################################################################################
# Build rules
################################################################################

all: .tag_expand_stamp header_check ${TARGET}
	
${TARGET}: ${OBJ}
	@printf "\n\033[1;33mLinking\033[0m\n\n" 
	#link objects
	${CC} ${CFLAGS} -o ${ELF} ${OBJ}

.c.o:
	@printf "\n\033[1;33mCompilation ...\033[0m\n\n" 
	${CC} ${CFLAGS} -c $< -o $@
	
# include dependency files safely, used to complie if header was edited
header_check:
	@printf "\n\033[1;33mCheck header files\033[0m\n\n" 
	@if ls ${DEP} >/dev/null 2>&1; then cat ${DEP}; fi > .deps
	
-include .deps

	
upload:all
	@printf "\n\033[1;33mUpload binary to AVR flash\033[0m\n\n" 
	#to hex format
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	#upload to atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
	
clean:
	@printf "\n\033[1;31mRemove files\033[0m\n\n" 
	rm -f  ${ELF} ${HEX} ${OBJ} *.out ${SRCS:.c=.d} .deps .tag_expand_stamp
	
dump:all
	@printf "\n\033[1;33mGenerate debugging informations\033[0m\n\n" 
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > hex.out
	avr-objdump -D -m avr6 ${ELF} > elf.out
	
doc:
	@printf "\n\033[1;36mMake Doxygen documentation\033[0m\n\n" 
	doxygen Doxyfile


.tag_expand_stamp: TaskMate_tag_expand ${DRIVER_LIST_FILE} ${TASK_LIST_FILE}
	@printf "\n\033[1;33mList have been updated\033[0m\n\n" 
	./TaskMate_tag_expand
	touch .tag_expand_stamp


# Special rule for TaskMate_tag_expand.c
TaskMate_tag_expand: TaskMate_tag_expand.o
	@printf "\n\033[1;33mTaskMate.c have been updated\033[0m\n\n" 
	${CLANG}  TaskMate_tag_expand.c -o TaskMate_tag_expand

