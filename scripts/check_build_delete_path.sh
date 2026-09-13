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

allow_build_root=false

if [ "${1:-}" = "--allow-build-root" ]; then
	allow_build_root=true
	shift
fi

if [ "$#" -eq 0 ]; then
	printf "Usage: %s [--allow-build-root] <path>...\n" "$0" >&2
	exit 2
fi

script_path=$(realpath "$0") || exit 1
script_directory=${script_path%/*}
project_root=${script_directory%/*}
build_root=${project_root}/build

for delete_path in "$@"; do
	if [ -z "${delete_path}" ]; then
		printf "Unsafe empty build deletion path rejected.\n" >&2
		exit 1
	fi

	if [ -e "${delete_path}" ] || [ -L "${delete_path}" ]; then
		canonical_path=$(realpath "${delete_path}") || exit 1
	else
		case "${delete_path}" in
		*/*)
			parent_path=${delete_path%/*}
			file_name=${delete_path##*/}
			;;
		*)
			parent_path=.
			file_name=${delete_path}
			;;
		esac

		canonical_parent=$(realpath "${parent_path}") || exit 1
		canonical_path=${canonical_parent}/${file_name}
	fi

	case "${canonical_path}" in
	"${build_root}")
		if [ "${allow_build_root}" = true ]; then
			continue
		fi
		;;
	"${build_root}"/*)
		continue
		;;
	esac

	printf "Unsafe build deletion path rejected: %s\n" "${delete_path}" >&2
	exit 1
done
