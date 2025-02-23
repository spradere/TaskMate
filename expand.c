#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_FILE "task_list"
#define DRIVER_FILE "driver_list" 
#define SOURCE_FILE "sysCore/TaskMate.c"
#define TEMP_FILE "TaskMate.tmp"

#define TASK_COUNT_MAX 256
#define DRIVER_COUNT_MAX 256
#define LINE_MAX 256

char line[LINE_MAX];
#define ARGV_MAX 4
char argv[ARGV_MAX][64]; // [tag] task include for example

void get_argv(void);

int main(void)
{
	//allocate tables
	char task_table[TASK_COUNT_MAX][LINE_MAX];
	char driver_table[DRIVER_COUNT_MAX][LINE_MAX];
	
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
	
	//read list -> table
	
    int task_count=0;
    while(task_count < TASK_COUNT_MAX && fgets(task_table[task_count], LINE_MAX, task_file)) 
    {
        task_table[task_count][strcspn(task_table[task_count], "\n")] = '\0';  // Remplace newline
        task_count++;
    }
	if(task_count==0){printf("error : no task\n"); exit(0);}
	
	int driver_count=0;
    while(driver_count < DRIVER_COUNT_MAX && fgets(driver_table[driver_count], LINE_MAX, driver_file)) 
    {
        driver_table[driver_count][strcspn(driver_table[driver_count], "\n")] = '\0';  // Remplace newline
        driver_count++;
    }
	if(driver_count==0){printf("error : no driver\n"); exit(0);}

	// close files
	fclose(task_file);
	fclose(driver_file);
		
	
	// print tables
	int i;
	
	printf("find task :\n");
	for(i=0;i<task_count;i++)
	{
		printf("\ttask[%i]=%s\n",i, task_table[i]);
	}
	
	printf("\nfind driver :\n");
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
	int flag_tag=0;
	
	while (fgets(line, LINE_MAX, file_src)) 
	{
		get_argv();
		
		//if(argv[0][0]!=0) {printf(" <%s> <%s> <%s> <%s>\n",argv[0],argv[1],argv[2],argv[3]);}
		
		if ( !(strcmp(argv[0], "//")) && !(strcmp(argv[1],"[tag]")) ){printf("found tag <%s> <%s>\n",argv[2],argv[3]);} 	
		if ( !(strcmp(argv[0], "//")) && !(strcmp(argv[1],"[/tag]")) ){printf("found end tag\n");} 	
	
	}
	
	
	fclose(file_src);
	fclose(file_tmp);
	
	return 0;
}


void get_argv()
{
	int argn;
	int i_line=0;
	int i_arg;
	
	// reset all argv
	for(argn=0;argn<ARGV_MAX;argn++){argv[argn][0]=0;}

	// get off space or tab
	while( ((line[i_line]== ' ') || (line[i_line]== '\t')) && (line[i_line]!= '\n')){i_line++;}
	
	// commemt detected ?
	if( (line[i_line]=='/')&&(line[i_line+1]=='/') )
	{
		argv[0][0]='/';argv[0][1]='/';argv[0][2]=0;
		
		i_line +=2;
		
		for(argn=1;argn<ARGV_MAX;argn++)
		{
			// get off space or tab
			while( ((line[i_line]== ' ') || (line[i_line]== '\t')) && (line[i_line]!= '\n'))
				{i_line++;}
		
			// get argx
			i_arg=0;
			while( (line[i_line]!= ' ')  && (line[i_line]!= '\n') )
			{
				argv[argn][i_arg++]=line[i_line++];
			}
			argv[argn][i_arg]=0;
		}
	}
}
