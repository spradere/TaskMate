/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file writeInclude.c
 * @brief write include code implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/writeInclude.h"

void writeInclude(const modules_database_t *data_base, const char *file_name,
                  const char *arch, const char *mcu, const char *board) {
  FILE *file_include = fopen(file_name, "w");
  if (file_include == NULL) {
    msgError("creating file");
    printf("\t <%s>\n", file_name);
    exit(1);
  }

  fprintf(file_include, "// Auto generated code, do not edit !\n");
  fprintf(file_include, "// any changes will be lost\n\n");

  const module_type_t *mod = &data_base->modules_type[MODULES_DRIVERS_ID];

  for (int i = 0; i < mod->modules_count; i++) {
    fprintf(file_include, "#include \"hal/board/%s/%s.h\"\n", board,
            mod->modules[i].name);
  }
  fprintf(file_include, "\n");

  mod = &data_base->modules_type[MODULES_SERVICES_ID];

  for (int i = 0; i < mod->modules_count; i++) {
    fprintf(file_include, "#include \"services/%s.h\"\n", mod->modules[i].name);
  }
  fprintf(file_include, "\n");

  mod = &data_base->modules_type[MODULES_TASKS_ID];

  for (int i = 0; i < mod->modules_count; i++) {
    fprintf(file_include, "#include \"tasks/%s.h\"\n", mod->modules[i].name);
  }

  fclose(file_include);
}
