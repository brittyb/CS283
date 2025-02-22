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
#include <errno.h>
//This function is for debugging purposes
void print_cmd_buff(cmd_buff_t *cmd_buff)
{
	if(cmd_buff == NULL){
		printf("cmd_buff is null\n");
		return;
	}
	printf("argc: %d\n", cmd_buff->argc);
	printf("_cmd_buffer: %s\n", cmd_buff->_cmd_buffer);
	for(int i = 0; i < 9; i++)
	{
		printf("arg %d: %s\n", (i+1), cmd_buff->argv[i]);
	}
}

// removed leading and trailing whitespace
int remove_whitespace(char *str)
{
	int start_index = 0;
	int end_index = strlen(str) - 1;
	
	while(str[start_index] == SPACE_CHAR)
	{
		start_index++;
	}

	while(str[end_index] == SPACE_CHAR)
	{
		end_index--;
	}
	int new_len = end_index + 1 - start_index;
	if(str == NULL || start_index > (int)strlen(str)){ return ERR_MEMORY; }
	memmove(str, str + start_index, new_len);
	str[new_len] = '\0';
	return OK;
}

// add an individual argument to cmd_buff given its start and end index in the cmd_line
int add_argv(char *cmd_line, cmd_buff_t *cmd_buff, int start_index, int end_index)
{

	if(cmd_buff->argc >= CMD_ARGV_MAX)
	{
		return ERR_CMD_ARGS_BAD;
	}
	int length = end_index - start_index;

	if(length >= 256){
		return ERR_CMD_OR_ARGS_TOO_BIG;
	}
	cmd_buff->argv[cmd_buff->argc] = malloc((length + 1) * sizeof(char));
	if(cmd_buff->argv[cmd_buff->argc] == NULL) { return ERR_MEMORY;}
	strncpy(cmd_buff->argv[cmd_buff->argc], cmd_line + start_index, length);
	cmd_buff->argv[cmd_buff->argc][length] = '\0';
	cmd_buff->argc++;
	return OK;
}

// get the arguments and add them to cmd_buff (being mindful of double quotes)
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
	int start_position = 0;
	int end_position = 0;
	int position = strspn(cmd_line, " ");
	int rc = OK;	
	while(position < (int)strlen(cmd_line))
	{
		start_position = position;
	        if(cmd_line[start_position] == '"')
		{
			position++;
			end_position = position + 1 + strcspn(cmd_line + position, "\"");
		}else{
			end_position = position + strcspn(cmd_line + position, " ");
		}	
		rc = add_argv(cmd_line, cmd_buff, start_position, end_position);
		if(rc != OK){ break;}	
		position = end_position + strspn(cmd_line + end_position, " ");
		
	}	   
       return rc;
}

//After getting the arguments, combine them to create a new buffer with no extra spaces
int alloc_cmd_buffer(cmd_buff_t *cmd_buff)
{
	int buffer_length = 0;
	for(int i = 0; i < cmd_buff->argc; i++)
	{	
		buffer_length += strlen(cmd_buff->argv[i]) + 1; // plus one for spaces
	}
	cmd_buff->_cmd_buffer = malloc((buffer_length + 1) * sizeof(char));
	if(cmd_buff->_cmd_buffer == NULL){ return ERR_MEMORY; }
	cmd_buff->_cmd_buffer[0] = '\0';
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
	return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
	free(cmd_buff->_cmd_buffer);
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		free(cmd_buff->argv[i]);
	}
	free(cmd_buff);
	return 0;	
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd_buff, Built_In_Cmds cmd, int *previous_rc)
{
	if(cmd == BI_CMD_CD)
	{
		if(cmd_buff->argc > 2){ *previous_rc = ERR_CMD_ARGS_BAD;
		}else if(cmd_buff->argc == 1){ *previous_rc = OK;  
		}else if(cmd_buff->argc == 2){
                        if(chdir(cmd_buff->argv[1]) == 0)
			{
				*previous_rc = OK;
			}else{
				*previous_rc = ERR_CMD_ARGS_BAD;
			}
                }else{
			*previous_rc = ERR_CMD_ARGS_BAD;
		
		}
		return BI_EXECUTED;	
	}
	if(cmd == BI_CMD_EXIT) {return BI_CMD_EXIT; }
	if(cmd == BI_CMD_DRAGON){ print_dragon(); } 
	if(cmd == BI_RC) {printf("%d\n", *previous_rc); return BI_EXECUTED; }
	return BI_NOT_BI;
}

Built_In_Cmds exec_non_bic(cmd_buff_t *cmd, int *previous_rc)
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
			rc = errno;
            		if(rc == ENOENT) { printf("Command not found in PATH\n");}
                	if(rc == EACCES) { printf("Permission denied\n");}
                	if(rc == ENOMEM) { printf("No memory\n");}
                	if(rc == EMFILE) { printf("Too many files open\n");}	
			exit(rc);
        	}
	}else{

		wait(&c_result);
		int exit_code = WEXITSTATUS(c_result);
		*previous_rc = exit_code;
		
	}

	return BI_EXECUTED;

}



Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, EXIT_CMD) == 0) { return BI_CMD_EXIT;}
    if (strcmp(input, "cd") == 0) { return BI_CMD_CD; }
    if (strcmp(input, "dragon") == 0) {return BI_CMD_DRAGON;}
    if (strcmp(input, "rc") == 0) {return BI_RC;}
    return BI_NOT_BI;
}

// This function clears existing arguments and sets argc to 0 so that cmd_buff can have new values when the next command is given
int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
	for (int i = 0; i < cmd_buff->argc; i++) {
		free(cmd_buff->argv[i]);
		cmd_buff->argv[i] = NULL;
		
	}
	
	cmd_buff->argc = 0;
	if (cmd_buff->_cmd_buffer) {
		cmd_buff->_cmd_buffer[0] = '\0';
	}
	return OK;
}

// This function removes quotes from arguments before adding it to the _cmd_buffer
void remove_quotes(cmd_buff_t *cmd_buff) 
{
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		int len = strlen(cmd_buff->argv[i]);
    		if (len >= 2 && cmd_buff->argv[i][0] == '"' && cmd_buff->argv[i][len - 1] == '"') 
		{
        		memmove(cmd_buff->argv[i], cmd_buff->argv[i] + 1, len - 2);
        		cmd_buff->argv[i][len - 2] = '\0';
    		}
	}
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
    int previous_rc = 0; 
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
	if(strlen(cmd_buff) == 0){ printf(CMD_WARN_NO_CMD); rc = WARN_NO_CMDS; continue; }
	rc = build_cmd_buff(cmd_buff, cmd);
	if(rc != OK) { previous_rc = rc; clear_cmd_buff(cmd); continue; }
 	remove_quotes(cmd); //if one of the args has quotes remove them in the buffer	
	rc = alloc_cmd_buffer(cmd); //populate _cmd_buffer
	if(rc != OK) {previous_rc = rc; clear_cmd_buff(cmd); continue; }
	//print_cmd_buff(cmd);
	
	Built_In_Cmds bic = match_command(cmd->argv[0]);	
	if(bic == BI_CMD_EXIT){ free_cmd_buff(cmd); break; }
	if(bic != BI_NOT_BI)
	{
		if(exec_built_in_cmd(cmd, bic, &previous_rc) == BI_NOT_BI){ exec_non_bic(cmd, &previous_rc);}

	}else{
		exec_non_bic(cmd, &previous_rc);
	}
	clear_cmd_buff(cmd);
        	
    }
    return OK;
  
}
