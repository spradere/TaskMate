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
${FILE_TAGS_STAMP}: ${FILES_NOTARGET_SRC} ${FILES_NOTARGET_SRC_H} ${FILES_AUTOCODE_SRC} \
					${FILES_AUTOCODE_SRC_H}
	@ctags -f ${FILE_TAGS} ${FILES_NOTARGET_SRC}
	@ctags -f ${FILE_TAGS} -a ${FILES_NOTARGET_SRC_H}
	@ctags -f ${FILE_TAGS} -a ${FILES_AUTOCODE_SRC}
	@ctags -f ${FILE_TAGS} -a ${FILES_AUTOCODE_SRC_H}
	@touch ${FILE_TAGS_STAMP}

.PHONY: vim_mk
vim_mk: ${FILE_TAGS_STAMP}
#help [global] Open Vim with all .mk Makefiles.
	vim ${FILES_MK}

.PHONY: geany_autoCode
geany_autoCode:
#help [global] Open Geany with all autoCode .c and .h files.
	geany ${FILES_AUTOCODE_SRC_ALL}

.PHONY: geany_mk
geany_mk:
#help [global] Open Geany with all .mk Makefiles.
	geany ${FILES_MK}

.PHONY: geany_tm
geany_tm:
#help [global] Open Geany with all TaskMate .c and .h files.
	geany ${FILES_NOTARGET_SRC_ALL}
