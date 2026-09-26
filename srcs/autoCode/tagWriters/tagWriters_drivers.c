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

#include "tagWriters.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tagWriterWriteDriversAlloc(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	fprintf(context->file, "\tmod_driver_item_t *mod;\n");
	fprintf(context->file, "\thal_driver_control_data_t control_data;\n");

	for( int i = 0; i < mod->modules_count; i++ )
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
		else { fprintf(context->file, "\t\t.address = 0x%02X,\n", mod->modules[i].address); }
		fprintf(context->file, "\t\t.control = hal_%sControl\n", mod->modules[i].name);
		fprintf(context->file, "\t};\n");
	}
}

void tagWriterWriteDriverNameCatalog(const tag_writer_context_t *context)
{
	const module_type_t *mod = &context->data_base->modules_type[MOD_DRIVER_ID];

	for( int i = 0; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "TM_STR_NEW(driver%i_name, \"%s\");\n", i, mod->modules[i].name);
	}

	fprintf(context->file,
			"\nstatic const tm_string_t *const driver_name_catalog[MOD_DRIVER_COUNT] =\n{\n");
	for( int i = 0; i < mod->modules_count; i++ )
	{
		fprintf(context->file, "\t&driver%i_name,\n", i);
	}
	fprintf(context->file, "};\n");
}
