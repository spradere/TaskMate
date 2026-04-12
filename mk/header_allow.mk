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
# Allowed source access system critical headers
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

ALLOWED_PATTERN1 = hal/public/hal_context.h
ALLOWED_LIST1 = \
	${SRC_DIR}/sysCore/modules.c \
	${SRC_DIR}/sysCore/tm_scheduler.c

ALLOWED_PATTERN2 = hal/public/hal_stack.h
ALLOWED_LIST2 = \
	${SRC_DIR}/sysCore/tm_scheduler.c

ALLOWED_PATTERN3 = hal/public/hal_int.h
ALLOWED_LIST3 = \
	${SRC_DIR}/hal/public/panic.h \
	${SRC_DIR}/sysCore/tm_scheduler.c

ALLOWED_PATTERN4 = sysCore/runLevel.h
ALLOWED_LIST4 = \
	${SRC_DIR}/sysCore/runLevel.c \
	${SRC_DIR}/autoCode/autoCode.c \
	${SRC_DIR}/autoCode/autoCode.h \
	${SRC_DIR}/sysCore/boot.c

ALLOWED_PATTERN5 = interfaces/modules_define.h
ALLOWED_LIST5 = \
	${SRC_DIR}/sysCore/modules.h \
	${SRC_DIR}/autoCode/autoCode.c \
	${SRC_DIR}/autoCode/autoCode.h \
	${SRC_DIR}/hal/arch/avr8/hal_context_impl.c

ALLOWED_PATTERN6 = sysCore/modules_list.h
ALLOWED_LIST6 = \
	${SRC_DIR}/autoCode/autoCode.c \
	${SRC_DIR}/sysCore/modules.c

ALLOWED_PATTERN7 = sysCore/hal_init.h
ALLOWED_LIST7 = \
	${SRC_DIR}/autoCode/autoCode.c \
	${SRC_DIR}/sysCore/boot.c \
	${SRC_DIR}/sysCore/gpio.c

GREP_LIST != cat "${.PARSEDIR}/${.PARSEFILE}" \
	| grep '^[[:space:]]*ALLOWED_PATTERN[0-9][0-9]*[[:space:]]*=' \
	| sed 's/^[[:space:]]*ALLOWED_PATTERN\([0-9]*\)[[:space:]]*=.*/\1/'

# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "Header check patterns with grep\n" > ${H_CHECK_LOG}
.for index in ${GREP_LIST}
	@allowed="${ALLOWED_LIST${index}}"; \
	pattern="${ALLOWED_PATTERN${index}}"; \
	printf "Checking pattern %s...\n" "$$pattern"; \
	printf "\nChecking pattern %s...\n" "$$pattern" >> ${H_CHECK_LOG} ; \
	files="$$(grep -R -l "$$pattern" "${SRC_DIR}" 2>/dev/null || true)"; \
	for f in $$files; do \
	    test=no; \
	   for ok in $$allowed; do \
	        [ "$$f" = "$$ok" ] && test=yes; \
	    done; \
	    if [ "$$test" = "no" ]; then \
	       printf "%s[ FAIL ] Forbidden include detected in: %s%s\n" "${COLOUR_FAIL}" "$$f" "${COLOUR_RESET}"; \
	       printf "[ FAIL ] Forbidden include detected in : %s\n" "$$f" >> ${H_CHECK_LOG}; \
	       exit 1; \
		else \
			printf "[  OK  ]%s\n" "$$f" >> ${H_CHECK_LOG}; \
	    fi; \
	done
.endfor
