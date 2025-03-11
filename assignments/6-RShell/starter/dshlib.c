#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"
#include <errno.h>
/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
// This function is for debugging
void print_clist(command_list_t *clist)
{
	for(int i = 0; i < clist->num; i++)
	{
		print_cmd_buff(&clist->commands[i]);
	}
}

// removed leading and trailing whitespace
int remove_whitespace(char *str)
{
    if (str == NULL) {
        return ERR_MEMORY;
    }

    int len = strlen(str);
    if (len == 0) {
        return OK;
    }

    int start_index = 0;
    int end_index = len - 1;

    while (start_index <= end_index && str[start_index] == SPACE_CHAR) {
        start_index++;
    }

    while (end_index >= start_index && str[end_index] == SPACE_CHAR) {
        end_index--;
    }

    int new_len = end_index - start_index + 1;

    if (start_index > 0) {
        memmove(str, str + start_index, new_len);
    }
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
	if(length >= ARG_MAX){
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



void free_cmd_args(cmd_buff_t *cmd_buff)
{
	for(int i = 0; i < cmd_buff->argc; i++)
	{
		free(cmd_buff->argv[i]);
	}
}
int free_cmd_buff(cmd_buff_t *cmd_buff)
{
	if (cmd_buff == NULL){ return ERR_MEMORY;}
	free(cmd_buff->_cmd_buffer);
	cmd_buff->_cmd_buffer = NULL;
	free_cmd_args(cmd_buff);
	free(cmd_buff);
	cmd_buff = NULL;
	return OK;
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
	if(cmd == BI_CMD_DRAGON){ print_dragon(); return BI_EXECUTED;}
	if(cmd == BI_RC) {printf("%d\n", *previous_rc); return BI_EXECUTED; }
	return BI_NOT_BI;
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

int add_pipe_command(char *cmd, command_list_t *clist)
{
     int rc = OK;
     rc = build_cmd_buff(cmd, &clist->commands[clist->num]);
     if(rc != OK){ return rc; }
     rc = alloc_cmd_buffer(&clist->commands[clist->num]);
    return rc;
}

// modified to handle double pipes with no args in between
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    int rc = OK;
    char *start = cmd_line;
    char *end;

    while (*start) {
        end = strchr(start, PIPE_CHAR);

        if (!end) {
            end = start + strlen(start);
        }

        if (end == start) {
            return ERR_CMD_ARGS_BAD;
        }

        char saved_char = *end;
        *end = '\0';

        //printf("Before removing whitespace: [%s]\n", start);
        if(remove_whitespace(start) == WARN_NO_CMDS){ printf("hi\n");return ERR_CMD_ARGS_BAD; }
        //printf("After removing whitespace: [%s]\n", start);

        if (strlen(start) == 0) {
            *end = saved_char;
	    return ERR_CMD_ARGS_BAD;
        }

        if (clist->num >= CMD_MAX) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }

        rc = add_pipe_command(start, clist);
	if (rc != OK) {
            *end = saved_char;
            return rc;
        }
        clist->num++;

        *end = saved_char;
        start = end + 1;
    }

    //print_clist(clist);
    return rc;
}

void free_clist(command_list_t *clist)
{
	for(int i = 0; i < clist->num; i++)
	{
		free(clist->commands[i]._cmd_buffer);
		clist->commands[i]._cmd_buffer = NULL;
		free_cmd_args(&clist->commands[i]);
	}
	clist->num = 0;
	//print_clist(clist);
	free(clist);
}

int exec_pipe_commands(command_list_t *clist)
{
    int rc = OK;
    int num_cmds = clist->num;
    pid_t pids[num_cmds];
    int pipes[num_cmds - 1][2];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds; i++)
    {
	pids[i] = fork();
        if (pids[i] == -1)
	{
            perror("fork");
            exit(EXIT_FAILURE);
        }
	if (pids[i] == 0)
	{
            if (i > 0)
	    {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }


        	if (i < num_cmds - 1)
		{
            		dup2(pipes[i][1], STDOUT_FILENO);
        	}


		for (int j = 0; j < num_cmds - 1; j++)
		{
            		close(pipes[j][0]);
            		close(pipes[j][1]);
        	}

        	execvp(clist->commands[i].argv[0], clist->commands[i].argv);
        	perror("execvp");

		rc = ERR_CMD_ARGS_BAD;
        	exit(EXIT_FAILURE);

	}
    }
    for(int i = 0; i < num_cmds - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    //check if any children failed to update rc
    int status = 0;
    for (int i = 0; i < num_cmds; i++)
    {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
        {
            rc = ERR_CMD_ARGS_BAD;
        }
    }


    return rc;
}

int exec_cmd(char *cmd_buff, cmd_buff_t *cmd, int *previous_rc)
{
    int rc = 0;
    cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
    if(strcmp(cmd_buff, "") == 0){ printf(CMD_WARN_NO_CMD); return rc;}
    remove_whitespace(cmd_buff);
    if(strlen(cmd_buff) == 0){ printf(CMD_WARN_NO_CMD); rc = WARN_NO_CMDS; return rc; }
    rc = build_cmd_buff(cmd_buff, cmd);
    //print_cmd_buff(cmd);
    if(rc != OK) { *previous_rc = rc; free_cmd_buff(cmd);//clear_cmd_buff(cmd);
							 return rc; }
    remove_quotes(cmd); //if one of the args has quotes remove them in the buffer
    rc = alloc_cmd_buffer(cmd); //populate _cmd_buffer
    if(rc != OK) {*previous_rc = rc; free_cmd_buff(cmd);//clear_cmd_buff(cmd);
							return rc; }



    Built_In_Cmds bic = match_command(cmd->argv[0]);
    if(bic == BI_CMD_EXIT){ free_cmd_buff(cmd);
			    return OK_EXIT; }
    if(bic != BI_NOT_BI)
    {
           if(exec_built_in_cmd(cmd, bic, previous_rc) == BI_NOT_BI){ exec_non_bic(cmd, previous_rc);
                                                                       }
    }else{
            exec_non_bic(cmd, previous_rc);
    }
    free_cmd_buff(cmd);
    return OK;
}

int exec_local_cmd_loop(){
    char *cmd_buff= malloc(sizeof(char) * SH_CMD_MAX);
     int rc = 0;
     int previous_rc = 0;


      if(cmd_buff == NULL) { return ERR_MEMORY; }
      while(1)
      {

	printf("%s", SH_PROMPT);
	if(fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
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
	if(strchr(cmd_buff, PIPE_CHAR) == NULL)
	{
		cmd_buff_t *cmd = calloc(1, sizeof(cmd_buff_t));
        	if(cmd == NULL) { return ERR_MEMORY; }
        	cmd->argc = 0;
		rc = exec_cmd(cmd_buff, cmd, &previous_rc);

		if(rc == OK_EXIT){ free(cmd_buff);
				   exit(0);}
	}else{
		command_list_t *clist = calloc(1, sizeof(command_list_t));
            	clist->num = 0;
		rc = build_cmd_list(cmd_buff, clist);
		if(rc != OK){ previous_rc = rc; }else {rc = exec_pipe_commands(clist); }
		if(rc != OK){ previous_rc = rc;}
		free_clist(clist);
	}
      }
    free(cmd_buff);
    return OK;
}
