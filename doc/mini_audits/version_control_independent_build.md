# Version-control-independent build

Date: 31 October 2026
Version: `v0.33`
Revision: `ea76f67dfbbc7a8473b866dde3543084da79db7d`

## Question

Which Git dependencies prevent the build system and its validation from operating in a source export or under another version-control system, and how should they be removed without conflating the build with optional repository maintenance tools?

## Problems found in code

- ~~[mk/data.mk](../../mk/data.mk) derives the TaskMate version and build number from `git describe` and `git rev-list`; [git_version.sh](../../scripts/git_version.sh) falls back to `0.00` outside a repository, but the build number has no fallback and the resulting macros are consumed by [system.c](../../srcs/system/services/system.c). A source export therefore does not provide the same valid build inputs as a Git checkout.~~
- [programs-list.conf](../../conf/programs-list.conf) declares Git as a required program in the common list checked by the Make graph. This makes the executable mandatory even for operations that have no version-control purpose.
- [build.mk](../../mk/build.mk) writes Git details into the build report and makes `all` depend on `.gitignore`; [path_files.mk](../../mk/path_files.mk) and [backup.mk](../../mk/backup.mk) make the build own Git-specific allow lists, ignore-file generation, and backup exclusions. A normal firmware build therefore reads version-control state and modifies version-control policy.
- [build_test.sh](../../test/build/build_test.sh) tests Git-derived versions, bundles, and branch deletion in the build-system test suite. These tests require a Git repository and executable even though bundle and branch scripts are repository maintenance utilities rather than firmware build components.

## Solution

Add a declarative project version file as the source of `TM_VER_MAJOR` and `TM_VER_MINOR`, and replace the commit count with a validated, explicit build identifier that has a deterministic default and can be overridden by a release or CI invocation. Remove Git commands, `SCRIPT_GIT_VERSION`, and Git report fields from the Make graph, and remove Git from the common required-program list.

Stop generating `.gitignore` from `all` and remove the Git-specific Make variables and rules. Version-control ignore files, if retained, must be static repository metadata outside the build. Make backup selection depend only on project and artifact paths, not on a particular version-control metadata directory.

Remove the Git-specific cases from mandatory build-system validation or move them with the optional repository maintenance scripts into a separate test entry point. Add a regression test that runs the normal `test1` build configuration from a tree without version-control metadata and with no version-control executable available. Acceptance requires identical generated version macros for identical explicit inputs and no version-control command or metadata access from any mandatory build or build-validation target.
