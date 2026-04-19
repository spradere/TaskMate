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
# Utility
################################################################################

clean:
#help [global] Remove all build files.
	@printf "\n%sRemove files%s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
	@printf "${COLOUR_CLEAN_SOFT}"
	rm -f ${OBJS} ${DEPS} ${BUILD_DIR_TARGET}/TaskMate*
	rm -f ${AUTOCODE_TARGET} ${BUILD_DIR_TARGET}/.autoCode_stamp* ${BUILD_DIR_TARGET}/autoCode_*
	@printf "${COLOUR_RESET}"
.PHONY: clean

doc:
#help [global] Generate Doxygen documentation. Configuration file /doc/Doxyfile
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	doxygen ${DOC_DIR}/Doxyfile
.PHONY: doc

_cloc_data:
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D --exclude-ext=rc,md,txt > ${CLOC_RAW}
	@printf "\n" >> ${CLOC_RAW}
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D,make --exclude-ext=rc,c,h,awk >> ${CLOC_RAW}
	@awk -v file="${CLOC_DATA}" -f scripts/cloc_data.awk ${CLOC_RAW}
.PHONY: _cloc_data

cloc: _cloc_data
#help [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@cat ${CLOC_RAW}
	@printf "${COLOUR_WHITE_BOLD}\nTotal loc and ratio :\n"
	@awk -f scripts/cloc_show.awk "${CLOC_DATA}"
	@printf "${COLOUR_RESET}"
.PHONY: cloc

note:
#help [global] Look for TODO / FIX / HACK comments in code.
	@printf "\n%sLook for TODO / FIX / HACK%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@grep -r -n -i -E 'TODO|FIX|HACK' ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}

.PHONY: note

cppcheck:
#help [global] cppcheck static code analysis for autoCode and TaskMate.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cppcheck -I/root/code/TaskMate/TaskMate_current/ \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${SRCS} \
		${AUTOCODE_SRCS}
.PHONY: cppcheck

clang_format:
#help [global] Formatting code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS}
.PHONY: clang_format

tidy_autoCode:
#help [global] tidy static code analysis for autoCode, configuration /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy $(AUTOCODE_SRCS) ${AUTOCODE_SRCS_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${SRC_DIR}/
.PHONY:tidy_autoCode

help:
#help [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@awk ${AWK_COLOURS} '\
		/^([A-Za-z0-9][A-Za-z0-9_-]*):/ { \
			print COLOUR_HELP_TARGET $$1 COLOUR_RESET; \
			} \
		$$1 == "#help" {\
			printf("  %s%s%s", COLOUR_HELP_TAG, $$2, COLOUR_RESET); \
			temp = $$0; \
			sub(/.*\]/,"",temp); \
			print temp; \
		}\
	' ${MK_FILES}
.PHONY: help
