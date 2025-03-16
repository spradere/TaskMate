/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */
 
 /**
 * @file TaskMate_tag_expand.c
 * @brief helper to generate code for task and drivers handle in Taskmate.c  
 * 
 * - Simple and reliable, read plain text file <task_list> and <driver_list>
 * - Write code for #include / static allocation / initialisation
 * 
 * @warning file name must match with main function, lcd.c -> void lcd(void)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_FILE "task_list"
#define DRIVER_FILE "driver_list" 
#define SOURCE_FILE "sysCore/TaskMate.c"
#define TEMP_FILE "sysCore/TaskMate.tmp.c"

#define TASK_COUNT_MAX 256
#define DRIVER_COUNT_MAX 256

#define LINE_SIZE_MAX 256
char line[LINE_SIZE_MAX];

#define ARGN_COUNT_MAX 4
#define ARGV_SIZE_MAX 64
char argv[ARGN_COUNT_MAX][ARGV_SIZE_MAX]; 

void get_argv(void);

int main(void)
{
	//allocate tables
	char task_table[TASK_COUNT_MAX][LINE_SIZE_MAX];
	char driver_table[DRIVER_COUNT_MAX][LINE_SIZE_MAX];
	
	//open list files
	FILE *task_file=fopen(TASK_FILE,"r");
	if(task_file==0)
	{
		printf("error : task file not found <%s>\n",TASK_FILE);
		exit(0);
	}	
	
	FILE *driver_file=fopen(DRIVER_FILE,"r");
	if(driver_file==0)
	{
		printf("error : driver file not found <%s>\n",DRIVER_FILE);
		exit(0);
	}		
	
	//read task file -> table
	int task_count=0;
    while(	(task_count < TASK_COUNT_MAX)
			&& fgets(task_table[task_count], LINE_SIZE_MAX, task_file)	) 
    {
        task_table[task_count][strcspn(task_table[task_count], "\n")] = 0;  // Remplace newline
        task_count++;
    }
	if(task_count==0){printf("error : no task\n"); exit(0);}

	//read driver file -> table
	int driver_count=0;
    while(	(driver_count < DRIVER_COUNT_MAX)
			&& fgets(driver_table[driver_count], LINE_SIZE_MAX, driver_file)	) 
    {
        driver_table[driver_count][strcspn(driver_table[driver_count], "\n")] = 0;  // Remplace newline
        driver_count++;
    }
	if(driver_count==0){printf("error : no driver\n"); exit(0);}

	// close files
	fclose(task_file);
	fclose(driver_file);
		
	
	// print tables
	int i;
	
	printf("found task :\n");
	for(i=0;i<task_count;i++)
	{
		printf("\ttask[%i]=%s\n",i, task_table[i]);
	}
	
	printf("\nfound driver :\n");
	for(i=0;i<driver_count;i++)
	{
		printf("\tdriver[%i]=%s\n",i, driver_table[i]);
	}
	printf("\n");
	
	
	// open souce and tmp file
	FILE *file_src=fopen(SOURCE_FILE,"r");
	if(file_src==0)
	{
		printf("error : source file not found <%s>\n",SOURCE_FILE);
		exit(1);
	}	
		
	FILE *file_tmp=fopen(TEMP_FILE,"w");
	if(file_src==0)
	{
		printf("error : creating temp file  <%s>\n",TEMP_FILE);
		exit(1);
	}		
	
	// read form source
	int tag_section=0;
	
	while (fgets(line, LINE_SIZE_MAX, file_src)) 
	{
		get_argv();
		
		if ( !(strcmp(argv[0], "//")) && !(strcmp(argv[1],"[tag]")) )
		{
			printf("found tag <%s> <%s> ... ",argv[2],argv[3]);
			fprintf(file_tmp,"%s",line);
			tag_section=1;
			
			if( !(strcmp(argv[2], "task")) ) // task tag
			{
				if( !(strcmp(argv[3], "include")) ) // task include
				{
					for(i=0;i<task_count;i++)
					{
						fprintf(file_tmp,"#include \"tasks/%s.h\"\n",task_table[i]);
					}
				}
			
				if( !(strcmp(argv[3], "alloc")) ) // task alloc
				{
					fprintf(file_tmp,"#define TASK_COUNT %i\n",task_count);
					fprintf(file_tmp,"task_table_t task_table[TASK_COUNT];\n");
					fprintf(file_tmp,"uint8_t task_current=0;\n");
				}
				
				if( !(strcmp(argv[3], "init")) ) // task init
				{
					fprintf(file_tmp,"\tuint8_t i=0; \n");
					for(i=0;i<task_count;i++)
					{
						fprintf(file_tmp,"\ttaskCreate(%s,i++);\n",task_table[i]);
					}
				}
			}
			
			
			if( !(strcmp(argv[2], "driver")) ) //driver tag 
			{
				if( !(strcmp(argv[3], "include")) ) // driver include
				{
					for(i=0;i<driver_count;i++)
					{
						fprintf(file_tmp,"#include \"drivers/%s.h\"\n",driver_table[i]);
					}
				}
				
				if( !(strcmp(argv[3], "alloc")) ) // driver alloc
				{
					fprintf(file_tmp,"#define DRIVER_COUNT %i\n",driver_count);
					fprintf(file_tmp,"driver_table_t driver_table[DRIVER_COUNT];\n");
				}
				
				if( !(strcmp(argv[3], "init")) ) // driver init
				{
					for(i=0;i<driver_count;i++)
					{
						fprintf(file_tmp,"\tdriver_table[%i]=(driver_table_t) \n",i);
						fprintf(file_tmp,"\t{\n");
						fprintf(file_tmp,"\t\t.driver_id = %i,\n",i);
						fprintf(file_tmp,"\t\t.driver_name = %sGetName(),\n",driver_table[i]);
						fprintf(file_tmp,"\t\t.setStatus = %sSetStatus, \n",driver_table[i]);
						fprintf(file_tmp,"\t\t.getStatus = %sGetStatus, \n",driver_table[i]);
						fprintf(file_tmp,"\t\t.init = %sInit, \n",driver_table[i]);
						fprintf(file_tmp,"\t\t.start = %sStart, \n",driver_table[i]);
						fprintf(file_tmp,"\t\t.stop = %sStop\n",driver_table[i]);						
						fprintf(file_tmp,"\t};\n");
					}
				}
				
			}
		}
		 	
		if ( !(strcmp(argv[0], "//")) && !(strcmp(argv[1],"[/tag]")) )
		{
			printf("end tag\n");
			tag_section=0;
		}
		
		if(tag_section==0){fprintf(file_tmp,"%s",line);} // tag used to flush old code
	}
	
	// Replace original file with the modified version
    if (remove(SOURCE_FILE) != 0 || rename(TEMP_FILE, SOURCE_FILE) != 0) {
        perror("error : replacing TaskMate.c");
        exit(2);
    }

	fclose(file_src);
	fclose(file_tmp);
	
	return 0;
}


void get_argv(void)
{
	int argn;
	int i_line=0;
	int i_arg;
	
	// reset all argv
	for(argn=0;argn<ARGN_COUNT_MAX;argn++){argv[argn][0]=0;}

	// get off leading space or tab
	while( 	((line[i_line]== ' ') || (line[i_line]== '\t')) 
			&& (line[i_line]!= '\n') && (line[i_line]!=0)
			&& (i_line<LINE_SIZE_MAX-1)	)
		{i_line++;}
	
	// comment detected ?
	if( (line[i_line]=='/')&&(line[i_line+1]=='/') )
	{
		argv[0][0]='/';argv[0][1]='/';argv[0][2]=0;
		
		i_line +=2;
		
		for(argn=1;argn<ARGN_COUNT_MAX;argn++)
		{
			// get off space or tab
			while( 	((line[i_line]== ' ') || (line[i_line]== '\t')) 
					&& (line[i_line]!= '\n') && (line[i_line]!= 0) 
					&& (i_line<LINE_SIZE_MAX-1)	)
					
				{i_line++;}
			
			// exit if end of line
			if ( (line[i_line] == '\n') || (line[i_line] == '\0') ) {break;}
			
			// get argx
			i_arg=0;
			while( 	(line[i_line]!= ' ') && (line[i_line]!= '\t') 
					&& (line[i_line]!= '\n') && (line[i_line]!= 0)
					&& (i_arg<ARGV_SIZE_MAX-1) && (i_line<LINE_SIZE_MAX-1)	)
			{
				argv[argn][i_arg++]=line[i_line++];
			}
			argv[argn][i_arg]=0;
		}
	}
}
