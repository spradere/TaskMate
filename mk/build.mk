################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

################################################################################
# Build rules
################################################################################

# Info about help system : targets begins with '_' or '$' are internal system only
# they'll not be displayed in # make help

.MAIN: all

.BEGIN:
	@mkdir -p ${BUILD_DIR_TARGET}
	@mkdir -p ${LOG_DIR}
.if make(upload) || make(all)
	@printf '%s\n' "${TM_VERSION}" | \
	cmp -s - "${TM_VERSION_FILE}" 2>/dev/null || \
	printf '%s\n' "${TM_VERSION}" > "${TM_VERSION_FILE}"
.endif

.END:
	@printf "##########################\n" > "${BUILD_INFO}"
	@printf "# Last build informations \n" >> "${BUILD_INFO}"
	@printf "##########################\n\n" >> "${BUILD_INFO}"

	@printf "TaskMate %s\n" "${TM_VERSION}" >> "${BUILD_INFO}"
	@printf "date : " >> "${BUILD_INFO}"
	@date >> "${BUILD_INFO}"
	@printf "Hardware target : %s -> %s -> %s\n" "${ARCH}" "${MCU}" "${BOARD}" >> "${BUILD_INFO}"
	@printf "build counter for this target : %s\n" "${BUILD_CNT}" >> "${BUILD_INFO}"
	@git -v >> "${BUILD_INFO}"
	@printf "git tag : " >> "${BUILD_INFO}"
	@git describe --tags >> "${BUILD_INFO}"
	@printf "${CC} : " >> "${BUILD_INFO}"
	@printf "${CC_VER}\n" >> "${BUILD_INFO}"

.if make(upload) || make(all)
	@printf "%s\n" "${COLOUR_CYAN_BOLD}"
	@printf "##########################\n"
	@printf "# Build summary\n"
	@printf "##########################\n\n"
	@printf "\t%-16s : %s\n" "TaskMate version" "${TM_VERSION}"
	@printf "\t%-16s : %s -> %s -> %s\n" "Hardware target" "${ARCH}" "${MCU}" "${BOARD}"
	@printf "\t%-16s : %s\n" "build" "${BUILD_CNT}"

	@awk '\
		$$1 == "loc_total" { \
			printf("\t%-16s : %s\n", "lines of code", $$2); \
		}' ${CLOC_DATA}

	@awk '\
		NR > 1 { \
		name = $$1; \
		pct  = $$4; \
		printf("\t%-16s : %0.1f%%\n", name, pct); \
		if ($$4 > 98) \
			{\
			printf("${COLOUR_RED_BOLD}\t>>> ERROR: usage high > 98%% <<< ${COLOUR_RESET}\n"); \
			exit(1); \
			} \
		if ($$4 > 85) \
			printf("${COLOUR_YELLOW_BOLD}\t>>> WARNING: usage high > 85%% <<< ${COLOUR_CYAN_BOLD}\n"); \
		}' ${MEM_DATA}

	@printf "${COLOUR_RESET}"
.endif

all: _system_critical_check ${AUTOCODE_STAMP} _dependency _mcu_memory_data ${TARGET}
#@ [global] System build.
	@printf "\n%sBuild complete%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# dependency files used to compile sources if related header or source was edited
_dependency:
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > "${DEPS_FILE}"

# autoCode and required files
AUTOCODE_PRINT_LAST_LOG = ls -t ${AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat

${AUTOCODE_STAMP}: 	${AUTOCODE_TARGET} ${FILES_INIT_RC} ${ERROR_CAT} \
					${TM_VERSION_FILE} ${BUILD_CNT_FILE}

	@printf "\n%sautoCode, init.rc or related sources files have changed -> run autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@rm -f ${AUTOCODE_LOG}*
.endif

	# write autoCode options
	@printf "# TaskMate version\n" > "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--tm_ver ${TM_VERSION}" >> "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--tm_build ${BUILD_CNT}" >> "${AUTOCODE_CONFIG}"

	@printf "\n# hardware target\n" >> "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--arch ${ARCH}" >> "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--mcu ${MCU}" >> "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--board ${BOARD}" >> "${AUTOCODE_CONFIG}"

	@printf "\n# files path\n" >> "${AUTOCODE_CONFIG}"
	@printf "%s\n" "--errors ${ERROR_CAT}" >> "${AUTOCODE_CONFIG}"

	./${AUTOCODE_TARGET} ${AUTOCODE_CONFIG} > "${AUTOCODE_LOG_STAMP}"
	@touch ${AUTOCODE_STAMP}

	@${AUTOCODE_PRINT_LAST_LOG} | grep ': \*' | sed 's/^.* info : //'
	@printf "${COLOUR_YELLOW}"
	@${AUTOCODE_PRINT_LAST_LOG} | grep ': change' | sed 's/^.*: *//'
	@printf "${COLOUR_RESET}"

	@${AUTOCODE_PRINT_LAST_LOG} | grep ': keep' | sed 's/^.*: *//' >> "${AUTOCODE_LOG_STAMP}"
	@${AUTOCODE_PRINT_LAST_LOG} | grep ': change' | sed 's/^.*: *//' >> "${AUTOCODE_LOG_STAMP}"

# Special rule for autoCode with clang, not arch specialized compiler
AUTOCODE_CFLAGS = -I${SRC_DIR}/
AUTOCODE_CFLAGS += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${AUTOCODE_TARGET}: ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${AUTOCODE_CFLAGS} ${AUTOCODE_SRCS} -o ${AUTOCODE_TARGET}

# Global errors
${ERROR_CAT}: ${ERROR_FILES}
	@printf "\n%sCat all *.err files in one for autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cat ${ERROR_FILES} > "${ERROR_CAT}"

# Run autoCode alone
autoCode_alone: ${AUTOCODE_TARGET}
#@ [global] Run autoCode alone.
	@printf "\n%sForce running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f "${AUTOCODE_STAMP}"
	@${MAKE} ${AUTOCODE_STAMP}
	@${AUTOCODE_PRINT_LAST_LOG}
	#@printf "${COLOUR_CYAN}"
	#@${AUTOCODE_PRINT_LAST_LOG} | grep ': keep' | sed 's/^.*: *//'
	#@printf "${COLOUR_RESET} ${COLOUR_YELLOW}"
	#@${AUTOCODE_PRINT_LAST_LOG} | grep ': change' | sed 's/^.*: *//'
	#@printf "${COLOUR_RESET}"
.PHONY: autoCode_alone
