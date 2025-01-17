
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SZ 50

#define SPACE_CHAR ' '

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
int  reverse_string(char*, int, int);
int  word_print(char*, int, int);
int  word_replace(char*, int, int, char*, char*);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int i = 0;
    int processed_length = 0;
    int buff_index = 0;
    char current_char = user_str[i];
    char last_char = ' ';
    while(current_char != '\0'){
	if(buff_index > len - 1){
		return -1;
	}
	if(last_char != ' ' && last_char != '\t'){
		buff[buff_index] = current_char;
		buff_index++;
		processed_length++;

	}else{
		if(current_char != ' ' && current_char != '\t'){
			buff[buff_index] = current_char;
			buff_index++;
			processed_length++;
		}
	}
	last_char = current_char;
	i++;
	current_char = user_str[i];
    }
    if(buff_index < len){
	if(buff[buff_index - 1] == ' ' || buff[buff_index - 1] == '\t'){
		buff[buff_index - 1] = '.';
	}
	for(buff_index; buff_index < len; buff_index++){
		buff[buff_index] = '.';
	}
    }
    return processed_length;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]");
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int wc = 0;
    int word_start = 0;
    for(int i = 0; i < str_len; i++){
	
	if(word_start == 0){
		if(buff[i] == SPACE_CHAR){
			continue;	
		}else{
			word_start = 1;
			wc++;
		}
	}else{
		if(buff[i] == SPACE_CHAR){
			word_start = 0;
		}
	}
    }
    

    return wc;
}

int reverse_string(char *buff, int len, int str_len){
    int end_idx = str_len - 1;       
    int start_idx = 0;
    char tmp_char;

    
	while(end_idx > start_idx){
		tmp_char = buff[start_idx];
		buff[start_idx] = buff[end_idx];
		buff[end_idx] = tmp_char;
		start_idx++;
		end_idx--;
	}
    
    return 0;
}

int word_print(char *buff, int len, int str_len){
    int last_char_idx = str_len - 1;
    int wc = 0;
    int wlen = 0;
    bool word_start = false;
   printf("Word Print\n----------\n"); 
	for(int i = 0; i < str_len; i++){
		if(word_start == false){
			if(buff[i] == SPACE_CHAR){
				printf(" (%d)\n", wlen);
				wlen = 0;
				continue;
			}else{
				word_start = true;
				wc++;
				printf("%d. ", wc);
				printf("%c", buff[i]);
				wlen++;
			}
		}else{
			if(buff[i] == SPACE_CHAR){
				word_start = false;
				printf("(%d)\n", wlen);
				wlen = 0;
			}else{
				printf("%c", buff[i]);
				wlen++;
			}
		}
		if(i == last_char_idx){
			printf("(%d)\n", wlen);
		}
	}
	printf("\nNumber of words returned: %d\n", wc);
	return 0;
}

int word_replace(char *buff, int len, int str_len, char *word_to_replace, char *new_word){
	
	int old_word_length = 0;
	while (word_to_replace[old_word_length] != '\0') {
        	old_word_length++;
    	}
	int new_word_length = 0;
	while (new_word[new_word_length] != '\0') {
                new_word_length++;
        }
	
	int word_index = -1;
	for(int i = 0; i < str_len; i++){
		if(i + new_word_length < str_len){
			for(int j = 0; j < old_word_length; j++){
				if(buff[j + i] != word_to_replace[j]){
					break;
				}
				if(j == old_word_length - 1){
					word_index = i;
					break;
				}
			}	
		}
		
	
	}

	if(word_index == -1){
		return -1;
	}
       	int current_char_to_copy;
       	char temp_buff[len];
	memcpy(temp_buff, buff, len);
	int new_str_len = str_len - old_word_length + new_word_length;
	if(new_str_len > len){
		new_str_len = len;
	}
	int current_new_len = 0;
        for(int i = 0; i < len; i++){
		if(i == word_index){
			for(int j = 0; j < new_word_length; j++){
				if(i+j < len){
					
					buff[i+j] = new_word[j];
					current_new_len++;
					
				}
			}
			i += old_word_length;
		}
		buff[current_new_len] = temp_buff[i];
		current_new_len++;

	}
	while(current_new_len <= len){
		buff[current_new_len - 1] = '.';
		current_new_len++;
	}	

	return 0;
	
	
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*	
     *		This is safe because if argc < 2 the if statement is
     *		executed without checking the second statement in the or
     */


    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    /*
     * 	The if statement checks that the correct number of arguments 
     * 	is given
     */

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if(buff == NULL){
	printf("Memory Allocation for buff Failed\n");
	exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }
    
    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
	 case 'r':
	    rc = reverse_string(buff, BUFFER_SZ, user_str_len);
	    break;

	  case 'w':
	    rc = word_print(buff, BUFFER_SZ, user_str_len);
	   
	    break;
	  case 'x':
	    if(argc < 5){
	    printf("Please enter a word to search for and a word you would like to replace it with\n");
	    exit(1);
	    }

	    rc = word_replace(buff, BUFFER_SZ, user_str_len, argv[3], argv[4]);
	    if(rc == -1){
		printf("Word not found\n");
		exit(3);
	    }
	    break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
