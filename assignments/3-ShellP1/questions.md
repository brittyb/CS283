1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets allows a maximum number of characters to be read. This means that we can restrict the input to never go over SH_CMD_MAX.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: using malloc() prevents stack overflow because it is stored on the heap instead. Using malloc also
    > allows for dynamic memory allocation and avoids wasted space.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  When checking what command is being executed extra spaces can interfere. "   cmd" is not the same as "cmd"

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 1.) echo < file.txt makes the output of echo the text in file.txt
    > 2.) ls > capture.txt direct the output of ls and writes it to capture.txt
    > 3.) ./error.sh 2> capture.txt redirects stderr to the file capture.txt
    > One challenge in our custom shell would be redirecting to/from files and making sure there are read/write permissions set.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Pipes are used to direct the output of one command to the input of another command.
    > Redirection is specifically for files/streams to change where the input comes from/ where the output goes.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Just because a program's output is expected/correct does not mean there are no errors. When piping it is also important to pass the output and not the error at times.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  The errors should be printed seperately from the output so that there is no confusion on what is an output and what is an error. Errors should be printed after all output has finished. We should not provide a way to merge them because keeping them seperate can help avoid confusion about what is and is not an error.
