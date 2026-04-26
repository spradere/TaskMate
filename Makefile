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
# Main makefile
################################################################################

# Main directories
PATH_SOURCES = src
PATH_BUILDS = build
PATH_LOGS = log
PATH_MAKEFILES = mk
PATH_DOCS = doc
PATH_SCRIPTS = scripts

# Build data
.include "${PATH_MAKEFILES}/options.mk"
.include "${PATH_MAKEFILES}/data.mk"
.include "${PATH_MAKEFILES}/colours.mk"

# Hardware target choice
.include "${PATH_MAKEFILES}/hardware_target.mk"

# Proceed files
.include "${PATH_MAKEFILES}/path_files.mk"
.include "${PATH_MAKEFILES}/sources.mk"
.include "${PATH_MAKEFILES}/header_allow.mk"

# Make global process
.include "${PATH_MAKEFILES}/build.mk"
.include "${PATH_MAKEFILES}/utils.mk"
.include "${PATH_MAKEFILES}/backup.mk"
.include "${PATH_MAKEFILES}/editors.mk"

# Make arch specific
.include "${FILE_ARCH_CC}"
