#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

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
    return 2;
}
