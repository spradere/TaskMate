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
# Allowed source access to system-critical headers
################################################################################

CFLAGS_${PATH_SRCS}/system/sysCore/sys_modules.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCore/sys_scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/TaskMate.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED \
	-include ${FILE_HAL_STRING_MACRO}
	
CFLAGS_${PATH_SRCS}/system/sysCore/sys_softwareTimeCounter.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCall/sc_driver.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCall/sc_threads.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCall/sc_gpio.c = \
	-DSC_GPIO_SYSTEM_ALLOWED

CFLAGS_${PATH_SRCS}/system/services/system.c = \
	-DSC_GPIO_SYSTEM_ALLOWED

# Add string macro acces
PATHS_TM_STRING_ALLOWED= \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/sysCall/*.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/services/*.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/system/services/commands/*.c} \
	${FILES_COMPILE_SRC:M${PATH_SRCS}/tmLibc/*.c}
	
.for src in ${PATHS_TM_STRING_ALLOWED}
CFLAGS_${src} += -include ${FILE_HAL_STRING_MACRO}
.endfor

# Check includes for system-critical features
.PHONY: _system_critical_check
_system_critical_check:
	@printf "%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@awk ${COLOURS_AWK} -v PATH_SOURCES=${PATH_SRCS} -v h_check_log=${FILE_H_CHECK_LOG} \
		-f ${SCRIPT_HEADER_ALLOW} "${CONF_SYSTEM_HEADER_ALLOW}"

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
		status=$$?; cat "${FILE_ARCH_CHECK_LOG}"; echo ">>> satus : " $$status; \
	fi
