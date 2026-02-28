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
# Editors
################################################################################

# Generate tags
${TAGS_STAMP}: ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@ctags -f ${TAGS} ${SRCS}
	@ctags -f ${TAGS} -a ${SRCS_H}
	@ctags -f ${TAGS} -a ${AUTOCODE_SRCS}
	@ctags -f ${TAGS} -a ${AUTOCODE_SRCS_H}
	@touch ${TAGS_STAMP}

vim_all: ${TAGS_STAMP}
#@ [global] open Vim with all TaskMate sources files .c .h (no autoCode)
	vim ${SRCS} ${SRCS_H}
.PHONY: vim_all

vim_mk: ${TAGS_STAMP}
#@ [global] open Vim with all Makefiles .mk
	vim Makefile ${MK_FILES}
.PHONY: vim_mk

vim_doc:
#@ [global] open Vim with all documentation files .md .txt
	vim ${DOCS}
.PHONY: vim_doc

vim_autoCode: ${TAGS_STAMP}
#@ [global] open Vim with all autoCode sources files .c .h
	vim ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
.PHONY: vim_autoCode

