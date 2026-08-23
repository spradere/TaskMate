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

CFLAGS_${PATH_SRCS}/system/sysCore/modules.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCore/tm_scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/TaskMate.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCore/boot.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCore/tm_softwareTimeCounter.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCall/sysCall.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/services/commands/driver.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/drivers/lcd_AMC2004/lcd_AMC2004.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/drivers/rtc_ZS042/rtc_ZS042.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/mcu/atmega2560/i2c.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/mcu/atmega2560/timerSched.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/mcu/atmega2560/timerSTC.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/hal/mcu/atmega2560/usart.c = \
	-DTM_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${PATH_SRCS}/system/sysCore/gpio.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED


# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

	@awk ${COLOURS_AWK} -v PATH_SOURCES=${PATH_SRCS} -v h_check_log=${FILE_H_CHECK_LOG} \
		-f ${PATH_SCRIPTS}/header_allow.awk "${FILE_H_ALLOW_CONF}"
