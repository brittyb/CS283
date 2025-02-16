#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

//This function is for debugging purposes
void print_cmd_buff(cmd_buff_t *cmd_buff)
{
	if(cmd_buff == NULL){
		printf("cmd_buff is null\n");
		return;
	}
	printf("argc: %d\n", cmd_buff->argc);
	printf("_cmd_buffer: %s\n", cmd_buff->_cmd_buffer);
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		printf("arg %d: %s\n", (i+1), cmd_buff->argv[i]);
	}
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

int add_argv(char *cmd_line, cmd_buff_t *cmd_buff, int start_index, int end_index)
{

	if(cmd_buff->argc >= CMD_ARGV_MAX)
	{
		//TODO: handle too many args
		printf("too many args\n");
		return -1;
	}
	int length = end_index - start_index;
	if(length >= ARG_MAX){
		//TODO: handle arg too long
		printf("arg too long\n");
		return -1;
	}
	cmd_buff->argv[cmd_buff->argc] = malloc((length + 1) * sizeof(char));
	//TODO: handle malloc errors
	strncpy(cmd_buff->argv[cmd_buff->argc], cmd_line + start_index, length);
	cmd_buff->argc++;
	return 1;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
	int start_position = 0;
	int end_position = 0;
	int position = strspn(cmd_line, " ");
        int count = 0;	
	while(position < strlen(cmd_line) && count < 10)
	{
		start_position = position;
	        if(cmd_line[start_position] == '"')
		{
			position++;
			end_position = position + 1 + strcspn(cmd_line + position, "\"");
		}else{
			end_position = position + strcspn(cmd_line + position, " ");
		}	
	
		add_argv(cmd_line, cmd_buff, start_position, end_position);
		position = end_position + strspn(cmd_line + end_position, " ");
		count++;
	}	    
}

int initialize_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
	cmd_buff->argc = 0;
	int length = strlen(cmd_line);
	cmd_buff->_cmd_buffer = malloc((length + 1) * sizeof(char));
	strcpy(cmd_buff->_cmd_buffer, cmd_line);
	return 1;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
	free(cmd_buff->_cmd_buffer);
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		free(cmd_buff->argv[i]);
	}
	free(cmd_buff);
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
    
    cmd_buff_t *cmd = malloc(sizeof(cmd_buff_t));
    char *cmd_buff= malloc(sizeof(char) * SH_CMD_MAX);
    int rc = 0;

    //TODO: see if there are any potential errors with memset
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
		//print_dragon();
		continue;
	}
	remove_whitespace(cmd_buff);
	initialize_cmd_buff(cmd_buff, cmd);
	rc = build_cmd_buff(cmd_buff, cmd);
	print_cmd_buff(cmd);
	if(rc == ERR_TOO_MANY_COMMANDS)
	{
		printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
		continue;
	
	}else if(rc == WARN_NO_CMDS){
		printf(CMD_WARN_NO_CMD);
		continue;
	}
        	
    }
    print_cmd_buff(cmd);
    free_cmd_buff(cmd);
    free(cmd_buff);
    return OK;
    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

}
