################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

################################################################################
# ANSI colours
################################################################################

# TaskMate default style

COLOUR_TARGET_INFO = ${COLOUR_YELLOW_BOLD}
COLOUR_HELP_TARGET = ${COLOUR_WHITE}
COLOUR_HELP_TAG = ${COLOUR_CYAN_BOLD}
COLOUR_CLEAN = ${COLOUR_RED_BOLD}
COLOR_CLEAN_SOFT = ${COLOUR_RED}
COLOUR_BACKUP = ${COLOUR_YELLOW}
COLOUR_FAIL = ${COLOUR_RED_BOLD}
COLOUR_OK = ${COLOUR_GREEN_BOLD}

# styles
COLOUR_RESET            != printf '\033[0m'
COLOUR_BOLD             != printf '\033[1m'
COLOUR_DIM              != printf '\033[2m'
COLOUR_ITALIC           != printf '\033[3m'
COLOUR_UNDERLINE        != printf '\033[4m'
COLOUR_BLINK_SLOW       != printf '\033[5m'
COLOUR_BLINK_FAST       != printf '\033[6m'
COLOUR_REVERSE          != printf '\033[7m'
COLOUR_HIDDEN           != printf '\033[8m'
COLOUR_STRIKETHROUGH    != printf '\033[9m'

# colours
COLOUR_BLACK            != printf '\033[30m'
COLOUR_RED              != printf '\033[31m'
COLOUR_GREEN            != printf '\033[32m'
COLOUR_YELLOW           != printf '\033[33m'
COLOUR_BLUE             != printf '\033[34m'
COLOUR_MAGENTA          != printf '\033[35m'
COLOUR_CYAN             != printf '\033[36m'
COLOUR_WHITE            != printf '\033[37m'

# bold colours
COLOUR_BLACK_BOLD       != printf '\033[1;30m'
COLOUR_RED_BOLD         != printf '\033[1;31m'
COLOUR_GREEN_BOLD       != printf '\033[1;32m'
COLOUR_YELLOW_BOLD      != printf '\033[1;33m'
COLOUR_BLUE_BOLD        != printf '\033[1;34m'
COLOUR_MAGENTA_BOLD     != printf '\033[1;35m'
COLOUR_CYAN_BOLD        != printf '\033[1;36m'
COLOUR_WHITE_BOLD       != printf '\033[1;37m'
