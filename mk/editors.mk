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
${FILE_TAGS_STAMP}: ${FILES_SRC} ${FILES_SRC_H} ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}
	@ctags -f ${FILE_TAGS} ${FILES_SRC}
	@ctags -f ${FILE_TAGS} -a ${FILES_SRC_H}
	@ctags -f ${FILE_TAGS} -a ${FILES_AUTOCODE_SRC}
	@ctags -f ${FILE_TAGS} -a ${FILES_AUTOCODE_SRC_H}
	@touch ${FILE_TAGS_STAMP}

vim_mk: ${FILE_TAGS_STAMP}
#help [global] Open Vim with all .mk Makefiles.
	vim ${FILES_MK}
.PHONY: vim_mk

geany_all:
#help [global] Open Geany with all .c and .h source files.
	geany ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}
.PHONY: geany_all

geany_mk:
#help [global] Open Geany with all .mk Makefiles.
	geany ${FILES_MK}
.PHONY: geany_mk

geany_tm:
#help [global] Open Geany with all TaskMate .c and .h source files (excluding autoCode).
	geany ${FILES_SRC} ${FILES_SRC_H}
.PHONY: geany_tm
