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

_cloc_data:
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D --exclude-ext=rc,md,txt > ${CLOC_RAW}
	@printf "\n" >> ${CLOC_RAW}
	@cloc * --exclude-dir=${BUILD_DIR},${LOG_DIR} --exclude-lang=D,make --exclude-ext=rc,c,h >> ${CLOC_RAW}

	@awk '\
		$$1 == "C" { \
		c_blank += $$3; \
		c_comment += $$4; \
		c_code += $$5; \
		} \
		$$1 == "C/C++" && $$2 == "Header" { \
		c_blank += $$4; \
		c_comment += $$5; \
		c_code += $$6; \
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
		printf("make_total %d\n", make_total) >> "${CLOC_DATA}"; \
		printf("loc+doc %d\n", loc_total + doc_total) >> "${CLOC_DATA}"; \
		printf("code_pct %0.1f\n", code_pct) >> "${CLOC_DATA}"; \
		printf("comment_pct %0.1f\n", comment_pct) >> "${CLOC_DATA}"; \
		printf("doc_pct %0.1f\n", doc_pct) >> "${CLOC_DATA}"; \
		printf("make_pct %0.1f\n", make_pct) >> "${CLOC_DATA}"; \
		close("${CLOC_DATA}"); \
		}' ${CLOC_RAW}
.PHONY: _cloc_data

cloc: _cloc_data
#@ [global] Count lines of codes.
	@printf "\n%sCount lines of codes%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@cat ${CLOC_RAW}
	@printf "${COLOUR_WHITE_BOLD}\nTotal loc + %% :\n"

	@awk '\
		NR > 1 { \
		vars[$$1] = $$2; \
		} \
		END { \
		split("\
		loc_total \
		loc+doc \
		code_pct \
		comment_pct \
		make_total \
		make_pct \
		doc_pct" \
		, order, " "); \
		for (i = 1; i <= length(order); i++) \
			{ \
			k = order[i]; \
			if (k in vars) \
				{ \
				printf("\t%-12s : %s\n", k, vars[k]); \
				} \
			} \
		}' ${CLOC_DATA}
	@printf "${COLOUR_RESET}"
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
