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

gitignore: ${FILE_GIT_IGNORE_STAMP}
#help [global] generate gitignore file
.PHONY: gitignore

push: gitignore
#help [global] Git push routine, use command line : # make push M="message"
	@printf "\n%sGit routine for \"${M}\" commit -> ${VAL_UPSTREAM} %s\n\n" \
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
${FILE_GIT_IGNORE_STAMP}: ${PATH_MAKEFILES}/backup.mk ${PATH_MAKEFILES}/path_files.mk
	@printf "# exclude everything\n" > "${FILE_GIT_IGNORE}"
	@printf "*\n" >> "${FILE_GIT_IGNORE}"

	@printf "\n# allowed directories and files\n" >> "${FILE_GIT_IGNORE}"
.for dir in ${PATHS_GIT_ALLOWED}
	@printf "!${dir}/\n" >> "${FILE_GIT_IGNORE}"
	@printf "!${dir}/**/\n" >> "${FILE_GIT_IGNORE}"
.for file in ${FILES_GIT_ALLOWED.${dir}}
	@printf "!${dir}/**/${file}\n" >> "${FILE_GIT_IGNORE}"
.endfor
.endfor

	@printf "\n# allowed files\n" >> "${FILE_GIT_IGNORE}"
.for file in ${FILES_GIT_ALLOWED}
	@printf "!${file}\n" >> "${FILE_GIT_IGNORE}"
.endfor
	@touch ${FILE_GIT_IGNORE_STAMP}

backup:
#help [global] USB key backup with current git tag in directory.
	@printf "\n%sBackup to <${PATH_USBKEY}${VAL_TM_BACKUP_DIR}>%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "%sInsert USB key and press ENTER to continue ... %s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@read DUMMY_VAR

	# Test if USB key is mount, do if not
	@if mount | grep -q "${PATH_USBKEY}"; then \
		printf "%sUSB key already mounted ${PATH_USBKEY}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
	else \
		printf "%sMount USB key ${PATH_USBKEY}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
		mount -v -t msdosfs ${FILE_USBDEV} ${PATH_USBKEY}; \
	fi

	# Run rsync
	@printf "%sRun rsync, output logged in ${FILE_RSYNC_LOG}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@mkdir -p ${PATH_USBKEY}${VAL_TM_BACKUP_DIR}
	rsync -av ./ --progress --delete --delete-excluded \
		--exclude=".git" \
		--exclude="${PATH_BUILDS}/" \
		--exclude="${PATH_LOGS}" \
		"${PATH_USBKEY}${VAL_TM_BACKUP_DIR}/" > "${FILE_RSYNC_LOG}"

	# umount
	@printf "%sUmount ${PATH_USBKEY}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@umount ${PATH_USBKEY}
	@printf "\n"
.PHONY: backup
