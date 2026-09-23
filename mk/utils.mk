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

.PHONY: doc
doc:
#help [global] Generate Doxygen documentation. Configuration file /doc/Doxyfile
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	doxygen ${PATH_DOCS}/Doxyfile

.PHONY: _cloc_data
_cloc_data:
	# Use :T because cloc does not handle a full path with the --exclude-dir option
	@cloc * --exclude-dir=${PATH_BUILDS:T},${PATH_LEGACY} --exclude-lang=D --exclude-ext=rc,md,txt > ${FILE_CLOCRAW}
	@printf "\n" >> ${FILE_CLOCRAW}
	@cloc * --exclude-dir=${PATH_BUILDS:T} --exclude-lang=D,make --exclude-ext=rc,c,h,awk,sh >> ${FILE_CLOCRAW}
	@awk -v file="${FILE_CLOCDATA}" -f ${SCRIPT_CLOC_DATA} ${FILE_CLOCRAW}

.PHONY: cloc
cloc: _cloc_data
#help [global] Count lines of code.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@cat ${FILE_CLOCRAW}
	@printf "${COLOUR_WHITE_BOLD}\nTotal loc and ratio :\n"
	@awk -f ${SCRIPT_CLOC_SHOW} "${FILE_CLOCDATA}"
	@printf "${COLOUR_RESET}"

.PHONY: note
note:
#help [global] Look for TODO / FIX / HACK comments in code.
	@printf "\n%sLook for TODO / FIX / HACK%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@grep -r -n -i -E 'TODO|FIX|HACK|enum' ${FILES_TM_SRC} ${FILES_TM_SRC_H} \
		${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}

.PHONY: cppcheck
cppcheck:
#help [global] cppcheck static code analysis for autoCode and TaskMate.
	@printf "\n%scppcheck static analysis%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cppcheck -I${PATH_SRCS} \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${FILES_TM_SRC} \
		${FILES_AUTOCODE_SRC}

.PHONY: format
format:
#help [global] Format code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format19 -i ${FILES_TM_SRC} ${FILES_TM_SRC_H} ${FILES_AUTOCODE_SRC}

.PHONY: tidy_autoCode
tidy_autoCode:
#help [global] tidy static code analysis for autoCode, configuration /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy19 ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H} -- \
		${CFLAGS_AUTOCODE}

.PHONY: help
help:
#help [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@awk ${COLOURS_AWK} -f ${SCRIPT_MAKE_HELP} ${FILES_MK}
