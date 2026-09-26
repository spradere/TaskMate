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

.BEGIN: ${FILE_PROGRAMS_CHECK_STAMP}
.if make(upload) || make(all)

	@mkdir -p "${PATH_BUILD_TARGET}"
	@mkdir -p "${PATH_LOGS}"
	@mkdir -p "${PATH_BUILD_GENERATED}"
	
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
	@awk -v expected_major="${VAL_BUILD_AUTOCODE_EXPECTED_VER_MAJOR}" \
		-v expected_minor="${VAL_BUILD_AUTOCODE_EXPECTED_VER_MINOR}" \
		-v report_versions=1 -f "${SCRIPT_AUTOCODE_VERSION}" \
		"${FILE_AUTOCODE_HEADER}" >> "${FILE_BUILD_INFO}"

	@printf "%s\n" "${COLOUR_WHITE_BOLD}"
	@printf "##########################\n"
	@printf "# Build summary\n"
	@printf "##########################\n\n"
	@printf "\t%-16s : %s\n" "TaskMate version" "${VAL_TM_VERSION}"
	@printf "\t%-16s : %s\n" "Hardware target" "${VAL_HW_STACK}"
	@printf "\t%-16s : %s\n" "build" "${VAL_BUILD_CNT}"

	@awk -f ${SCRIPT_BUILD_SUMMARY_CLOC} "${FILE_CLOCDATA}"

	@printf "${COLOUR_RED_BOLD}"
	@awk -f ${SCRIPT_BUILD_SUMMARY_MEMORY} "${FILE_MEMDATA}"

	@printf "${COLOUR_RESET}"
.endif

all: ${FILE_GIT_IGNORE} _hardware_target_check \
	_autocode .WAIT _architecture_include_check .WAIT _dependency \
	${FILE_TARGET} _mcu_memory_data _cloc_data
#help [global] TaskMate build.
	@printf "\n%sBuild complete%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# Dependency files used to compile sources when a related header or source file changes
_dependency:
	@if ls ${FILES_DEP} >/dev/null 2>&1; then \
		for file in ${FILES_DEP}; do tr -d '\r' < "$$file"; done; \
	fi > "${FILE_DEPS_ALL}"

.PHONY: clean_hard
clean_hard:
#help [global] Remove all build files for current target.
	@${SCRIPT_CHECK_PATH_FILE} -d "${PATH_BUILD_TARGET}" "${PATH_BUILDS}"
	@printf "\n%sRemove all files: ${PATH_BUILDS}/*\nRemove all files and subdir: ${PATH_BUILD_TARGET}/* %s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
		
	@printf "${COLOUR_CLEAN_SOFT}"
	find "${PATH_BUILDS}" -maxdepth 1 -type f -delete
	find "${PATH_BUILD_TARGET}" -mindepth 1 -depth -delete
	@printf "${COLOUR_RESET}"

.PHONY: clean
clean:
#help [global] Remove base build files for current target.
	@${SCRIPT_CHECK_PATH_FILE} -d \
		"${PATH_BUILD_TARGET}" \
		"${PATH_BUILDS}"
	@${SCRIPT_CHECK_PATH_FILE} -f \
		"${FILE_AUTOCODE_TARGET}"
		
	@printf "\n%sRemove files :%s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
	@printf "${COLOUR_CLEAN_SOFT}"
	@printf "${PATH_BUILD_TARGET}/**/*.o\n"
	@printf "${PATH_BUILD_TARGET}/**/*.d\n"
	@printf "${PATH_BUILD_TARGET}/TaskMate*\n"
	@printf "${FILE_AUTOCODE_TARGET}\n"
	@printf "${PATH_BUILD_TARGET}/*autoCode*\n"
	@printf "${COLOUR_RESET}"
		
	@-find "${PATH_BUILD_TARGET}" -type f \( -name "*.o" -o -name "*.d" \) -delete
	@-find "${PATH_BUILD_TARGET}" -maxdepth 1 -type f -name "TaskMate*" -delete
	@-find "${FILE_AUTOCODE_TARGET}" -type f -delete
	@-find "${PATH_BUILD_TARGET}" -maxdepth 1 -type f -name "*autoCode*" -delete
	@-find "${PATH_BUILDS}" -maxdepth 1 -type f -name "autoCode" -delete
