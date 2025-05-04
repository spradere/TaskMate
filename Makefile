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
SRC_DIR = src/
SRC_DIR_LIST = src/drivers/
SRC_DIR_LIST += src/services/
SRC_DIR_LIST += src/sysCore/
SRC_DIR_LIST += src/tasks/

# Automatically gather all needed files
SRCS != find ${SRC_DIR_LIST} -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -name "*.h"

# autoCode
AUTOCODE_TARGET = utility/autoCode
AUTOCODE_SRC != find utility/autoCode_src/ -name "*.c"

# Build files and directory
BUILD_DIR = build/
TARGET = TaskMate
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf

OBJS = ${SRCS:${SRC_DIR}%.c=${BUILD_DIR}%.o}
OBJS_ONE_DIR = ${OBJS:S/drivers\///:S/services\///:S/sysCore\///:S/tasks\///}
OBJS_COUNT != seq  ${OBJS_ONE_DIR:[#]}

# Dependency files
DEPS = ${OBJS_ONE_DIR:.o=.d}
DEPS_FILE = .deps.d

# Compiler flags
CFLAGS = -mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall
CFLAGS += -I/root/code/TaskMate/TaskMate_current/src -MMD -MP

# Initrc files
FILES_INIT_RC = src/drivers/drivers_init.rc
FILES_INIT_RC += src/services/services_init.rc
FILES_INIT_RC += src/tasks/tasks_init.rc

# Get git tag for USB key directory backup
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1
GIT_TAG != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || echo "0.00"
TASKMATE_DIR != printf "/code/TaskMate/TaskMate_%s" ${GIT_TAG}


################################################################################
# Build rules
################################################################################

all: .autoCode_stamp header_check ${TARGET}
	@printf "\n\033[1;33mAll done\033[0m\n\n"

# Link
${TARGET}: ${OBJS_ONE_DIR}
	@printf "\n\033[1;33mLinking\033[0m\n\n"
	${CC} ${CFLAGS} -o ${ELF} ${OBJS_ONE_DIR}

# Compile
.for index in ${OBJS_COUNT}
${OBJS_ONE_DIR:[${index}]}: ${SRCS:[${index}]}
	@printf "\n\033[1;33mCompilation ...\033[0m\n\n"
	@printf "source : <%s> -> <%s>\n" ${.ALLSRC:[1]} ${.TARGET}
	${CC} ${CFLAGS} -c  ${.ALLSRC:[1]} -o ${.TARGET}
.endfor

# Include dependency files used to compile *.c if related header was edited
header_check:
	@printf "\n\033[1;33mCheck header files\033[0m\n\n"
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${DEPS_FILE}

-include ${DEPS_FILE}

# Test if autoCode and initrc files was modified
.autoCode_stamp: ${AUTOCODE_TARGET} ${FILES_INIT_RC}
	@printf "\n\033[1;33mList have changed or autoCode.c -> run autoCode\033[0m\n\n"
	./${AUTOCODE_TARGET}
	touch .autoCode_stamp

# Special rule for autoCode with clang, not avr-gcc
${AUTOCODE_TARGET}: ${AUTOCODE_SRC}
	@printf "\n\033[1;33mCompiling autoCode\033[0m\n\n"
	clang -I/root/code/TaskMate/TaskMate_current/ ${AUTOCODE_SRC} -o ${AUTOCODE_TARGET}


################################################################################
# Utility
################################################################################

# Flash Gordon
upload:all
	@printf "\n\033[1;33mUpload binary to AVR flash\033[0m\n\n"
	# ELF to hex format
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	# RAM usage
	@printf "\nStatic RAM usage : "
	avr-size -B ${HEX}
	@printf "\n"
	# Upload to Atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
.PHONY: upload

# Heavy sweep
clean:
	@printf "\n\033[1;31mRemove files\033[0m\n\n"
	rm -f  ${ELF} ${HEX} ${OBJS_ONE_DIR}
	rm -f ${DEPS}
	rm -f autoCode_stamp ${AUTOCODE_TARGET}
.PHONY: clean

# Disassemble machine code in two formats
dump:all
	@printf "\n\033[1;33mGenerate debugging informations\033[0m\n\n"
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > hex.out
	avr-objdump -D -m avr6 ${ELF} > elf.out
.PHONY: dump

# Make doxygen documentation
doc:
	@printf "\n\033[1;36mMake Doxygen documentation\033[0m\n\n"
	doxygen doc/Doxyfile
.PHONY: doc

# Count lines of code
cloc:
	@cloc * --exclude-dir=html --exclude-lang=D --exclude-ext=rc
.PHONY: cloc

# clang-tidy
TIDY_SRC := ${AUTOCODE_SRC}
TIDY_SRC += ${SRCS}

tidy:
	@printf "\n\033[1;33mTidy static test code, config in .clang-tidy\033[0m\n\n"
	clang-tidy $(TIDY_SRC) -- \
		-I/root/code/TaskMate/TaskMate_current/src \
		-I/root/code/TaskMate/TaskMate_current/ \
		-isystem /usr/local/avr/include -isystem /usr/local/lib/gcc/avr/14.1.0 \
		-D__AVR__=6 -D__AVR_ATmega2560__=1
.PHONY: tidy

# clang-format
format:
	@printf "\033[0;33mAuto formatting code, config in .clang-format\033[0m\n\n"
	clang-format -i $(SRCS) $(SRCS_H) $(AUTOCODE_SRC)
.PHONY: format


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
.PHONY: push

# USB key backup with current tag in directory
backup:
	@printf "\n\033[1;33mBackup to <${USB_DIR}${TASKMATE_DIR}>\033[0m\n\n"
	@printf "\033[0;33mInsert USB key and press ENTER to continue ... \033[0m\n"
	@read DUMMY_VAR

	#Test if USB key is mount, do if not
	@if mount | grep "/media/usbkey" > /dev/null; then \
		printf "\033[0;33mUSB key already mounted ${USB_DIR}\033[0m\n"; \
	else \
		printf "\033[0;33mMount USB key ${USB_DIR}\033[0m\n"; \
		mount -v -t msdosfs ${USB_DEV} ${USB_DIR}; \
	fi

	# Test if dest directory exist, create if not
	@if [ -d "${USB_DIR}${TASKMATE_DIR}" ]; then \
	else \
		mkdir ${USB_DIR}${TASKMATE_DIR}; \
	fi

	# Run rsync
	@printf "\033[0;33mRun rsync, output logged in log/rsync.log\033[0m\n"
	rsync -av * --progress --delete --exclude "*.o" --exclude="html" "${USB_DIR}${TASKMATE_DIR}/" > log/rsync.log

	# Umount
	@printf "\033[0;33mUmount ${USB_DIR}\033[0m\n"
	@umount ${USB_DIR}
	@printf "\n"
.PHONY: backup
