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
# Hardware target selection and test
################################################################################

# Default target
#VAL_TARGET ?= test1
#.if empty(VAL_TARGET)
#.error No target selected, use make VAL_TARGET=<target name>
#.endif

FILE_TARGET_MK = ${PATH_SRCS}/user/target/${VAL_TARGET}/target.mk

.if (!empty(VAL_TARGET)) && (!exists(${FILE_TARGET_MK}))
.error Target makefile not found >>>${VAL_TARGET}/target.mk<<<
.include "${FILE_TARGET_MK}"

# Check build hardware stack
.PHONY: _hardware_target_check
_hardware_target_check: ${CONF_HARDWARE_TARGETS} ${SCRIPT_HARDWARE_TARGET}
	@printf "%sChecking hardware target ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@awk -v hardware_target="${VAL_HW_STACK}" \
		-f "${SCRIPT_HARDWARE_TARGET}" "${CONF_HARDWARE_TARGETS}"

.endif
