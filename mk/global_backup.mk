################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
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


# Git push, use command line : # make push M="message"
push:
	@printf "\n\033[1;33mGit routine for \"${M}\" commit\033[0m\n\n"
	@git add .
	@git commit -m "${M}"
	@git push
	@printf "\n"
.PHONY: push

# USB key backup with current tag in directory
backup:
	@printf "\n\033[1;33mBackup to <${USB_DIR}${TASKMATE_DIR}>\033[0m\n\n"
	@printf "\033[0;33mInsert USB key and press ENTER to continue ... \033[0m\n"
	@read DUMMY_VAR

	#Test if USB key is mount, do if not
	@if mount | grep "/media/usbkey" > /dev/null; then \
		printf "\033[0;33mUSB key already mounted ${USB_DIR}\033[0m\n"; \
	else \
		printf "\033[0;33mMount USB key ${USB_DIR}\033[0m\n"; \
		mount -v -t msdosfs ${USB_DEV} ${USB_DIR}; \
	fi

	# Test if dest directory exist, create if not
	@if [ -d "${USB_DIR}${TASKMATE_DIR}" ]; then \
	else \
		mkdir ${USB_DIR}${TASKMATE_DIR}; \
	fi

	# Run rsync
	@printf "\033[0;33mRun rsync, output logged in log/rsync.log\033[0m\n"
	rsync -av * --progress --delete --exclude "*.o" --exclude="html" --exclude="build" \
		"${USB_DIR}${TASKMATE_DIR}/" > log/rsync.log

	# Umount
	@printf "\033[0;33mUmount ${USB_DIR}\033[0m\n"
	@umount ${USB_DIR}
	@printf "\n"
.PHONY: backup
