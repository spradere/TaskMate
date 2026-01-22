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

# Targets begins with '_' or ${} are internal system only
# They'll not be displayed in help: target

all: _system_critical_check ${AUTOCODE_STAMP} _dependency_check ${TARGET}
#@ [global] System build.
	@printf "\n%sAll done%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# Link
${TARGET}: ${OBJS}
	@printf "\n%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	${CC} ${CFLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections -flto -o ${ELF} ${OBJS}

# Compile
${OBJS}: ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}
	@printf "\n%sCompilation ...%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} ${.TARGET}
	@mkdir -p ${.TARGET:H}
	@${CC} ${CFLAGS} ${CFLAGS_${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}} \
		-c ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} -o ${.TARGET}

# dependency files used to compile *.c if related header or source was edited
_dependency_check:
	@printf "\n%sCheck dependency files%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${DEPS_FILE}

# Test if autoCode, initrc and error files was modified
${AUTOCODE_STAMP}: ${AUTOCODE_TARGET} ${FILES_INIT_RC} ${ERROR_CAT} ${HAL_FILES_USER} ${HAL_FILES_SYSTEM}
	@printf "\n%sautoCode, init_rc or error files have changed -> run autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f ${LOG_DIR}/autoCode_*
	# list hal sources files
	@printf "%s" "${HAL_FILES_USER}" > ${BUILD_DIR}/hal_files_user
	@sed -i '' 's|src/||g' ${BUILD_DIR}/files_hal_user
	@printf "%s" "${HAL_FILES_SYSTEM}" > ${BUILD_DIR}/hal_files_system
	@sed -i '' 's|src/||g' ${BUILD_DIR}/files_hal_system

	./${AUTOCODE_TARGET} ${ARCH} ${MCU} ${BOARD} ${ERROR_CAT} > ${LOG_DIR}/autoCode_${AUTOCODE_DATE_TIME}
	@touch ${AUTOCODE_STAMP}

# Special rule for autoCode with clang, not mcu specialized compiler
${AUTOCODE_TARGET}: ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${AUTOCODE_CFLAGS} ${AUTOCODE_SRCS} -o ${AUTOCODE_TARGET}

# check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
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
	       printf "%s[ FAIL ] Forbidden include detected in: %s%s\n" "${COLOUR_FAIL}" "$$f" "${COLOUR_RESET}"; \
	       exit 1; \
		else \
			printf "%s[  OK  ]%s %s\n" "${COLOUR_OK}" "${COLOUR_RESET}" "$$f"; \
	    fi; \
	done
.endfor

# global errors
${ERROR_CAT}: ${ERROR_FILES}
	@printf "\n%sCat all *.err in one file for autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cat ${ERROR_FILES} > ${ERROR_CAT}

# special rule for autoCode alone
autoCode_alone: ${AUTOCODE_TARGET}
#@ [global] Build and run autoCode alone.
	@printf "\n%sCompiling and running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@./${AUTOCODE_TARGET} ${ARCH} ${MCU} ${BOARD} ${ERROR_CAT}
.PHONY: autoCode_alone
