################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
#
################################################################################

################################################################################
# ANSI colors
################################################################################

# TaskMate default style

COLOR_TARGET_INFO = ${COLOR_YELLOW_BOLD}
COLOR_HELP_TARGET = ${COLOR_WHITE}
COLOR_HELP_TAG = ${COLOR_CYAN_BOLD}
COLOR_CLEAN = ${COLOR_RED_BOLD}
COLOR_BACKUP = ${COLOR_YELLOW}

# styles
COLOR_RESET            != printf '\033[0m'
COLOR_BOLD             != printf '\033[1m'
COLOR_DIM              != printf '\033[2m'
COLOR_ITALIC           != printf '\033[3m'
COLOR_UNDERLINE        != printf '\033[4m'
COLOR_BLINK_SLOW       != printf '\033[5m'
COLOR_BLINK_FAST       != printf '\033[6m'
COLOR_REVERSE          != printf '\033[7m'
COLOR_HIDDEN           != printf '\033[8m'
COLOR_STRIKETHROUGH    != printf '\033[9m'

# colors
COLOR_BLACK            != printf '\033[30m'
COLOR_RED              != printf '\033[31m'
COLOR_GREEN            != printf '\033[32m'
COLOR_YELLOW           != printf '\033[33m'
COLOR_BLUE             != printf '\033[34m'
COLOR_MAGENTA          != printf '\033[35m'
COLOR_CYAN             != printf '\033[36m'
COLOR_WHITE            != printf '\033[37m'

# bold colors
COLOR_BLACK_BOLD       != printf '\033[1;30m'
COLOR_RED_BOLD         != printf '\033[1;31m'
COLOR_GREEN_BOLD       != printf '\033[1;32m'
COLOR_YELLOW_BOLD      != printf '\033[1;33m'
COLOR_BLUE_BOLD        != printf '\033[1;34m'
COLOR_MAGENTA_BOLD     != printf '\033[1;35m'
COLOR_CYAN_BOLD        != printf '\033[1;36m'
COLOR_WHITE_BOLD       != printf '\033[1;37m'
