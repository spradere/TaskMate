/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters_drivers.c
 * @brief autoCode driver tag writer implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <ctype.h>

#define TAGWRITERSDRIVERS_WIDTH_ADDRESS 2U
#include "tagWriters.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteDriversAlloc(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	fprintf(context->file, "\tmod_driver_item_t *mod;\n");
	fprintf(context->file, "\thal_driver_control_data_t control_data;\n");

	for( int i = 0U; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "\n\tmod = mod_driverGetPointer(%i);\n", i);
		fprintf(context->file, "\tcontrol_data.run_level = %i;\n", mod->modules[i].status);
		fprintf(context->file,
				"\thal_%sControl(DRV_CTRL_RLSET, &control_data);\n",
				mod->modules[i].name);
		fprintf(context->file, "\t*(mod) = (mod_driver_item_t)\n");
		fprintf(context->file, "\t{\n");
		if( mod->modules[i].address == MOD_DRIVER_ADDRESS_NONE )
		{
			fprintf(context->file, "\t\t.address = MOD_DRIVER_ADDRESS_NONE,\n");
		}
		else
		{
			fprintf(context->file, "\t\t.address = 0x%0*X,\n",
					(int)TAGWRITERSDRIVERS_WIDTH_ADDRESS, mod->modules[i].address);
		}
		fprintf(context->file, "\t\t.control = hal_%sControl\n", mod->modules[i].name);
		fprintf(context->file, "\t};\n");
	}
}

void tagWriterWriteDriverNameCatalog(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	for( int i = 0U; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "TM_STR_NEW(driver%i_name, \"%s\");\n", i, mod->modules[i].name);
	}

	fprintf(context->file,
			"\nstatic const tm_string_t *const driver_name_catalog[MOD_DRIVER_COUNT] =\n{\n");
	for( int i = 0U; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "\t&driver%i_name,\n", i);
	}
	fprintf(context->file, "};\n");
}

void tagWriterWriteDriverHave(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	for( int i = 0U; i < mod->modules_count; i++ )
	{
		const char *name = mod->modules[i].name;
		fprintf(context->file, "#define TM_DRIVER_HAVE_");
		for( size_t character = 0U; name[character] != 0U; character++ )
		{
			const unsigned char current = (unsigned char)name[character];
			const unsigned char previous =
				(character == 0U) ? 0U : (unsigned char)name[character - 1U];
			const unsigned char next = (unsigned char)name[character + 1U];

			// Split lower-to-upper and acronym-to-word boundaries in macro names.
			if( isupper(current) && (character > 0U) &&
				(islower(previous) || isdigit(previous) || (isupper(previous) && islower(next))) )
			{
				fputc('_', context->file);
			}
			fputc(toupper(current), context->file);
		}
		fprintf(context->file, " 1U\n");
	}
}
