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

# Get git tag for USB folder backup
USB_FOLDER = /media/usbkey
USB_DEV = /dev/da0s1
GIT_TAG != git describe --tags | cut -d'-' -f1 | sed 's/^v//'
TASKMATE_FOLDER != printf "/code/TaskMate/TaskMate_${GIT_TAG}"


################################################################################
# Build rules
################################################################################

all: .tag_expand_stamp header_check ${TARGET}
	@printf "\n\033[1;33mAll done\033[0m\n\n" 
	
# Link	
${TARGET}: ${OBJ}
	@printf "\n\033[1;33mLinking\033[0m\n\n" 
	${CC} ${CFLAGS} -o ${ELF} ${OBJ}

# Compile
.c.o:
	@printf "\n\033[1;33mCompilation ...\033[0m\n\n" 
	${CC} ${CFLAGS} -c $< -o $@
	
# Include dependency files safely, used to compile *.c if related header was edited
header_check:
	@printf "\n\033[1;33mCheck header files\033[0m\n\n" 
	@if ls ${DEP} >/dev/null 2>&1; then cat ${DEP}; fi > .deps
	
-include .deps

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
	rm -f  ${ELF} ${HEX} ${OBJ} *.out ${SRCS:.c=.d} .deps .tag_expand_stamp

# Disassemble machine code in two formats
dump:all
	@printf "\n\033[1;33mGenerate debugging informations\033[0m\n\n" 
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > hex.out
	avr-objdump -D -m avr6 ${ELF} > elf.out

# Make doxygen documentation
doc:
	@printf "\n\033[1;36mMake Doxygen documentation\033[0m\n\n" 
	doxygen Doxyfile

# Test if mofified list files
.tag_expand_stamp: TaskMate_tag_expand ${DRIVER_LIST_FILE} ${TASK_LIST_FILE}
	@printf "\n\033[1;33mList have been updated\033[0m\n\n" 
	./TaskMate_tag_expand
	touch .tag_expand_stamp

# Special rule for TaskMate_tag_expand.c with clang
TaskMate_tag_expand: TaskMate_tag_expand.o
	${CLANG} -o TaskMate_tag_expand TaskMate_tag_expand.o
	
TaskMate_tag_expand.o: TaskMate_tag_expand.c
	@printf "\n\033[1;33mTaskMate.c have been updated\033[0m\n\n" 
	${CLANG} -c $< -o $@


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
	@printf "\n\033[1;33mBackup to <${USB_FOLDER}${TASKMATE_FOLDER}>\033[0m\n\n"
	@printf "\033[0;33mInsert USB key and press ENTER to continue ... \033[0m\n"
		
	@read DUMMY_VAR
	
	#Test if USB key is mount, do if not
	@if mount | grep "/media/usbkey" > /dev/null; then \
		printf "\033[0;33mUSB key already mounted${USB_FOLDER}\033[0m\n"; \
	else \
		printf "\033[0;33mMount USB key ${USB_FOLDER}\033[0m\n"; \
		mount -v -t msdosfs ${USB_DEV} ${USB_FOLDER}; \
	fi
	
	# Test if dest folder exist, create if not	
	@if [ -d "${USB_FOLDER}${TASKMATE_FOLDER}" ]; then \
	else \
		mkdir ${USB_FOLDER}${TASKMATE_FOLDER}; \
	fi
	# Run rsync
	@printf "\033[0;33mRun rsync, output logged in rsync.log\033[0m\n"
	rsync -av * --progress --delete --exclude	"*.o" --exclude="html". "${USB_FOLDER}${TASKMATE_FOLDER}/" > rsync.log
	
	# Umount
	@printf "\033[0;33mUmount ${USB_FOLDER}\033[0m\n"
	@umount ${USB_FOLDER}
	@printf "\n"
