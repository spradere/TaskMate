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

# board settings
F_CPU = 16000000UL

CFLAGS += -DF_CPU=${F_CPU}

PROGRAMMER = avrispmkII
PORT = /dev/ttyU0
