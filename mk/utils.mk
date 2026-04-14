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
#@ [global] Remove all build files.
	@printf "\n%sRemove files%s\n\n" \
		"${COLOUR_CLEAN}" "${COLOUR_RESET}"
	@printf "${COLOUR_CLEAN_SOFT}"
	rm -f ${OBJS} ${DEPS} ${BUILD_DIR_TARGET}/TaskMate*
	rm -f ${AUTOCODE_TARGET} ${BUILD_DIR_TARGET}/.autoCode_stamp* ${BUILD_DIR_TARGET}/autoCode_*
	@printf "${COLOUR_RESET}"
.PHONY: clean

doc:
#@ [global] Generate Doxygen documentation. Configuration file /doc/Doxyfile
	@printf "\n%sMake Doxygen documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	doxygen ${DOC_DIR}/Doxyfile
.PHONY: doc

_cloc_raw:
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D --exclude-ext=rc,md,txt > ${CLOC_RAW}
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D,make --exclude-ext=rc,c,h >> ${CLOC_RAW}
.PHONY: _cloc_raw

_cloc_data: _cloc_raw
	@awk '\
		$$1 == "C" { \
		c_blank += $$3; \
		c_comment += $$4; \
		c_code += $$5; \
		printf("Count lines of C code \n"); \
		} \
		$$1 == "C/C++" && $$2 == "Header" { \
		c_blank += $$4; \
		c_comment += $$5; \
		c_code += $$6; \
		printf("Count lines of .h code \n"); \
		} \
		/^make/ { \
		make_blank += $$3; \
		make_comment += $$4; \
		make_code += $$5; \
		} \
		/^Markdown/ { \
		doc_blank += $$3; \
		doc_code += $$5; \
		} \
		/^Text/ { \
		doc_blank += $$3; \
		doc_code += $$5; \
		} \
		END { \
		loc_total = c_blank + c_comment + c_code + make_blank + make_comment + make_code; \
		code_total = c_code + make_code; \
		make_total = make_blank + make_comment + make_code; \
		comment_total = c_comment + make_comment; \
		doc_total = doc_blank + doc_code; \
		code_pct = (code_total / loc_total) * 100; \
		comment_pct = (comment_total / loc_total) * 100; \
		doc_pct = (doc_total / loc_total) * 100; \
		make_pct = (make_total / loc_total) * 100; \
		printf("Count lines of code \n") > "${CLOC_DATA}"; \
		printf("loc_total %d\n", loc_total) >> "${CLOC_DATA}"; \
		printf("loc+doc %d\n", loc_total + doc_total) >> "${CLOC_DATA}"; \
		printf("code_pct %f\n", code_pct) >> "${CLOC_DATA}"; \
		printf("comment_pct %f\n", comment_pct) >> "${CLOC_DATA}"; \
		printf("doc_pct %f\n", doc_pct) >> "${CLOC_DATA}"; \
		printf("make_pct %f\n", make_pct) >> "${CLOC_DATA}"; \
		close("${CLOC_DATA}"); \
		}' ${CLOC_RAW}
.PHONY: _cloc_data

cloc:
#@ [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@out=$$(cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D --exclude-ext=rc,md,txt); \
	printf "%s\n" "$$out"; \
	printf "\n"; \
	printf "%s\n" "$$out" \
		| awk '/^SUM:/ { \
		blank   = $$3; \
		comment = $$4; \
		code    = $$5; \
		total   = blank + comment + code; \
		code_pct    = (code / total) * 100; \
		comment_pct = (comment / total) * 100; \
		printf("${COLOUR_WHITE_BOLD}Total lines : %d\n", total); \
		printf("\tCode    : %d (%.1f%%)\n", code, code_pct); \
		printf("\tComment : %d (%.1f%%)\n", comment, comment_pct); \
		printf("${COLOUR_RESET}"); \
		}'

	@printf "\n%sCount lines of documentation%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@out=$$(cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D,make --exclude-ext=rc,c,h); \
	printf "%s\n" "$$out"; \
	printf "\n"; \
	printf "%s\n" "$$out" \
		| awk '/^SUM:/ { \
		blank   = $$3; \
		code    = $$5; \
		total   = blank + code; \
		printf("${COLOUR_WHITE_BOLD}Total lines : %d\n", total); \
		printf("${COLOUR_RESET}"); \
		}'

.PHONY: cloc

note:
#@ [global] Look for TODO / FIX / HACK comments in code.
	@printf "\n%sLook for TODO / FIX / HACK%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@grep -r -n -i -E 'TODO|FIX|HACK' ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}

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
.PHONY: cppcheck

clang_format:
#@ [global] Formatting code with clang-format, configuration /.clang-format.
	@printf "%sAuto formatting code%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang-format -i ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS}
.PHONY: clang_format

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
