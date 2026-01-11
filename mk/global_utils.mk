################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
#
################################################################################

################################################################################
# Utility
################################################################################

# Heavy sweep
clean:
#@ [global] Remove all build files.
	@printf "\n%sRemove files%s\n\n" \
		"${COLOR_CLEAN}" "${COLOR_RESET}"
	rm -f ${OBJS} ${DEPS} ${BUILD_DIR}/TaskMate.*
	rm -f ${AUTOCODE_TARGET} ${BUILD_DIR}/.autoCode_stamp* build/autoCode_*

.PHONY: clean

# Make doxygen documentation
doc:
#@ [global] Generate Doxygen documentation.
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	doxygen doc/Doxyfile
.PHONY: doc

# Count lines of code
cloc:
#@ [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@cloc * --exclude-dir=build,html,log,templates\
		--exclude-lang=D --exclude-ext=rc
.PHONY: cloc

# Check annotations
note:
#@ [global] Look for todo / fix / hack comments in code.
	@printf "\n%sLook for todo / fix / hack%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@grep -r -n -i -E 'todo|fix|hack' ${SRCS} ${SRCS_h } ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}

.PHONY: note

# cppcheck
cppcheck:
#@ [global] cppcheck static code analysis for autoCode and TaskMate.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@cppcheck -I/root/code/TaskMate/TaskMate_current/ \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${SRCS} \
		${AUTOCODE_SRCS}
.PHONY: check

# clang-format
clang_format:
#@ [global] Formating code with clang-format, config /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS}
.PHONY: format

# clang-tidy for autoCode
tidy_autoCode:
#@ [global] tidy static code analysis for autoCode, config /.clang-tidy.
	@printf "\n%sTidy autoCode static code test%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@clang-tidy $(AUTOCODE_SRCS) ${AUTOCODE_SRCS_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/src/
.PHONY:tidy_autoCode

# display targets
help:
#@ [global] List all utility targets, not the system ones.
	@printf "%sPrint all utility targets%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@sed -n \
		-e 's/^\([A-Za-z0-9][A-Za-z0-9_-]*\)[[:space:]]*:.*$$/${COLOR_HELP_TARGET}\1${COLOR_RESET}/p' \
		-e 's/^#@ \(\[[^]]*\]\)/    -> ${COLOR_HELP_TAG}\1${COLOR_RESET}/p' \
		-e 's/^#@ /    -> /p' \
		${MK_FILES}
.PHONY: help
