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

# Prefix each path with its filename so :O sorts by filename, then by full path.
FILES_EDITOR_AUTOCODE = ${FILES_AUTOCODE_SRC_ALL}
FILES_EDITOR_AUTOCODE := ${FILES_EDITOR_AUTOCODE:C,^.*/([^/]*)$,\1|&,:O:C,^[^|]*\|,,}
FILES_EDITOR_DOC = ${FILES_DOC}
FILES_EDITOR_DOC := ${FILES_EDITOR_DOC:C,^.*/([^/]*)$,\1|&,:O:C,^[^|]*\|,,}
FILES_EDITOR_MK = ${FILES_MK}
FILES_EDITOR_MK := ${FILES_EDITOR_MK:C,^.*/([^/]*)$,\1|&,:O:C,^[^|]*\|,,}
FILES_EDITOR_TM = ${FILES_NOTARGET_SRC_ALL}
FILES_EDITOR_TM := ${FILES_EDITOR_TM:C,^.*/([^/]*)$,\1|&,:O:C,^[^|]*\|,,}

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
	vim ${FILES_EDITOR_MK}

.PHONY: geany_autoCode
geany_autoCode:
#help [global] Open Geany with all autoCode .c and .h files.
	geany ${FILES_EDITOR_AUTOCODE}

.PHONY: geany_mk
geany_mk:
#help [global] Open Geany with all .mk Makefiles.
	geany ${FILES_EDITOR_MK}

.PHONY: geany_tm
geany_tm:
#help [global] Open Geany with all TaskMate .c and .h files.
	geany ${FILES_EDITOR_TM}

.PHONY: geany_doc
geany_doc:
#help [global] Open Geany with all documentations files.
	geany ${FILES_EDITOR_DOC}
