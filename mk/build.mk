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

# Help system: targets beginning with '_' or '$' are internal only
# and are not displayed by 'make help'.

.BEGIN: ${FILE_PROGRAMS_CHECK_STAMP}
	@mkdir -p "${PATH_BUILD_TARGET}"
	@mkdir -p "${PATH_LOGS}"
	@mkdir -p "${PATH_BUILD_GENERATED}"
	
.if make(upload) || make(all)
	@printf "// #####################################\n" > "${FILE_TM_INFO}.tmp"
	@printf "// # TaskMate informations informations \n" >> "${FILE_TM_INFO}.tmp"
	@printf "// #####################################\n\n" >> "${FILE_TM_INFO}.tmp"

	@printf "#define TM_VER_MAJOR %s\n" "${VAL_TM_VER_MAJOR}" >> "${FILE_TM_INFO}.tmp"
	@printf "#define TM_VER_MINOR %s\n" "${VAL_TM_VER_MINOR}" >> "${FILE_TM_INFO}.tmp"

	@printf "#define TM_BUILD %s\n" "${VAL_BUILD_CNT}" >> "${FILE_TM_INFO}.tmp"
	
	@if ! cmp -s "${FILE_TM_INFO}.tmp" "${FILE_TM_INFO}"; then \
		mv "${FILE_TM_INFO}.tmp" "${FILE_TM_INFO}"; \
	else \
		if [ -n "${FILE_TM_INFO}" ] && [ -f "${FILE_TM_INFO}.tmp" ]; then \
			find "${FILE_TM_INFO}.tmp" -type f -delete; \
		fi \
	fi
.endif
	
# Check required programs once
${FILE_PROGRAMS_CHECK_STAMP}: ${CONF_PROGRAMS_LIST} ${SCRIPT_CHECK_PROGRAMS}
	@printf "%sChecking required programs ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${SCRIPT_CHECK_PROGRAMS} "${CONF_PROGRAMS_LIST}"
	@mkdir -p "${PATH_BUILDS}"
	@touch "${FILE_PROGRAMS_CHECK_STAMP}"

.END:
.if make(upload) || make(all)
	@printf "##########################\n" > "${FILE_BUILD_INFO}"
	@printf "# Last build informations \n" >> "${FILE_BUILD_INFO}"
	@printf "##########################\n\n" >> "${FILE_BUILD_INFO}"

	@printf "TaskMate %s\n" "${VAL_TM_VERSION}" >> "${FILE_BUILD_INFO}"
	@printf "date : " >> "${FILE_BUILD_INFO}"
	@date >> "${FILE_BUILD_INFO}"
	@printf "Hardware target : %s\n" "${VAL_HW_STACK}" >> "${FILE_BUILD_INFO}"
	@printf "build counter for this target : %s\n" "${VAL_BUILD_CNT}" >> "${FILE_BUILD_INFO}"
	@git -v >> "${FILE_BUILD_INFO}"
	@printf "git tag : " >> "${FILE_BUILD_INFO}"
	@git describe --tags >> "${FILE_BUILD_INFO}"
	@printf "${CC} : " >> "${FILE_BUILD_INFO}"
	@printf "${VAL_CC_VERSION}\n" >> "${FILE_BUILD_INFO}"

	@printf "%s\n" "${COLOUR_WHITE_BOLD}"
	@printf "##########################\n"
	@printf "# Build summary\n"
	@printf "##########################\n\n"
	@printf "\t%-16s : %s\n" "TaskMate version" "${VAL_TM_VERSION}"
	@printf "\t%-16s : %s\n" "Hardware target" "${VAL_HW_STACK}"
	@printf "\t%-16s : %s\n" "build" "${VAL_BUILD_CNT}"

	@awk -f ${SCRIPT_BUILD_SUMMARY_CLOC} "${FILE_CLOCDATA}"

	@awk ${COLOURS_AWK} -f ${SCRIPT_BUILD_SUMMARY_MEMORY} "${FILE_MEMDATA}"

	@printf "${COLOUR_RESET}"
.endif

all: ${FILE_GIT_IGNORE} _hardware_target_check _system_critical_check \
	_hal_public_removal_check _autocode .WAIT _architecture_include_check .WAIT _dependency \
	${FILE_TARGET} _mcu_memory_data _cloc_data
#help [global] TaskMate build.
	@printf "\n%sBuild complete%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# Dependency files used to compile sources when a related header or source file changes
_dependency:
	@if ls ${FILES_DEP} >/dev/null 2>&1; then \
		for file in ${FILES_DEP}; do tr -d '\r' < "$$file"; done; \
	fi > "${FILE_DEPS_ALL}"
