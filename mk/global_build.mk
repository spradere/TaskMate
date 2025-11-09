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
# Build rules
################################################################################

.MAIN: all

all: .autoCode_stamp header_check ${TARGET}
	@printf "\n\033[1;33mAll done\033[0m\n\n"

# Link
${TARGET}: ${OBJS}
	@printf "\n\033[1;33mLinking\033[0m\n\n"
	${CC} ${CFLAGS} -o ${ELF} ${OBJS}

# Compile
${OBJS}: ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}
	@printf "\n\033[1;33mCompilation ...\033[0m\n\n"
	@printf "source : <%s> -> <%s>\n" ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} ${.TARGET}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS} -c ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} -o ${.TARGET}

# Include dependency files used to compile *.c if related header was edited
header_check:
	@printf "\n\033[1;33mCheck header files\033[0m\n\n"
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${DEPS_FILE}

# Test if autoCode and initrc files was modified
.autoCode_stamp: ${AUTOCODE_TARGET} ${FILES_INIT_RC}
	@printf "\n\033[1;33minitrc have changed or autoCode.c -> run autoCode\033[0m\n\n"
	./${AUTOCODE_TARGET} ${ARCH} ${MCU} ${BOARD}
	touch .autoCode_stamp

# Special rule for autoCode with clang, not avr-gcc
${AUTOCODE_TARGET}: ${AUTOCODE_SRC}
	@printf "\n\033[1;33mCompiling autoCode\033[0m\n\n"
	clang -I/root/code/TaskMate/TaskMate_current/ ${AUTOCODE_SRC} -o ${AUTOCODE_TARGET}
