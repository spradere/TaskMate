#!/bin/sh

# ###############################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
# ###############################################################################

set -eu

path_type=${1:-}
case "${path_type}" in
-d | -f) shift ;;
*)
	printf "Usage: %s [-d | -f] <path>...\n" "$0" >&2
	exit 2
	;;
esac

if [ "$#" -eq 0 ]; then
	printf "Usage: %s [-d | -f] <path>...\n" "$0" >&2
	exit 2
fi

current_path=$(realpath .) || exit 1

for path in "$@"; do
	if [ -z "${path}" ]; then
		printf "Empty Make path variable rejected.\n" >&2
		exit 1
	fi

	if [ -e "${path}" ] || [ -L "${path}" ]; then
		if [ ! "${path_type}" "${path}" ]; then
			printf "Invalid %s path rejected: %s\n" "${path_type}" "${path}" >&2
			exit 1
		fi
		canonical_path=$(realpath "${path}") || exit 1
	else
		case "${path}" in
		*/*)
			parent_path=${path%/*}
			file_name=${path##*/}
			;;
		*)
			parent_path=.
			file_name=${path}
			;;
		esac

		canonical_parent=$(realpath "${parent_path}") || exit 1
		canonical_path=${canonical_parent}/${file_name}
	fi

	case "${canonical_path}" in
	"${current_path}" | "${current_path%/}"/*) continue ;;
	esac

	printf "Path outside current directory rejected: %s\n" "${path}" >&2
	exit 1
done
