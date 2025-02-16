#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

void print_command(command_list_t clist)
{
	int i;
	for(i = 0; i < clist.num; i++)
	{
		printf("<%i>%s ", (i+1), clist.commands[i].exe);
		if(strlen(clist.commands[i].args) > 0)
		{
			printf("[%s]", clist.commands[i].args);
		}
		printf("\n");
	}
}

void print_dragon()
{
	printf("                                                                        @%%%%%%%%                        \n");
	printf("                                                                     %%%%%%%%%%%%                         \n");
    	printf("                                                                    %%%%%%%%%%%%                          \n");
    	printf("                                                                 %% %%%%%%%%%%%%%%           @              \n");
    	printf("                                                                %%%%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%           \n");
    	printf("                                       %%%%%%%%%%%%%%  %%%%%%%%@         %%%%%%%%%%%%%%%%%%%%%%%%@    %%%%%%%%%%%%  @%%%%%%%%        \n");
    	printf("                                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%          \n");
    	printf("                                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%           \n");
    	printf("                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%     %%%%%%            \n");
    	printf("                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%%            \n");
    	printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                \n");
   	printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              \n");
    	printf("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%@              \n");
    	printf("      %%%%%%%%%%%%%%%%@           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%                \n");
    	printf("    %%%%%%%%%%%%%%%%%%%%%%%%%%         %%%%@%%%%%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%      @%%                \n");
    	printf("  %%%%%%%%%%%%%%%%%%%%   %%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                        \n");
    	printf(" %%%%%%%%%%%%%%%%%%       %%         %%%%%%%%%%%%%%%%%%%%%%%%%%             %%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%                       \n");
   	printf("%%%%%%%%%%%%%%%%%%@                %% %%%%%%%%%%%%%%%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n");
    	printf("%%%%%%%%%%%%%%%%@                 %%%%@%%%%%%%%%%%%%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  \n");
    	printf("%%%%%%%%%%%%%%@                   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              \n");
    	printf("%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%  \n");
    	printf("%%%%%%%%%%%%%%%%%%@                   @%%%%%%%%%%%%%%%%%%%%%%%%%%%%         %%%%%%%%%%%%%%%%%%%%%%%%@ %%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%\n");
    	printf("%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%\n");
    	printf("%%%%%%%%%%%%%%%%%%@%%%%@                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%%%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%%\n");
    	printf(" %%%%%%%%%%%%%%%%%%%%                  %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%@        %%%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%\n");
    	printf("  %%%%%%%%%%%%%%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%%%% \n");
    	printf("   %%%%%%%%%%%%%%%%%%%%%%%%%% %%%%  %%  %%@ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%% \n");
    	printf("    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%%%%%%%%% \n");
    	printf("     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%%%%   \n");
    	printf("      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%               \n");
    	printf("        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%%%%%%%%%%%%          \n");
    	printf("           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  @%%%%%%%%%%%%%%%%%%         \n");
    	printf("              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%           @%%@%%                  @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%        \n");
    	printf("                  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%         \n");
    	printf("                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%%%%%%%%%%            \n");
    	printf("                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%%%%%% %%%%%%                      %%%%%%%%%%%%%%%%%%%%  %%%%%%@          \n");
    	printf("                     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%% %%%%                          %%%%%%%%%%%%%%%%%%%%%%%%%%@          \n");    
    	printf("                                                                                 %%%%%%%%%%%%%%@        \n");
}

int remove_whitespace(char *str)
{
	int start_index = 0;
	int end_index = strlen(str) - 1;
	int current_index = 0;
	
	while(str[start_index] == SPACE_CHAR)
	{
		start_index++;
	}

	while(str[end_index] == SPACE_CHAR)
	{
		end_index--;
	}
	int new_len = end_index + 1 - start_index;

	memmove(str, str + start_index, new_len);
	str[new_len] = '\0';
	return 0;
}

void get_exe_and_args(char *cmd, command_list_t *clist)
{
	char *saveptr;
	char *exe_token = strtok_r(cmd, " ", &saveptr);
	char *arg_token = strtok_r(NULL, "", &saveptr);
	 
	//printf("exe: %s args: %s\n", exe_token, arg_token);
	strcpy(clist->commands[clist->num].exe, exe_token);
	if(arg_token != NULL)
	{
		strcpy(clist->commands[clist->num].args, arg_token);
	}
	
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    memset(clist, 0, sizeof(command_list_t));
    char * cmd_token = strtok(cmd_line, PIPE_STRING);
    while(cmd_token != NULL)
    {
	
	//printf("cmd token %s\n", cmd_token);
	
	if(clist->num >= CMD_MAX){
		//printf("Too many commands\n");
		return ERR_TOO_MANY_COMMANDS;
	}
	//printf("Before removing whitespace: %s\n", cmd_token);
	remove_whitespace(cmd_token);	
	//printf("After removing whitespace: %s\n", cmd_token);
	if(strlen(cmd_token) == 0 || strlen(cmd_line) == 0){
		return WARN_NO_CMDS;
	}	
	get_exe_and_args(cmd_token, clist);
	if(clist->num >= CMD_MAX)
	{
		return ERR_TOO_MANY_COMMANDS;
	}	
	clist->num++;
		
	
	cmd_token = strtok(NULL, PIPE_STRING);
	
    }
/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    //char *cmd_buff;
    //int rc = 0;
    //cmd_buff_t cmd;
    char *cmd_buff= malloc(sizeof(char) * SH_CMD_MAX);
    int rc = 0;
    command_list_t clist;
    while(1)
    {
	printf("%s", SH_PROMPT);
	if(fgets(cmd_buff, ARG_MAX, stdin) == NULL)
	{
		printf("\n");
		break;
	}
	cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
	if(strcmp(cmd_buff, "") == 0)
	{
		printf(CMD_WARN_NO_CMD);
		continue;
	}
	if(strcmp(cmd_buff, EXIT_CMD) == 0) 
	{
		
		exit(0);
	}
	if(strcmp(cmd_buff, "dragon") == 0)
	{
		print_dragon();
		continue;
	}
	rc = build_cmd_list(cmd_buff, &clist);
	if(rc == ERR_TOO_MANY_COMMANDS)
	{
		printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
		continue;
	
	}else if(rc == WARN_NO_CMDS){
		printf(CMD_WARN_NO_CMD);
		continue;
	}
        printf(CMD_OK_HEADER, clist.num);
	print_command(clist);	
    }
    free(cmd_buff);
    return OK;
    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}
