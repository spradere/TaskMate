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
# Backup
################################################################################

# Get git tag for USB key directory backup
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1
GIT_TAG != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || echo "0.00"
TASKMATE_DIR != printf "/code/TaskMate/TaskMate_%s" ${GIT_TAG}


# Git push
push:
#@ [global] Git push routine, use command line : # make push M="message"
	@printf "\n%sGit routine for \"${M}\" commit%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@git add .
	@git commit -m "${M}"
	@git push
	@printf "\n"
.PHONY: push

# USB key backup
backup:
#@ [global] USB key backup with current git tag in directory.
	@printf "\n%sBackup to <${USB_DIR}${TASKMATE_DIR}>%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "%sInsert USB key and press ENTER to continue ... %s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@read DUMMY_VAR

	#Test if USB key is mount, do if not
	@if mount | grep -q "${USB_DIR}"; then \
		printf "%sUSB key already mounted ${USB_DIR}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
	else \
		printf "%sMount USB key ${USB_DIR}%s\n" \
			"${COLOUR_BACKUP}" "${COLOUR_RESET}"; \
		mount -v -t msdosfs ${USB_DEV} ${USB_DIR}; \
	fi

	# Test if destination directory exist, create if not
	@if [ -d "${USB_DIR}${TASKMATE_DIR}" ]; then \
	else \
		mkdir ${USB_DIR}${TASKMATE_DIR}; \
	fi

	# Run rsync
	@printf "%sRun rsync, output logged in log/rsync.log%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	rsync -av * --progress --delete --exclude "*.o" --exclude="html" --exclude="build" --exclude="log" \
		"${USB_DIR}${TASKMATE_DIR}/" > ${LOG_DIR}/rsync.log

	# umount
	@printf "%sUmount ${USB_DIR}%s\n" \
		"${COLOUR_BACKUP}" "${COLOUR_RESET}"
	@umount ${USB_DIR}
	@printf "\n"
.PHONY: backup
