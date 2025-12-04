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

all: system_critical_check ${AUTOCODE_STAMP} dependency_check ${TARGET}
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
	#${CC} ${CFLAGS} -c ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} -o ${.TARGET}
	${CC} ${CFLAGS} ${CFLAGS_${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}} \
		-c ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} -o ${.TARGET}

# Include dependency files used to compile *.c if related header was edited
dependency_check:
	@printf "\n\033[1;33mCheck dependency files\033[0m\n\n"
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${DEPS_FILE}

# Test if autoCode and initrc files was modified
${AUTO_HEADERS}: ${AUTOCODE_STAMP}

${AUTOCODE_STAMP}: ${AUTOCODE_TARGET} ${FILES_INIT_RC}
	@printf "\n\033[1;33minitrc have changed or autoCode.c -> run autoCode\033[0m\n\n"
	./${AUTOCODE_TARGET} ${ARCH} ${MCU} ${BOARD} > build/autoCode_${AUTOCODE_TIMESTAMP}
	touch ${AUTOCODE_STAMP}

# Special rule for autoCode with clang, not avr-gcc
${AUTOCODE_TARGET}: ${AUTOCODE_SRC}
	@printf "\n\033[1;33mCompiling autoCode\033[0m\n\n"
	clang -I/root/code/TaskMate/TaskMate_current/ ${AUTOCODE_SRC} -o ${AUTOCODE_TARGET}

# check #include for system critical features
system_critical_check:
	@printf "\n\033[1;33mChecking forbidden system critical includes ...\033[0m\n"

.for index in ${GREP_LIST}
	@allowed="${GREP_ALLOWED${index}}"; \
	pattern="${GREP_PATTERN${index}}"; \
	printf "\nChecking pattern %s...\n" "$$pattern"; \
	files="`grep -R -l "$$pattern" ${SRC_DIR} 2>/dev/null || true`"; \
	for f in $$files; do \
	    test=no; \
	   for ok in $$allowed; do \
	        [ "$$f" = "$$ok" ] && test=yes; \
	    done; \
	    if [ "$$test" = "no" ]; then \
	       printf "\033[1;31m[ FAIL ] Forbidden include detected in: $$f\033[0m\n"; \
	       exit 1; \
		else \
			printf "\033[1;32m[  OK  ]\033[0m %s\n" "$$f"; \
	    fi; \
	done
.endfor
