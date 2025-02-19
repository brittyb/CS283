1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork creates a new process while preserving the parent process, so the command and the parent process are run as seperate processes. Just using execvp replaces the calling process’s address space with a new program.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails f_result will be < 0 and ERR_EXEC_CMD is returned. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  It finds the command to execute by searching for the command in the directories specified by the PATH environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() is called in the parent process so that it can wait for the exit status of the child. Without it the parent process might go on without having information about the child process that it needs. The child process still takes up system resources even after it has terminated if wait() is not called in the parent process.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() returns any error codes returned from wait(). This is important because it can help with identifying errors and debugging.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  If build_cmd_buff() encounters double quotes after whitespace it checks to see if there are matching double quotes and considers everything inside the double quotes as an argument (including spaces). This is necessary because for some commands like echo you may want to have an argument with spaces that needs to be read as one argument.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Rather than using command_list_t I switched over to the new struct cmd_buff_t. One part I found challenging was figuring out how to use my old remove_whitespace function. This function was very useful for seperating commands by pipes, but less effective at seperating by arguments. I could not always use this old function because it only got rid of leading and trailing space. I also had to consider double quote arguments. My old remove_whitespace function found the index of the first non whitespace character and index of the last non-whitespace character to remove leading and trailing spaces. In this assignment I still used this function before parsing individual arguments. For individual arguments I used strcspn and strspn to remove extra whitespace that is not leading or trailing.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals interrupt a process and force it to handle the signal. Signals can be handled in many different ways. They can be ignored, deferred to a user supplied process, or a kernel default action can be applied. They allow us to pause or terminate a process.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:
    >SIGTERM is a polite request to terminate a process that can be ignored or deferred. This can be useful if a program has things it needs to clean up before terminating.
    > SIGKILL terminates a process immediately and cannot be ignored. If SIGTERM does not work or a program needs to be ended immediately this should be used.
    > SIGINT is similar to SIGTERM, but it instead is called with CTRL + C on the keyboard. This is useful when your program is taking too long and you want to end it early.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP allows you to pause a process without terminating it. It cannot be caught or ignored like SIGINT. This is because it gives a way to reliably pause a process that works every time when needed.
