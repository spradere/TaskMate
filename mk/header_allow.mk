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
# Allowed source access to system critical headers
################################################################################

CFLAGS_${SRC_DIR}/sysCore/modules.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/tm_scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/TaskMate.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/boot.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/runLevel.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/tm_softwareTimeCounter.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCall/sysCall.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/hal/arch/avr8/hal_context_impl.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/gpio.c = \
	-D=HAL_SYSTEM_CRITICAL_ALLOWED

# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@awk ${AWK_COLOURS} -v src_dir=${SRC_DIR} -v h_check_log=${H_CHECK_LOG} -f ${DIR_SCRIPTS}/header_allow.awk "${H_ALLOW_FILE}"


