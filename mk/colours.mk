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
# ANSI colours
################################################################################

# TaskMate default style
COLOUR_TARGET_INFO = ${COLOUR_YELLOW_BOLD}
AWK_COLOURS += -v COLOUR_TARGET_INFO="${COLOUR_TARGET_INFO}"

COLOUR_HELP_TARGET = ${COLOUR_WHITE}
AWK_COLOURS += -v COLOUR_HELP_TARGET="${COLOUR_HELP_TARGET}"

COLOUR_HELP_TAG = ${COLOUR_CYAN_BOLD}
AWK_COLOURS += -v COLOUR_HELP_TAG="${COLOUR_HELP_TAG}"

COLOUR_CLEAN = ${COLOUR_RED_BOLD}
AWK_COLOURS += -v COLOUR_CLEAN="${COLOUR_CLEAN}"

COLOUR_CLEAN_SOFT = ${COLOUR_RED}
AWK_COLOURS += -v COLOUR_CLEAN_SOFT="${COLOUR_CLEAN_SOFT}"

COLOUR_BACKUP = ${COLOUR_YELLOW}
AWK_COLOURS += -v COLOUR_BACKUP="${COLOUR_BACKUP}"

COLOUR_FAIL = ${COLOUR_RED_BOLD}
AWK_COLOURS += -v COLOUR_FAIL="${COLOUR_FAIL}"

COLOUR_OK = ${COLOUR_GREEN_BOLD}
AWK_COLOURS += -v COLOUR_OK="${COLOUR_OK}"


# Styles
COLOUR_RESET            != printf '\033[0m'
AWK_COLOURS += -v COLOUR_RESET="${COLOUR_RESET}"

COLOUR_BOLD             != printf '\033[1m'
AWK_COLOURS += -v COLOUR_BOLD="${COLOUR_BOLD}"

COLOUR_DIM              != printf '\033[2m'
AWK_COLOURS += -v COLOUR_DIM="${COLOUR_DIM}"

COLOUR_ITALIC           != printf '\033[3m'
AWK_COLOURS += -v COLOUR_ITALIC="${COLOUR_ITALIC}"

COLOUR_UNDERLINE        != printf '\033[4m'
AWK_COLOURS += -v COLOUR_UNDERLINE="${COLOUR_UNDERLINE}"

COLOUR_BLINK_SLOW       != printf '\033[5m'
AWK_COLOURS += -v COLOUR_BLINK_SLOW="${COLOUR_BLINK_SLOW}"

COLOUR_BLINK_FAST       != printf '\033[6m'
AWK_COLOURS += -v COLOUR_BLINK_FAST="${COLOUR_BLINK_FAST}"

COLOUR_REVERSE          != printf '\033[7m'
AWK_COLOURS += -v COLOUR_REVERSE="${COLOUR_REVERSE}"

COLOUR_HIDDEN           != printf '\033[8m'
AWK_COLOURS += -v COLOUR_HIDDEN="${COLOUR_HIDDEN}"

COLOUR_STRIKETHROUGH    != printf '\033[9m'
AWK_COLOURS += -v COLOUR_STRIKETHROUGH="${COLOUR_STRIKETHROUGH}"


# Colours
COLOUR_BLACK            != printf '\033[30m'
AWK_COLOURS += -v COLOUR_BLACK="${COLOUR_BLACK}"

COLOUR_RED              != printf '\033[31m'
AWK_COLOURS += -v COLOUR_RED="${COLOUR_RED}"

COLOUR_GREEN            != printf '\033[32m'
AWK_COLOURS += -v COLOUR_GREEN="${COLOUR_GREEN}"

COLOUR_YELLOW           != printf '\033[33m'
AWK_COLOURS += -v COLOUR_YELLOW="${COLOUR_YELLOW}"

COLOUR_BLUE             != printf '\033[34m'
AWK_COLOURS += -v COLOUR_BLUE="${COLOUR_BLUE}"

COLOUR_MAGENTA          != printf '\033[35m'
AWK_COLOURS += -v COLOUR_MAGENTA="${COLOUR_MAGENTA}"

COLOUR_CYAN             != printf '\033[36m'
AWK_COLOURS += -v COLOUR_CYAN="${COLOUR_CYAN}"

COLOUR_WHITE            != printf '\033[37m'
AWK_COLOURS += -v COLOUR_WHITE="${COLOUR_WHITE}"


# Bold colours
COLOUR_BLACK_BOLD       != printf '\033[1;30m'
AWK_COLOURS += -v COLOUR_BLACK_BOLD="${COLOUR_BLACK_BOLD}"

COLOUR_RED_BOLD         != printf '\033[1;31m'
AWK_COLOURS += -v COLOUR_RED_BOLD="${COLOUR_RED_BOLD}"

COLOUR_GREEN_BOLD       != printf '\033[1;32m'
AWK_COLOURS += -v COLOUR_GREEN_BOLD="${COLOUR_GREEN_BOLD}"

COLOUR_YELLOW_BOLD      != printf '\033[1;33m'
AWK_COLOURS += -v COLOUR_YELLOW_BOLD="${COLOUR_YELLOW_BOLD}"

COLOUR_BLUE_BOLD        != printf '\033[1;34m'
AWK_COLOURS += -v COLOUR_BLUE_BOLD="${COLOUR_BLUE_BOLD}"

COLOUR_MAGENTA_BOLD     != printf '\033[1;35m'
AWK_COLOURS += -v COLOUR_MAGENTA_BOLD="${COLOUR_MAGENTA_BOLD}"

COLOUR_CYAN_BOLD        != printf '\033[1;36m'
AWK_COLOURS += -v COLOUR_CYAN_BOLD="${COLOUR_CYAN_BOLD}"

COLOUR_WHITE_BOLD       != printf '\033[1;37m'
AWK_COLOURS += -v COLOUR_WHITE_BOLD="${COLOUR_WHITE_BOLD}"

