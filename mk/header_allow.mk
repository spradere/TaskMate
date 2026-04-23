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

CFLAGS_${PATH_SOURCES}/sysCore/modules.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/tm_scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/TaskMate.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/boot.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/runLevel.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/tm_softwareTimeCounter.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCall/sysCall.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/hal/arch/avr8/hal_context_impl.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SOURCES}/sysCore/gpio.c = \
	-D=HAL_SYSTEM_CRITICAL_ALLOWED

# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_FILE_TARGET_INFO}" "${COLOUR_RESET}"

	@awk ${COULOURS_AWK} -v PATH_SOURCES=${PATH_SOURCES} -v h_check_log=${H_CHECK_LOG} -f ${PATH_SCRIPTS}/header_allow.awk "${H_ALLOW_FILE}"


