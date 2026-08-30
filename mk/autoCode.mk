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
# autoCode files and rules
################################################################################

# autoCode files
FILE_AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config
FILE_AUTOCODE_STAMP = ${PATH_BUILD_TARGET}/.autoCode_stamp
FILE_AUTOCODE_LOG_DATED = ${FILE_AUTOCODE_LOG}_${VAL_DATE_TIME}

FILES_PARSE_TAG =  \
	${PATH_SRCS}/system/sysCall/error.c \
	${PATH_SRCS}/interfaces/error_catalog.h \
	${PATH_SRCS}/interfaces/modules_define.h \
	${PATH_SRCS}/system/sysCore/modules.c \
	${PATH_SRCS}/system/sysCore/modules_list.h \
	${PATH_SRCS}/system/sysCore/hal_init.h \
	${PATH_SRCS}/hal/public/define.h \
	${PATH_SRCS}/interfaces/gpio_signals.h 	
		
FILE_INITRC_LIST = ${PATH_BUILD_TARGET}/files_initrc
FILE_PARSE_TAG_LIST = ${PATH_BUILD_TARGET}/files_to_parse
FILE_HALINIT_LIST = ${PATH_BUILD_TARGET}/files_halinit
FILE_HALDEFINE_LIST = ${PATH_BUILD_TARGET}/files_haldefine
FILE_ERROR_LIST = ${PATH_BUILD_TARGET}/files_error
FILE_ERROR_CAT = ${PATH_BUILD_TARGET}/errors.err

FILE_INITRC_DEPS = ${PATH_BUILD_TARGET}/files_initrc.deps
FILE_PARSE_TAG_DEPS = ${PATH_BUILD_TARGET}/files_to_parse.deps
FILE_HALINIT_DEPS = ${PATH_BUILD_TARGET}/files_halinit.deps
FILE_HALDEFINE_DEPS = ${PATH_BUILD_TARGET}/files_haldefine.deps
FILE_ERROR_DEPS = ${PATH_BUILD_TARGET}/files_error.deps

# autoCode launch and required files
${FILE_AUTOCODE_STAMP}: ${FILE_AUTOCODE_TARGET} ${FILE_INITRC_LIST} ${FILE_ERROR_LIST} \
						${FILE_PARSE_TAG_LIST} ${FILE_HALINIT_LIST} ${FILE_HALDEFINE_LIST}

	@printf "\n%sautoCode, related files have changed -> run autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@rm -f ${FILE_AUTOCODE_LOG}*
.endif

	# write autoCode options
	@cat ${FILES_ERROR} > "${FILE_ERROR_CAT}"
	@printf "# autoCode options\n" > "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--errors ${FILE_ERROR_CAT}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--initrc ${FILE_INITRC_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--parsetag ${FILE_PARSE_TAG_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--halinit ${FILE_HALINIT_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--haldefine ${FILE_HALDEFINE_LIST}" >> "${FILE_AUTOCODE_CONFIG}"	
	@printf "%s\n" "--gpio_signals ${FILE_GPIO_SIGNALS}" >> "${FILE_AUTOCODE_CONFIG}"
		
	# launch autoCode
	./${FILE_AUTOCODE_TARGET} ${FILE_AUTOCODE_CONFIG} > "${FILE_AUTOCODE_LOG_DATED}"
	@touch ${FILE_AUTOCODE_STAMP}

	# proceed log
	@awk ${COLOURS_AWK} -v log_file="${FILE_AUTOCODE_LOG_DATED}" \
		-f ${PATH_SCRIPTS}/autocode_log.awk "${FILE_AUTOCODE_LOG_DATED}"

# Special rule for autoCode with clang, not arch specialized compiler
CFLAGS_AUTOCODE = -I${PATH_SRCS}/
CFLAGS_AUTOCODE += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${FILE_AUTOCODE_TARGET}: ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${CFLAGS_AUTOCODE} ${FILES_AUTOCODE_SRC} -o ${FILE_AUTOCODE_TARGET}

# dependency generation
${FILE_INITRC_DEPS}: .PHONY
	@${PATH_SCRIPTS}/compare_replace.sh \
		"${FILE_INITRC_DEPS}" "${FILE_INITRC_LIST}"

${FILE_PARSE_TAG_DEPS}: .PHONY
	@${PATH_SCRIPTS}/compare_replace.sh \
		"${FILE_PARSE_TAG_DEPS}" "${FILE_PARSE_TAG_LIST}"		
	
${FILE_HALINIT_DEPS}: .PHONY
	@${PATH_SCRIPTS}/compare_replace.sh \
		"${FILE_HALINIT_DEPS}" "${FILE_HALINIT_LIST}"

${FILE_HALDEFINE_DEPS}: .PHONY
	@${PATH_SCRIPTS}/compare_replace.sh \
		"${FILE_HALDEFINE_DEPS}" "${FILE_HALDEFINE_LIST}"
		
${FILE_ERROR_DEPS}: .PHONY
	@${PATH_SCRIPTS}/compare_replace.sh \
		"${FILE_ERROR_DEPS}" "${FILE_ERROR_LIST}"
				
# Files list for autoCode
${FILE_ERROR_LIST}: ${FILES_ERROR} ${FILE_ERROR_DEPS}
	@printf "" > ${FILE_ERROR_LIST}
.for file in ${FILES_ERROR}
	@printf "%s\n" ${file} >> ${FILE_ERROR_LIST}
.endfor

${FILE_INITRC_LIST}: ${FILES_INITRC} ${FILE_INITRC_DEPS}
	@printf "" > ${FILE_INITRC_LIST}
.for file in ${FILES_INITRC}
	@printf "%s\n" ${file} >> ${FILE_INITRC_LIST}
.endfor

${FILE_PARSE_TAG_LIST}: ${FILES_PARSE_TAG} ${FILE_PARSE_TAG_DEPS}
	@printf "" > ${FILE_PARSE_TAG_LIST}
.for file in ${FILES_PARSE_TAG}
	@printf "%s\n" ${file} >> ${FILE_PARSE_TAG_LIST}
.endfor

${FILE_HALINIT_LIST}: ${FILES_HALINIT} ${FILE_HALINIT_DEPS}
	@printf "" > ${FILE_HALINIT_LIST}
.for file in ${FILES_HALINIT}
	@printf "%s\n" ${file} >> ${FILE_HALINIT_LIST}
.endfor

${FILE_HALDEFINE_LIST}: ${FILES_HALDEFINE} ${FILE_HALDEFINE_DEPS}
	@printf "" > ${FILE_HALDEFINE_LIST}
.for file in ${FILES_HALDEFINE}
	@printf "%s\n" ${file} >> ${FILE_HALDEFINE_LIST}
.endfor

# Run autoCode alone
autoCode_alone: ${FILE_AUTOCODE_TARGET}
#help [global] Run autoCode alone.
	@printf "\n%sForce running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f "${FILE_AUTOCODE_STAMP}"
	@${MAKE} ${FILE_AUTOCODE_STAMP}
	@ls -t ${FILE_AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat
.PHONY: autoCode_alone
