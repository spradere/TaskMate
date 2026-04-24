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
	rm -f ${FILES_OBJ} ${FILES_DEP} ${PATH_BUILD_TARGET}/TaskMate*
	rm -f ${FILE_AUTOCODE_TARGET} ${PATH_BUILD_TARGET}/.autoCode_stamp* ${PATH_BUILD_TARGET}/autoCode_*
	@printf "${COLOUR_RESET}"
.PHONY: clean

doc:
#help [global] Generate Doxygen documentation. Configuration file /doc/Doxyfile
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	doxygen ${PATH_DOCS}/Doxyfile
.PHONY: doc

_cloc_data:
	@cloc * --exclude-dir=${PATH_BUILDS},${PATH_LOGS} --exclude-lang=D --exclude-ext=rc,md,txt > ${FILE_CLOCRAW}
	@printf "\n" >> ${FILE_CLOCRAW}
	@cloc * --exclude-dir=${PATH_BUILDS},${PATH_LOGS} --exclude-lang=D,make --exclude-ext=rc,c,h,awk >> ${FILE_CLOCRAW}
	@awk -v file="${FILE_CLOCDATA}" -f ${PATH_SCRIPTS}/cloc_data.awk ${FILE_CLOCRAW}
.PHONY: _cloc_data

cloc: _cloc_data
#help [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@cat ${FILE_CLOCRAW}
	@printf "${COLOUR_WHITE_BOLD}\nTotal loc and ratio :\n"
	@awk -f ${PATH_SCRIPTS}/cloc_show.awk "${FILE_CLOCDATA}"
	@printf "${COLOUR_RESET}"
.PHONY: cloc

note:
#help [global] Look for TODO / FIX / HACK comments in code.
	@printf "\n%sLook for TODO / FIX / HACK%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@grep -r -n -i -E 'TODO|FIX|HACK' ${FILES_SRC} ${FILES_SRC_H} ${FILES_AUTOCOE_SRC} ${FILES_AUTOCOE_SRC_H}

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
		${FILES_SRC} \
		${FILES_AUTOCOE_SRC}
.PHONY: cppcheck

clang_format:
#help [global] Formatting code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format -i ${FILES_SRC} ${FILES_SRC_H} ${FILES_AUTOCOE_SRC}
.PHONY: clang_format

tidy_autoCode:
#help [global] tidy static code analysis for autoCode, configuration /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy $(FILES_AUTOCOE_SRC) ${FILES_AUTOCOE_SRC_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${PATH_SOURCES}/
.PHONY:tidy_autoCode

help:
#help [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@awk ${COLOURS_AWK} '\
		/^([A-Za-z0-9][A-Za-z0-9_-]*):/ { \
			print COLOUR_HELP_FILE_TARGET $$1 COLOUR_RESET; \
			} \
		$$1 == "#help" {\
			printf("  %s%s%s", COLOUR_HELP_TAG, $$2, COLOUR_RESET); \
			temp = $$0; \
			sub(/.*\]/,"",temp); \
			print temp; \
		}\
	' ${FILES_MK}
.PHONY: help
