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

# clang-tidy
#TIDY_SRC = ${AUTOCODE_SRC}
#TIDY_SRC += ${SRCS}

#tidy:
#	@printf "\n\033[1;33mTidy static test code, config in src/.clang-tidy\033[0m\n\n"
#	@clang-tidy $(TIDY_SRC) -- \
#		-I/root/code/TaskMate/TaskMate_current/src \
#		-I/root/code/TaskMate/TaskMate_current/ \
#		-isystem /usr/local/avr/include -isystem /usr/local/lib/gcc/avr/14.1.0 \
#		-D__AVR__=6 -D__AVR_ATmega2560__=1 \
#		-DF_CPU=${F_CPU}
#.PHONY: tidy

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
		${TIDY_SRC}

.PHONY: check

# clang-format
format:
	@printf "\033[1;33mAuto formatting code, config in src/.clang-format\033[0m\n\n"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRC}
.PHONY: format
