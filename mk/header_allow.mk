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
# Headers source access
################################################################################

# Add string macro access to high level code
PATHS_TM_STRING_ALLOWED= \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCall/*.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/services/*.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/tmLibc/*.c}

PATHS_TM_STRING_ALLOWED := ${PATHS_TM_STRING_ALLOWED:O}
.if ${PATHS_TM_STRING_ALLOWED:[#]} != ${PATHS_TM_STRING_ALLOWED:u:[#]}
.error Duplicate path in PATHS_TM_STRING_ALLOWED: ${PATHS_TM_STRING_ALLOWED}
.endif
	
.for src in ${PATHS_TM_STRING_ALLOWED}
CFLAGS_${src} += -include ${FILE_HAL_STRING_MACRO}
.endfor

# Add architecture types only where a complete context or stack word is required
FILES_HAL_ARCH_TYPES_ALLOWED = \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/boot.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCall/sc_driver.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCall/sc_gpio.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCall/sc_threads.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCore/sys_modules.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCore/sys_scheduler.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCore/sys_softwareTimeCounter.c}

FILES_HAL_ARCH_TYPES_ALLOWED := ${FILES_HAL_ARCH_TYPES_ALLOWED:O}
.if ${FILES_HAL_ARCH_TYPES_ALLOWED:[#]} != ${FILES_HAL_ARCH_TYPES_ALLOWED:u:[#]}
.error Duplicate path in FILES_HAL_ARCH_TYPES_ALLOWED: ${FILES_HAL_ARCH_TYPES_ALLOWED}
.endif

.for file in ${FILES_HAL_ARCH_TYPES_ALLOWED}
CFLAGS_${file} += -include ${FILE_HAL_ARCH_TYPES}
.endfor

# Check direct includes against the architecture matrix
.PHONY: _architecture_include_check
_architecture_include_check: ${FILE_ARCH_VALID_MATRIX} ${SCRIPT_ARCH_INCLUDE}
	@printf "%sChecking architecture direct includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@if awk -v matrix_file="${FILE_ARCH_VALID_MATRIX}" -v path_sources="${PATH_SRCS}" \
		-f "${SCRIPT_ARCH_INCLUDE}" "${FILE_ARCH_VALID_MATRIX}" \
		${FILES_COMPILE_SRC} ${FILES_SRC_H} > "${FILE_ARCH_CHECK_LOG}"; then \
		cat "${FILE_ARCH_CHECK_LOG}"; \
	else \
		status=$$?; cat "${FILE_ARCH_CHECK_LOG}"; \
		printf ">>> status : %s\n" "$$status"; exit "$$status"; \
	fi
