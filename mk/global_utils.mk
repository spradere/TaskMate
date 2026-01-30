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
# Utility
################################################################################

tags:
#@ [global] Generate tags for Vim
	@ctags ${SRCS}
	@ctags -a ${SRCS_H}
	@ctags -a ${AUTOCODE_SRCS}
	@ctags -a ${AUTOCODE_SRCS_H}
.PHONY: tags

clean:
#@ [global] Remove all build files.
	@printf "\n%sRemove files%s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
	@printf "${COLOUR_CLEAN_SOFT}"
	rm -f ${OBJS} ${DEPS} ${BUILD_DIR}/TaskMate*
	rm -f ${AUTOCODE_TARGET} ${BUILD_DIR}/.autoCode_stamp* ${BUILD_DIR}/autoCode_*
	@printf "${COLOUR_RESET}"
.PHONY: clean

doc:
#@ [global] Generate Doxygen documentation. Configuration file /doc/Doxyfile
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	doxygen ${DOC_DIR}/Doxyfile
.PHONY: doc

cloc:
#@ [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR},html \
		--exclude-lang=D --exclude-ext=rc,md,txt
	@printf "\n%sCount lines of documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR},html \
		--exclude-lang=D,make --exclude-ext=rc,c,h
.PHONY: cloc

note:
#@ [global] Look for TODO / FIX / HACK comments in code.
	@printf "\n%sLook for TODO / FIX / HACK%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@grep -r -n -i -E 'TODO|FIX|HACK' ${SRCS} ${SRCS_h } ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}

.PHONY: note

cppcheck:
#@ [global] cppcheck static code analysis for autoCode and TaskMate.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cppcheck -I/root/code/TaskMate/TaskMate_current/ \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${SRCS} \
		${AUTOCODE_SRCS}
.PHONY: check

clang_format:
#@ [global] Formatting code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS}
.PHONY: format

tidy_autoCode:
#@ [global] tidy static code analysis for autoCode, configuration /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy $(AUTOCODE_SRCS) ${AUTOCODE_SRCS_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${SRC_DIR}/
.PHONY:tidy_autoCode

help:
#@ [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@sed -n \
		-e 's/^\([A-Za-z0-9][A-Za-z0-9_-]*\)[[:space:]]*:.*$$/${COLOUR_HELP_TARGET}\1${COLOUR_RESET}/p' \
		-e 's/^#@ \(\[[^]]*\]\)/    -> ${COLOUR_HELP_TAG}\1${COLOUR_RESET}/p' \
		-e 's/^#@ /    -> /p' \
		${MK_FILES}
.PHONY: help
