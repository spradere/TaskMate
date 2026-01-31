################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

################################################################################
# Build rules
################################################################################

# Info help system : targets begins with '_' or ${} are internal system only
# they'll not be displayed in `make help`

.MAIN: all

.BEGIN:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${LOG_DIR}

all: _system_critical_check ${AUTOCODE_STAMP} _dependency_check ${TARGET}
#@ [global] System build.
	@printf "\n%sAll done%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# dependency files used to compile sources if related header or source was edited
_dependency_check:
	@printf "\n%sCheck dependency files%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > ${DEPS_FILE}

# Test for autoCode required files
${AUTOCODE_STAMP}: ${AUTOCODE_TARGET} ${FILES_INIT_RC} ${ERROR_CAT} ${FILES_HAL_USER} ${FILES_HAL_SYSTEM}
	@printf "\n%sautoCode, init_rc or related sources files have changed -> run autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@rm -f ${AUTOCODE_LOG}*
.endif

	# write autoCode options
	@printf "# hardware target\n" > ${AUTOCODE_CONFIG}
	@printf "%s\n" "--arch ${ARCH}" >> ${AUTOCODE_CONFIG}
	@printf "%s\n" "--mcu ${MCU}" >> ${AUTOCODE_CONFIG}
	@printf "%s\n" "--board ${BOARD}" >> ${AUTOCODE_CONFIG}

	@printf "\n# files path\n" >> ${AUTOCODE_CONFIG}
	@printf "%s\n" "--errors ${ERROR_CAT}" >> ${AUTOCODE_CONFIG}
	@printf "%s\n" "--files_hal_user ${FILE_HAL_USER_PATH}" >> ${AUTOCODE_CONFIG}
	@printf "%s\n" "--files_hal_system ${FILE_HAL_SYSTEM_PATH}" >> ${AUTOCODE_CONFIG}

	# write list hal sources files
	@: > ${FILE_HAL_USER_PATH}
.for file in ${FILES_HAL_USER}
	@printf "%s\n" "${file}" >> ${FILE_HAL_USER_PATH}
.endfor
	@sed -i '' 's|${SRC_DIR}/||g' ${FILE_HAL_USER_PATH}

	@: > ${FILE_HAL_SYSTEM_PATH}
.for file in ${FILES_HAL_SYSTEM}
	@printf "%s\n" "${file}" >> ${FILE_HAL_SYSTEM_PATH}
.endfor
	@sed -i '' 's|${SRC_DIR}/||g' ${FILE_HAL_SYSTEM_PATH}

	./${AUTOCODE_TARGET} ${AUTOCODE_CONFIG} > ${AUTOCODE_LOG_STAMP}
	@touch ${AUTOCODE_STAMP}

# Special rule for autoCode with clang, not mcu specialized compiler
${AUTOCODE_TARGET}: ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${AUTOCODE_CFLAGS} ${AUTOCODE_SRCS} -o ${AUTOCODE_TARGET}

# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.for index in ${GREP_LIST}
	@allowed="${ALLOWED_LIST${index}}"; \
	pattern="${ALLOWED_PATTERN${index}}"; \
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

# Global errors
${ERROR_CAT}: ${ERROR_FILES}
	@printf "\n%sCat all *.err files in one for autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cat ${ERROR_FILES} > ${ERROR_CAT}

AUTOCODE_LS_CMD = ls -t ${AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat

autoCode_alone: ${AUTOCODE_TARGET}
#@ [global] Run autoCode alone.
	@printf "\n%sForce running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f ${AUTOCODE_STAMP}
	@${MAKE} ${AUTOCODE_STAMP}
	#@ls -t ${AUTOCODE_LOG}* | head -1 | xargs cat
	@${AUTOCODE_LS_CMD}
	@printf "${COLOUR_CYAN}"
	@${AUTOCODE_LS_CMD} | grep ': keep' | sed 's/^.*: *//'
	@printf "${COLOUR_RESET }${COLOUR_YELLOW}"
	@${AUTOCODE_LS_CMD} | grep ': change' | sed 's/^.*: *//'
	@printf "${COLOUR_RESET}"
.PHONY: autoCode_alone
