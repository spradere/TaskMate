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

################################################################################
# Variables
################################################################################

# Compiler 
CC = avr-gcc

# MCU and Programmer settings
MCU = atmega2560
F_CPU = 16000000UL
PROGRAMMER = avrispmkII
PORT = /dev/ttyU0

# Source directory
BUILD_DIR = build
SRC_DIR = src
SRC_DIR_LIST = ${SRC_DIR}/drivers
SRC_DIR_LIST += ${SRC_DIR}/tasks
SRC_DIR_LIST += ${SRC_DIR}/sysCore

# Automatically gather all needed files
SRCS != find ${SRC_DIR_LIST} -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -name "*.h"

# Files
TARGET = TaskMate
OBJS = ${SRCS:${SRC_DIR}/%.c=${BUILD_DIR}/%.o}
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf
DEPS = ${OBJS:.o=.d}

# Compiler flags
CFLAGS = -mmcu=${MCU} -DF_CPU=${F_CPU} -O2 -Wall
CFLAGS += -I/root/code/TaskMate/TaskMate_current/src -MMD -MP

# Task/Driver list files
UTILITY_DIR = utility
TASK_LIST_FILE = ${UTILITY_DIR}/task_list
DRIVER_LIST_FILE = ${UTILITY_DIR}/driver_list

# Get git tag for USB directory backup
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1
GIT_TAG != git describe --tags | cut -d'-' -f1 | sed 's/^v//'
TASKMATE_DIR != printf "/code/TaskMate/TaskMate_${GIT_TAG}"


################################################################################
# Build rules
################################################################################

all: .list_stamp header_check ${TARGET}
	@printf "\n\033[1;33mAll done\033[0m\n\n"
	
# Link
${TARGET}: ${OBJS}
	@printf "\n\033[1;33mLinking\033[0m\n\n"
	${CC} ${CFLAGS} -o ${ELF} ${OBJS}

# Compile
${OBJS}: ${@:${BUILD_DIR}/%.o=${SRC_DIR}/%.c}
	@printf "\n\033[1;33mCompilation ...\033[0m\n\n"
	
	@printf "source : <%s> -> <%s>\n" ${@:${BUILD_DIR}/%.o=${SRC_DIR}/%.c} $@
	${CC} ${CFLAGS} -c ${@:${BUILD_DIR}/%.o=${SRC_DIR}/%.c} -o $@
	
# Include dependency files safely, used to compile *.c if related header was edited
header_check:
	@printf "\n\033[1;33mCheck header files\033[0m\n\n" 
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${BUILD_DIR}/.deps
	
-include ${BUILD_DIR}/.deps

# Test if mofified list files
.list_stamp: autoCode ${DRIVER_LIST_FILE} ${TASK_LIST_FILE}
	@printf "\n\033[1;33mList have changed\033[0m\n\n"
	./${UTILITY_DIR}/autoCode
	touch .list_stamp

# Special rule for autoCode.c with clang
autoCode: ${UTILITY_DIR}/autoCode.o
	clang -o ${UTILITY_DIR}/autoCode ${UTILITY_DIR}/autoCode.o
	
${UTILITY_DIR}/autoCode.o: ${UTILITY_DIR}/autoCode.c
	@printf "\n\033[1;33mautoCode\033[0m\n\n" 
	clang -c ${UTILITY_DIR}/autoCode.c -o ${UTILITY_DIR}/autoCode.o


################################################################################
# Utility
################################################################################

# Flash Gordon
upload:all
	@printf "\n\033[1;33mUpload binary to AVR flash\033[0m\n\n"
	#to hex format
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	#upload to atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
	
# Heavy sweep
clean:
	@printf "\n\033[1;31mRemove files\033[0m\n\n"
	rm -f  ${ELF} ${HEX} ${OBJS} *.out ${SRCS:.c=.d} .deps
	rm -f ${UTILITY_DIR}/.list_stamp ${UTILITY_DIR}/autoCode.o ${UTILITY_DIR}/autoCode

# Disassemble machine code in two formats
dump:all
	@printf "\n\033[1;33mGenerate debugging informations\033[0m\n\n"
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > hex.out
	avr-objdump -D -m avr6 ${ELF} > elf.out
	
# Make doxygen documentation
doc:
	@printf "\n\033[1;36mMake Doxygen documentation\033[0m\n\n" 
	doxygen doc/Doxyfile

.PHONY: doc

# Count lines of code
cloc:
	@cloc * --exclude-dir=html --exclude-lang=D

# clang-tidy
TIDY_SRC := utility/autoCode.c
TIDY_SRC += ${SRCS}

tidy:
	@printf "\n\033[1;33mTidy static test code, config in .clang-tidy\033[0m\n\n"
	clang-tidy $(TIDY_SRC) -- \
		-Isrc I/root/code/TaskMate/TaskMate_current/src \
		-isystem /usr/local/avr/include -isystem /usr/local/lib/gcc/avr/14.1.0 \
		-D__AVR__=6 -D__AVR_ATmega2560__=1

# clang-format
format:
	printf "\033[0;33mAuto formating code, config in .clang-format\033[0m\n\n"
	#clang-format -i $(SRCS) $(SRCS_H)


################################################################################
# Backup
################################################################################

# Git push, use command line : # make push M="message"
push:
	@printf "\n\033[1;33mGit routine for \"${M}\" commit\033[0m\n\n"
	@git add .
	@git commit -m "${M}"
	@git push
	@printf "\n"

# USB key backup with current tag folder
backup:
	@printf "\n\033[1;33mBackup to <${USB_DIR}${TASKMATE_DIR}>\033[0m\n\n"
	@printf "\033[0;33mInsert USB key and press ENTER to continue ... \033[0m\n"
	
	@read DUMMY_VAR
	
	#Test if USB key is mount, do if not
	@if mount | grep "/media/usbkey" > /dev/null; then \
		printf "\033[0;33mUSB key already mounted${USB_DIR}\033[0m\n" \
	else \
		printf "\033[0;33mMount USB key ${USB_DIR}\033[0m\n" \
		mount -v -t msdosfs ${USB_DEV} ${USB_DIR}; \
	fi
	
	# Test if dest folder exist, create if not
	@if [ -d "${USB_DIR}${TASKMATE_DIR}" ]; then \
	else \
		mkdir ${USB_DIR}${TASKMATE_DIR}; \
	fi
	
	# Run rsync
	@printf "\033[0;33mRun rsync, output logged in log/rsync.log\033[0m\n"
	rsync -av * --progress --delete --exclude	"*.o" --exclude="html". "${USB_DIR}${TASKMATE_DIR}/" > log/rsync.log
	
	# Umount
	@printf "\033[0;33mUmount ${USB_DIR}\033[0m\n"
	@umount ${USB_DIR}
	@printf "\n"
