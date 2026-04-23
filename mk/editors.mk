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
# Editors
################################################################################

# Generate tags
${FILE_TAGS_STAMP}: ${SRCS} ${SRCS_H} ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
	@ctags -f ${FILE_TAGS} ${SRCS}
	@ctags -f ${FILE_TAGS} -a ${SRCS_H}
	@ctags -f ${FILE_TAGS} -a ${AUTOCODE_SRCS}
	@ctags -f ${FILE_TAGS} -a ${AUTOCODE_SRCS_H}
	@touch ${FILE_TAGS_STAMP}

vim_all: ${FILE_TAGS_STAMP}
#help [global] open Vim with all TaskMate sources files .c .h (no autoCode)
	vim ${SRCS} ${SRCS_H}
.PHONY: vim_all

vim_mk: ${FILE_TAGS_STAMP}
#help [global] open Vim with all Makefiles .mk
	vim ${MK_FILES}
.PHONY: vim_mk

geany_doc:
#help [global] open Vim with all documentation files .md .txt
	geany ${DOCS}
.PHONY: geany_doc

vim_autoCode: ${FILE_TAGS_STAMP}
#help [global] open Vim with all autoCode sources files .c .h
	vim ${AUTOCODE_SRCS} ${AUTOCODE_SRCS_H}
.PHONY: vim_autoCode

geany_mk:
#help [global] open Geany with all Makefiles .mk
	geany ${MK_FILES}
.PHONY: geany_mk

geany_all:
#help [global] open Geany with all TaskMate sources files .c .h (no autoCode)
	geany ${SRCS} ${SRCS_H}
.PHONY: geany_all
