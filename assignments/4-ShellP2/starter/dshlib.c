#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <unistd.h>
#include <linux/limits.h>

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
	if(str == NULL || start_index > strlen(str)){ return ERR_MEMORY; }
	memmove(str, str + start_index, new_len);
	str[new_len] = '\0';
	return OK;
}

int add_argv(char *cmd_line, cmd_buff_t *cmd_buff, int start_index, int end_index)
{

	if(cmd_buff->argc >= CMD_ARGV_MAX)
	{
		return ERR_CMD_OR_ARGS_TOO_BIG;
	}
	int length = end_index - start_index;
	if(length >= ARG_MAX){
		return ERR_CMD_OR_ARGS_TOO_BIG;
	}
	cmd_buff->argv[cmd_buff->argc] = malloc((length + 1) * sizeof(char));
	if(cmd_buff->argv[cmd_buff->argc] == NULL) { return ERR_MEMORY;}
	strncpy(cmd_buff->argv[cmd_buff->argc], cmd_line + start_index, length);
	cmd_buff->argc++;
	return OK;
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


int add_cmd_buffer(cmd_buff_t *cmd_buff)
{
	int buffer_length = 0;
	for(int i = 0; i < cmd_buff->argc; i++)
	{	
		buffer_length += strlen(cmd_buff->argv[i]) + 1; // plus one for spaces
	}
	cmd_buff->_cmd_buffer = malloc((buffer_length + 1) * sizeof(char));
	if(cmd_buff->_cmd_buffer == NULL){ return ERR_MEMORY; }
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		strcat(cmd_buff->_cmd_buffer, cmd_buff->argv[i]);
		if( i == cmd_buff->argc - 1)
		{
			cmd_buff->_cmd_buffer[buffer_length - 1] = '\0'; //last arg added, add null terminator
		}else{
			strcat(cmd_buff->_cmd_buffer, " "); //not last arg added, add space
		}
	}
}

int free_cmd_buff_and_args(cmd_buff_t *cmd_buff)
{
	free(cmd_buff->_cmd_buffer);
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		free(cmd_buff->argv[i]);
	}
	
}

Built_In_Cmds exec_bic(cmd_buff_t *cmd_buff, Built_In_Cmds cmd)
{
	if(cmd == BI_CMD_CD)
	{
		if(cmd_buff->argc > 2){ return ERR_CMD_ARGS_BAD;}
                if(cmd_buff->argc == 2){ //do nothing if less than 2
                        chdir(cmd_buff->argv[1]); //TODO: see any errors this may produce
			return BI_EXECUTED;
                }	
	}
	if(cmd == BI_CMD_EXIT) {return BI_CMD_EXIT; }
	if(cmd == BI_CMD_DRAGON){  } //TODO; implement dragon
	if(cmd == BI_RC) { }// TODO: implement rc
}

Built_In_Cmds exec_non_bic(cmd_buff_t *cmd)
{
	int f_result, c_result;
	f_result = fork();
    	if (f_result < 0){
        	perror("fork error");
        	return ERR_EXEC_CMD;
    	}

    	if (f_result == 0){
        	int rc;

        	rc = execvp(cmd->argv[0], cmd->argv);
        	if (rc < 0){
            		perror("fork error");
            		return ERR_EXEC_CMD;
        	}
	}else{

		wait(&c_result);
		int child_exit_code = WEXITSTATUS(c_result);
	}

	return BI_EXECUTED;

}



Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, EXIT_CMD) == 0) { return BI_CMD_EXIT;}
    if (strcmp(input, "cd") == 0) {return BI_CMD_CD; }
    if (strcmp(input, "dragon") == 0) {return BI_CMD_DRAGON;}
    if (strcmp(input, "rc") == 0) {return BI_RC;}
    return BI_NOT_BI;
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
    if(cmd == NULL) { return ERR_MEMORY; }
    cmd->argc = 0;
    char *cmd_buff= malloc(sizeof(char) * SH_CMD_MAX);
    if(cmd_buff == NULL) { return ERR_MEMORY; }
    int rc = 0;

  
    while(1)
    {
	printf("%s", SH_PROMPT);
	if(fgets(cmd_buff, ARG_MAX, stdin) == NULL)
	{
		printf("\n");
		break;
	}
	cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
	if(strcmp(cmd_buff, "") == 0){ printf(CMD_WARN_NO_CMD); continue;}
	
	remove_whitespace(cmd_buff);
	if(strlen(cmd_buff) == 0){ printf(CMD_WARN_NO_CMD); continue; }
	rc = build_cmd_buff(cmd_buff, cmd);
	add_cmd_buffer(cmd); //TODO: only do this if cmd was populated successfully
	Built_In_Cmds bic = match_command(cmd->argv[0]);
	
	if(bic == BI_CMD_EXIT){ break; }
	if(bic != BI_NOT_BI)
	{
		exec_bic(cmd, bic);
	}else{
		exec_non_bic(cmd);
	}
	print_cmd_buff(cmd);
	cmd->argc = 0;
	

	if(rc == ERR_TOO_MANY_COMMANDS)
	{ 
		printf(CMD_ERR_PIPE_LIMIT, CMD_MAX); 
		continue;	
	}else if(rc == WARN_NO_CMDS){ 
		printf(CMD_WARN_NO_CMD); 
		continue;
	}
        	
    }
    return OK;
    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
	free_cmd_buff_and_args(cmd);
	free(cmd);
	free(cmd_buff);
}
