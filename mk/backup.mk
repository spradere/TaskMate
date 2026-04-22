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
# Backup
################################################################################

gitignore: ${GIT_IGNORE_STAMP}
#help [global] generate gitignore file
.PHONY: gitignore

push: ${GIT_IGNORE_STAMP}
#help [global] Git push routine, use command line : # make push M="message"
	@printf "\n%sGit routine for \"${M}\" commit -> ${UPSTREAM} %s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@git add .
	@git commit -m "${M}"
	@git push
	@printf "\n"
.PHONY: push

pull:
#help [global] Git pull routine
	@git fetch
	@git status
	@printf "%sPress ENTER to continue or Ctrl C to exit%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@read dummy_var
	@git stash
	@git merge test
	@git stash pop
.PHONY: pull

merge:
#help [global] Git merge test -> main routine
	@git switch main
	@git merge --no-ff test
	@git push
	@git switch test
.PHONY: merge


# Write .gitignore file
${GIT_IGNORE_STAMP}: ${MAKE_DIR}/backup.mk ${MAKE_DIR}/path_files.mk
	@printf "# exclude everything\n" > "${GIT_IGNORE}"
	@printf "*\n" >> "${GIT_IGNORE}"

	@printf "\n# allowed directories and files\n" >> "${GIT_IGNORE}"
.for dir in ${GIT_ALLOWED_DIRS}
	@printf "!${dir}/\n" >> "${GIT_IGNORE}"
	@printf "!${dir}/**/\n" >> "${GIT_IGNORE}"
.for file in ${GIT_ALLOWED_FILES.${dir}}
	@printf "!${dir}/**/${file}\n" >> "${GIT_IGNORE}"
.endfor
.endfor

	@printf "\n# allowed files\n" >> "${GIT_IGNORE}"
.for file in ${GIT_ALLOWED_FILES}
	@printf "!${file}\n" >> "${GIT_IGNORE}"
.endfor
	@touch ${GIT_IGNORE_STAMP}

backup:
#help [global] USB key backup with current git tag in directory.
	@printf "\n%sBackup to <${USB_DIR}${TM_BACKUP_DIR}>%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "%sInsert USB key and press ENTER to continue ... %s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@read DUMMY_VAR

	# Test if USB key is mount, do if not
	@if mount | grep -q "${USB_DIR}"; then \
		printf "%sUSB key already mounted ${USB_DIR}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
	else \
		printf "%sMount USB key ${USB_DIR}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
		mount -v -t msdosfs ${USB_DEV} ${USB_DIR}; \
	fi

	# Run rsync
	@printf "%sRun rsync, output logged in ${RSYNC_LOG}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@mkdir -p ${USB_DIR}${TM_BACKUP_DIR}
	rsync -av ./ --progress --delete --delete-excluded \
		--include="${BUILD_DIR}/" \
		--include="${BUILD_DIR_TARGET}/" \
		--include="${BUILD_DIR_TARGET}/build_counter" \
		--exclude="${BUILD_DIR_TARGET}/*" \
		--exclude="${BUILD_DIR}/*" --exclude="${LOG_DIR}" \
		"${USB_DIR}${TM_BACKUP_DIR}/" > "${RSYNC_LOG}"

	# umount
	@printf "%sUmount ${USB_DIR}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@umount ${USB_DIR}
	@printf "\n"
.PHONY: backup
