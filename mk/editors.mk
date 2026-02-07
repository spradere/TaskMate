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
# editors
################################################################################

tags:
#@ [global] Generate tags for Vim
	@ctags -f .tags ${SRCS}
	@ctags -f .tags -a ${SRCS_H}
	@ctags -f .tags -a ${AUTOCODE_SRCS}
	@ctags -f .tags -a ${AUTOCODE_SRCS_H}
.PHONY: tags

vim_all: tags
	vim ${SRCS} ${SRCS_H} ${MK_FILES}
.PHONY: vim_all

geany_all: tags
	geany ${SRCS} ${SRCS_H} ${MK_FILES}
.PHONY: geany_all

