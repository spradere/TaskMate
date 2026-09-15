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
# Calculate AVR flash and RAM usage
################################################################################

NR == 2 {
	text = $1
	data = $2
	bss = $3
	flash = text + data
	ram = data + bss
	flash_total = flash_total_k * 1024
	ram_total = ram_total_k * 1024
	flash_pct = (flash / flash_total) * 100
	ram_pct = (ram / ram_total) * 100

	printf("Memory used total %%\n") > output_file
	printf("Flash %d %d %f\n", flash, flash_total, flash_pct) >> output_file
	printf("RAM %d %d %f\n", ram, ram_total, ram_pct) >> output_file
	close(output_file)
}
