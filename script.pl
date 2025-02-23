#!/usr/bin/perl
use strict;
use warnings;

# Define file paths
my $task_file   = "tasks";     # Input: Task list
my $source_file = "sysCore/TaskMate.c";    # Input: Source file to modify
my $temp_file   = "TaskMate.tmp";  # Temporary output file

# Read task list into an array
open(my $task_fh, "<", $task_file) or die "Error opening $task_file: $!";
my @tasks = <$task_fh>;
chomp @tasks;  # Remove newline characters from each task name
close($task_fh);

# Open TaskMate.c for reading and a temp file for writing
open(my $source_fh, "<", $source_file) or die "Error opening $source_file: $!";
open(my $output_fh, ">", $temp_file) or die "Error creating $temp_file: $!";

# Process TaskMate.c line by line
while (my $line = <$source_fh>) {
    # Insert generated includes
    if ($line =~ /\[TAG_INCLUDE\]/) {
        #print $output_fh "$lchmod +x task_generator.pline";
        print $output_fh map { "#include \"$_\.h\"\n" } @tasks;
    }
    # Insert task count
    elsif ($line =~ /\[TAG_TASK_COUNT\]/) {
        print $output_fh "#define TASK_COUNT " . scalar(@tasks) . "\n";
    }
    # Insert task table
    elsif ($line =~ /\[TAG_TASK_TABLE\]/) {
        print $output_fh "$line";
        print $output_fh "task_t task_table[TASK_COUNT] = {\n";
        print $output_fh map { "    {\"$_\", ${_}_init},\n" } @tasks;
        print $output_fh "};\n";
    }
    # Insert task initialization calls
    elsif ($line =~ /\[TAG_INIT_CALLS\]/) {
        print $output_fh "$line";
        print $output_fh map { "    taskCreate($_);\n" } @tasks;
    }
    # Print the line as is if no special tag is found
    else {
        print $output_fh "$line";
    }
}

# Close files
close($source_fh);
close($output_fh);

# Replace original file with updated version
#rename $temp_file, $source_file or die "Error replacing $source_file: $!";

print "TaskMate.c updated successfully!\n";
