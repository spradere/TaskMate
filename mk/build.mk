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
# they'll not be displayed with 'make help'

.MAIN: all

.BEGIN:
	#@mkdir -p "${PATH_BUILDS_TARGET}"
	#@mkdir -p "${PATH_LOGS}"
	
.if make(upload)
	printf "%i" ${BUILD_CNT} > "${BUILD_CNT_FILE}"
.endif	

.if make(upload) || make(all)
	@printf '%s\n' "${VAL_TM_VERSION}" | \
	cmp -s - "${VAL_TM_VERSION_FILE}" 2>/dev/null || \
	printf '%s\n' "${VAL_TM_VERSION}" > "${VAL_TM_VERSION_FILE}"
.endif

.END:
	@printf "##########################\n" > "${BUILD_INFO}"
	@printf "# Last build informations \n" >> "${BUILD_INFO}"
	@printf "##########################\n\n" >> "${BUILD_INFO}"

	@printf "TaskMate %s\n" "${VAL_TM_VERSION}" >> "${BUILD_INFO}"
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
	@printf "%s\n" "${COLOUR_WHITE_BOLD}"
	@printf "##########################\n"
	@printf "# Build summary\n"
	@printf "##########################\n\n"
	@printf "\t%-16s : %s\n" "TaskMate version" "${VAL_TM_VERSION}"
	@printf "\t%-16s : %s -> %s -> %s\n" "Hardware target" "${ARCH}" "${MCU}" "${BOARD}"
	@printf "\t%-16s : %s\n" "build" "${BUILD_CNT}"

	@awk '\
		$$1 == "code_total" { \
			printf("\t%-16s : %s\n", "lines of code", $$2); \
		}' ${FILE_CLOCDATA}

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
		}' ${FILE_MEMDATA}

	@printf "${COLOUR_RESET}"
.endif

all: _system_critical_check ${AUTOCODE_STAMP} _dependency ${FILE_TARGET} _mcu_memory_data _cloc_data
#help [global] System build.
	@printf "\n%sBuild complete%s\n\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"
		
# dependency files used to compile sources if related header or source was edited
_dependency:
	@if ls ${DEPS} >/dev/null 2>&1; then cat ${DEPS}; fi > "${DEPS_FILE}"

# autoCode and required files
${AUTOCODE_STAMP}: 	${FILE_AUTOCODE_TARGET} ${FILES_INIT_RC} ${FILE_ERROR_CAT} \
					${VAL_TM_VERSION_FILE} ${BUILD_CNT_FILE}

	@printf "\n%sautoCode, init.rc or related sources files have changed -> run autoCode%s\n\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@rm -f ${AUTOCODE_LOG}*
.endif

	# write autoCode options
	@printf "# TaskMate version\n" > "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--tm_ver ${VAL_TM_VERSION}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--tm_build ${BUILD_CNT}" >> "${FILE_AUTOCODE_CONFIG}"

	@printf "\n# hardware target\n" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--arch ${ARCH}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--mcu ${MCU}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--board ${BOARD}" >> "${FILE_AUTOCODE_CONFIG}"

	@printf "\n# files path\n" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--errors ${FILE_ERROR_CAT}" >> "${FILE_AUTOCODE_CONFIG}"

	# launch autoCode
	./${FILE_AUTOCODE_TARGET} ${FILE_AUTOCODE_CONFIG} > "${AUTOCODE_LOG_STAMP}"
	@touch ${AUTOCODE_STAMP}

	# proceed log
	@awk ${COULOURS_AWK} '\
		$$1 == "[fileUtility.c]" { \
			if($$4 ~ /^\*/) {\
				temp = $$0; \
				sub(/^[^*]*/,"",temp); \
				print temp; \
				} \
			} \
		$$4 == "keep" { \
			temp = $$0; \
			sub(/^[^:]*: /,"",temp); \
			print COLOUR_CYAN, temp, COLOUR_RESET; \
			print temp >> "${AUTOCODE_LOG_STAMP}"; \
		}\
		$$4 == "change" { \
			temp = $$0; \
			sub(/^[^:]*: /,"",temp); \
			print COLOUR_YELLOW, temp, COLOUR_RESET; \
			print temp >> "${AUTOCODE_LOG_STAMP}"; \
		}' ${AUTOCODE_LOG_STAMP}

# Special rule for autoCode with clang, not arch specialized compiler
AUTOCODE_CFLAGS = -I${PATH_SOURCES}/
AUTOCODE_CFLAGS += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${FILE_AUTOCODE_TARGET}: ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${AUTOCODE_CFLAGS} ${AUTOCODE_SRCS} -o ${FILE_AUTOCODE_TARGET}

# Global errors
${FILE_ERROR_CAT}: ${ERROR_FILES}
	@printf "\n%sCat all *.err files in one for autoCode%s\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"
	@cat ${ERROR_FILES} > "${FILE_ERROR_CAT}"

# Run autoCode alone
autoCode_alone: ${FILE_AUTOCODE_TARGET}
#help [global] Run autoCode alone.
	@printf "\n%sForce running autoCode alone%s\n\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f "${AUTOCODE_STAMP}"
	@${MAKE} ${AUTOCODE_STAMP}
	@ls -t ${AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat
.PHONY: autoCode_alone
