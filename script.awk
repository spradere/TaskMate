#!/usr/bin/awk -f

BEGIN {
    task_file = "tasks.txt"
    source_file = "TaskMate.c"
    temp_file = "TaskMate.tmp"

    # Read task list
    while ((getline task < task_file) > 0) {
        tasks[++task_count] = task
    }
    close(task_file)

    # Process `TaskMate.c`
    inside = 0
    while ((getline line < source_file) > 0) {
        if (line ~ /\[TAG_INCLUDE\]/) {
            print line
            for (i = 1; i <= task_count; i++) print "#include \"" tasks[i] ".h\""
            inside = 1
        } else if (line ~ /\[TAG_TASK_COUNT\]/) {
            print "#define TASK_COUNT " task_count
            inside = 1
        } else if (line ~ /\[TAG_TASK_TABLE\]/) {
            print line
            print "task_t task_table[TASK_COUNT] = {"
            for (i = 1; i <= task_count; i++) print "    {\"" tasks[i] "\", " tasks[i] "_init},"
            print "};"
            inside = 1
        } else if (line ~ /\[TAG_INIT_CALLS\]/) {
            print line
            for (i = 1; i <= task_count; i++) print "    taskCreate(" tasks[i] ");"
            inside = 1
        } else if (line ~ /\[TAG_INCLUDE_END\]/ || line ~ /\[TAG_TASK_TABLE_END\]/ || line ~ /\[TAG_INIT_CALLS_END\]/) {
            inside = 0
        }
        if (!inside) print line
    }
    close(source_file)
}
