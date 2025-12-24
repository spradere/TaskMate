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
	@printf "\n\033[1;31mRemove files\033[0m\n\n"
	rm -f ${OBJS} ${DEPS} build/TaskMate.*
	rm -f ${AUTOCODE_TARGET} build/.autoCode_stamp* build/autoCode_*

.PHONY: clean

# Make doxygen documentation
doc:
	@printf "\n\033[1;36mMake Doxygen documentation\033[0m\n\n"
	doxygen doc/Doxyfile
.PHONY: doc

# Count lines of code
cloc:
	@cloc * --exclude-dir=html --exclude-lang=D --exclude-ext=rc
.PHONY: cloc

# Check annotations
note:
	@printf "\n\033[1;33mCheck code\033[0m\n\n"
	@printf "**********************************************************\n"
	@printf "* todo / fix / hack\n"
	@printf "**********************************************************\n\n"
	@grep -r -n -i -E 'todo|fix|hack' ${TIDY_SRC}

.PHONY: note

# cppcheck
check:
	@printf "\n**********************************************************\n"
	@printf "* cppcheck \n"
	@printf "**********************************************************\n\n"
	@cppcheck -I/root/code/TaskMate/TaskMate_current/ \
		--enable=all --inconclusive --force \
		--suppress=missingIncludeSystem \
		--suppress=missingInclude \
		--check-level=exhaustive \
		${SRCS} \
		${AUTOCODE_SRCS}
.PHONY: check

# clang-format
format:
	@printf "\033[1;33mAuto formatting code, config in src/.clang-format\033[0m\n\n"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS}
.PHONY: format

# clang-tidy for autoCode
tidy_autoCode:
	@printf "\n\033[1;33mTidy autoCode static test code, config in src/.clang-tidy\033[0m\n\n"
	@clang-tidy $(AUTOCODE_SRCS) ${AUTOCODE_SRCS_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/src/
.PHONY:tidy_autoCode
