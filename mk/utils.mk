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

.PHONY: clean
clean:
#help [global] Remove build files.
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
		
	find "${PATH_BUILD_TARGET}" -type f \( -name "*.o" -o -name "*.d" \) -delete
	find "${PATH_BUILD_TARGET}" -maxdepth 1 -type f -name "TaskMate*" -delete
	find "${FILE_AUTOCODE_TARGET}" -type f -delete
	find "${PATH_BUILD_TARGET}" -maxdepth 1 -type f -name "*autoCode*" -delete; \
	find "${PATH_BUILDS}" -maxdepth 1 -type f -name "autoCode" -delete; \

.PHONY: clean_hard
clean_hard:
#help [global] Remove all build files.
	@${SCRIPT_CHECK_PATH_FILE} -d "${PATH_BUILD_TARGET}" "${PATH_BUILDS}"
	@printf "\n%sRemove all files: ${PATH_BUILDS}/*\nRemove all files and subdir: ${PATH_BUILD_TARGET}/* %s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
		
	@printf "${COLOUR_CLEAN_SOFT}"
	find "${PATH_BUILDS}" -maxdepth 1 -type f -delete
	find "${PATH_BUILD_TARGET}" -mindepth 1 -depth -delete
	@printf "${COLOUR_RESET}"
	
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
	@grep -r -n -i -E 'TODO|FIX|HACK|enum' ${FILES_COMPILE_SRC} ${FILES_SRC_H} \
		${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}

.PHONY: cppcheck
cppcheck:
#help [global] cppcheck static code analysis for autoCode and TaskMate.
	@printf "\n%scppcheck static analysis%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cppcheck -I/root/code/TaskMate/TaskMate_current/ \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${FILES_COMPILE_SRC} \
		${FILES_AUTOCODE_SRC}

.PHONY: format
format:
#help [global] Format code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format19 -i ${FILES_COMPILE_SRC} ${FILES_SRC_H} ${FILES_AUTOCODE_SRC}

.PHONY: tidy_autoCode
tidy_autoCode:
#help [global] tidy static code analysis for autoCode, configuration /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy19 $(FILES_AUTOCODE_SRC) ${FILES_AUTOCODE_SRC_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${PATH_SRCS}/

.PHONY: help
help:
#help [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@awk ${COLOURS_AWK} -f ${SCRIPT_MAKE_HELP} ${FILES_MK}
