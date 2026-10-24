################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# ------------------------------------------------------------------------------
# Description: Compare the private AVR8 context layout with source and linked code.
# Inputs: Context C source, macro header, timer vector dump, and startup dump.
# Outputs: Validation summary or the first layout, macro, or instruction mismatch.
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Expected stack operations from the private context structure
# ------------------------------------------------------------------------------

function fail(message)
{
	print "AVR8 context stack: " message > "/dev/stderr"
	failed = 1
	exit 1
}

function canonical(instruction)
{
	gsub(/\\t/, " ", instruction)
	gsub(/^[[:space:]]+|[[:space:]]+$/, "", instruction)
	gsub(/[[:space:]]+/, " ", instruction)
	gsub(/[[:space:]]*,[[:space:]]*/, ",", instruction)
	return instruction
}

function add_field(name)
{
	fields[++field_count] = name
}

function add_save(instruction)
{
	save[++save_count] = canonical(instruction)
}

function add_restore(instruction)
{
	restore[++restore_count] = canonical(instruction)
}

function build_expected(    i, field)
{
	if( !struct_closed || register_count != 30 || field_count != 37 )
	{
		fail("missing or unexpected packed AVR8 context layout")
	}
	if( fields[35] != "pc_ext" || fields[36] != "pc_high" || fields[37] != "pc_low" )
	{
		fail("program counter must occupy the final three stack bytes")
	}

	# AVR hardware stacks the return address before the naked ISR runs.
	for( i = field_count - 3; i >= 1; i-- )
	{
		field = fields[i]
		if( field == "avr8_SREG" )
		{
			add_save("in r0,__SREG__")
			add_save("push r0")
		}
		else if( field == "avr8_RAMPZ" )
		{
			add_save("in r0,__RAMPZ__")
			add_save("push r0")
		}
		else if( field ~ /^r([0-9]|[12][0-9]|3[01])$/ )
		{
			add_save("push " field)
			if( field == "r1" ) { add_save("clr r1") }
		}
		else { fail("unsupported context field: " field) }
	}
	for( i = 1; i <= field_count - 3; i++ )
	{
		field = fields[i]
		if( field == "avr8_SREG" )
		{
			add_restore("pop r0")
			add_restore("out __SREG__,r0")
		}
		else if( field == "avr8_RAMPZ" )
		{
			add_restore("pop r0")
			add_restore("out __RAMPZ__,r0")
		}
		else { add_restore("pop " field) }
	}
}

function disassembly_form(instruction)
{
	gsub(/__SREG__/, "0x3f", instruction)
	gsub(/__RAMPZ__/, "0x3b", instruction)
	if( instruction == "clr r1" ) { instruction = "eor r1,r1" }
	return instruction
}

function check_sequence(actual, start, expected, count, label,    i, want)
{
	if( start < 1 ) { fail(label " sequence is missing") }
	for( i = 1; i <= count; i++ )
	{
		want = disassembly_form(expected[i])
		if( actual[start + i - 1] != want )
		{
			fail(label " instruction " i ": expected " want ", got " actual[start + i - 1])
		}
	}
}

# ------------------------------------------------------------------------------
# Source and disassembly input
# ------------------------------------------------------------------------------

FNR == 1 {
	stage++
	if( stage == 2 ) { build_expected() }
}

stage == 1 {
	if( $1 == "#define" && $2 == "AVR8CONTEXT_R_COUNT" )
	{
		register_count = $3
		sub(/U$/, "", register_count)
		register_count += 0
	}
	if( /^typedef struct/ && /packed/ ) { in_struct = 1; next }
	if( in_struct && /^[[:space:]]*}[[:space:]]*[A-Za-z_][A-Za-z_0-9]*;/ )
	{
		struct_closed = 1
		in_struct = 0
	}
	if( !in_struct || $0 !~ /^[[:space:]]*uint8_t[[:space:]]+/ ) { next }
	field = $0
	sub(/\/\/.*/, "", field)
	sub(/^[[:space:]]*uint8_t[[:space:]]+/, "", field)
	sub(/;.*/, "", field)
	gsub(/[[:space:]]/, "", field)
	if( field == "r_registers[AVR8CONTEXT_R_COUNT]" )
	{
		for( i = 31; i >= 32 - register_count; i-- ) { add_field("r" i) }
	}
	else if( field ~ /^[A-Za-z_][A-Za-z_0-9]*$/ ) { add_field(field) }
	else { fail("unsupported context declaration: " field) }
	next
}

stage == 2 {
	if( /^#define AVR8_CONTEXT_SAVE[[:space:]]/ ) { macro = "save"; next }
	if( /^#define AVR8_CONTEXT_RESTORE[[:space:]]/ ) { macro = "restore"; next }
	if( /^#define/ || /^#endif/ ) { macro = "" }
	if( macro == "" || index($0, "\"") == 0 ) { next }
	instruction = $0
	sub(/^[^"]*"/, "", instruction)
	sub(/".*$/, "", instruction)
	sub(/\\n\\t.*/, "", instruction)
	instruction = canonical(instruction)
	if( macro == "save" ) { macro_save[++macro_save_count] = instruction }
	else { macro_restore[++macro_restore_count] = instruction }
	next
}

stage >= 3 && /^[[:space:]]*[[:xdigit:]]+:/ {
	# avr-objdump separates address, bytes, mnemonic, and operands with tabs.
	part_count = split($0, parts, "\t")
	if( part_count < 3 ) { fail("unexpected avr-objdump instruction format") }
	instruction = parts[3]
	if( part_count >= 4 ) { instruction = instruction " " parts[4] }
	sub(/[[:space:]]*;.*/, "", instruction)
	instruction = canonical(instruction)
	if( stage == 3 ) { vector_ops[++vector_count] = instruction }
	else { start_ops[++start_count] = instruction }
}

# ------------------------------------------------------------------------------
# Validation
# ------------------------------------------------------------------------------

END {
	if( failed ) { exit 1 }
	if( stage != 4 ) { fail("expected context source, header, vector, and start disassembly") }
	if( macro_save_count != save_count || macro_restore_count != restore_count )
	{
		fail("macro instruction count disagrees with context layout")
	}
	for( i = 1; i <= save_count; i++ )
	{
		if( macro_save[i] != save[i] )
		{
			fail("save macro instruction " i ": expected " save[i] ", got " macro_save[i])
		}
	}
	for( i = 1; i <= restore_count; i++ )
	{
		if( macro_restore[i] != restore[i] )
		{
			fail("restore macro instruction " i ": expected " restore[i] ", got " macro_restore[i])
		}
	}

	if( vector_count == 0 || vector_ops[vector_count] != "reti" )
	{
		fail("timer context vector does not end with reti")
	}
	check_sequence(vector_ops, 1, save, save_count, "timer vector save")
	check_sequence(vector_ops, vector_count - restore_count, restore, restore_count,
				   "timer vector restore")
	if( vector_count <= save_count + restore_count + 1 )
	{
		fail("timer vector has no scheduler stack switch")
	}
	for( i = save_count + 1; i < vector_count - restore_count; i++ )
	{
		if( vector_ops[i] ~ /^(push|pop) / )
		{
			fail("unexpected stack operation between vector save and restore")
		}
	}

	# LTO can place hal_contextStart in main; locate its SP write in either symbol.
	for( i = 1; i <= start_count - restore_count - 2; i++ )
	{
		if( start_ops[i] == "out 0x3e,r27" && start_ops[i + 1] == "out 0x3d,r26" )
		{
			check_sequence(start_ops, i + 2, restore, restore_count, "initial restore")
			if( start_ops[i + 2 + restore_count] != "reti" )
			{
				fail("initial restore does not end with reti")
			}
			initial_count++
		}
	}
	if( initial_count != 1 ) { fail("expected one initial context restore") }
	print "AVR8 context layout, macros, timer vector, and initial restore passed"
}
