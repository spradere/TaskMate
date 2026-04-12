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
	-DHAL_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/tm_scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED

CFLAGS_${SRC_DIR}/sysCore/TaskMate.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED

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

GREP_LIST != cat "${.PARSEDIR}/${.PARSEFILE}" \
	| grep '^[[:space:]]*ALLOWED_PATTERN[0-9][0-9]*[[:space:]]*=' \
	| sed 's/^[[:space:]]*ALLOWED_PATTERN\([0-9]*\)[[:space:]]*=.*/\1/'

# Check #include for system critical features
_system_critical_check:
	@printf "\n%sChecking forbidden system critical includes ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.for index in ${GREP_LIST}
	@allowed="${ALLOWED_LIST${index}}"; \
	pattern="${ALLOWED_PATTERN${index}}"; \
	printf "\nChecking pattern %s...\n" "$$pattern"; \
	files="$$(grep -R -l "$$pattern" "${SRC_DIR}" 2>/dev/null || true)"; \
	for f in $$files; do \
	    test=no; \
	   for ok in $$allowed; do \
	        [ "$$f" = "$$ok" ] && test=yes; \
	    done; \
	    if [ "$$test" = "no" ]; then \
	       printf "%s[ FAIL ] Forbidden include detected in: %s%s\n" "${COLOUR_FAIL}" "$$f" "${COLOUR_RESET}"; \
	       exit 1; \
		else \
			printf "%s[  OK  ]%s %s\n" "${COLOUR_OK}" "${COLOUR_RESET}" "$$f"; \
	    fi; \
	done
.endfor
