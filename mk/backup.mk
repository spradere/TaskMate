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

# Write .gitignore file
${FILE_GIT_IGNORE}: ${PATH_MAKEFILES}/backup.mk ${PATH_MAKEFILES}/path_files.mk
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

	@printf "\n# excluded files\n" >> "${FILE_GIT_IGNORE}"
.for file in ${FILES_GIT_EXCLUDED}
	@printf "${file}\n" >> "${FILE_GIT_IGNORE}"
.endfor

.PHONY: backup
backup:
#help [global] USB key backup with version in directory name.
	@printf "\n%sBackup to <${PATH_USBKEY}${VAL_TM_BACKUP_DIR}>%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "%sInsert USB key and press ENTER to continue ... %s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@read DUMMY_VAR

.if ${OPT_ENVIRONMENT} == "freebsd"
.if !empty(PATH_USBKEY) && !empty(FILE_USBDEV) && !empty(VAL_TM_BACKUP_DIR)
	@printf "%sFreeBSD backup%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"
			
	# Test key volume name
	@label=$$(fstyp -l ${FILE_USBDEV} 2>/dev/null | awk '{print $$2}'); \
	if [ "$$label" != "${VAL_USB_LABEL_EXPECTED}" ]; then \
		printf "Wrong USB volume: <%s>\n" "$$label"; \
		exit 1; \
	fi
	@printf "USB volume OK: %s\n" "${VAL_USB_LABEL_EXPECTED}"
	
	# Mount the USB key if necessary
	@if mount | grep -q "${PATH_USBKEY}"; then \
		printf "%sUSB key already mounted ${PATH_USBKEY}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
	else \
		printf "%sMount USB key ${PATH_USBKEY}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
		mount -v -t msdosfs ${FILE_USBDEV} ${PATH_USBKEY}; \
	fi
	
	# Test mount point
	@mount | grep -q " on ${PATH_USBKEY} " || { \
		printf "ERROR: %s is not mounted\n" "${PATH_USBKEY}"; \
		exit 1; \
	}
	# Run rsync
	@printf "%sRun rsync, output logged in ${FILE_RSYNC_LOG}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@mkdir -p ${PATH_USBKEY}${VAL_TM_BACKUP_DIR}
	rsync -av ./ --progress --delete --delete-excluded \
		--exclude=".git" \
		--exclude="${PATH_BUILDS}/" \
		--exclude="${PATH_LOGS}" \
		"${PATH_USBKEY}${VAL_TM_BACKUP_DIR}/" > "${FILE_RSYNC_LOG}"

	# Unmount
	@printf "%sUmount ${PATH_USBKEY}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@umount ${PATH_USBKEY}
	@printf "\n"
.endif
.endif
