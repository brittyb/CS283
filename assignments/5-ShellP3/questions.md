1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation uses waitpid() so that the parent process does not accept new input while waiting for all of the child processes to finish. Without calling waitpid() several things could happen. There could be a build up of processes consuming resources where a parent process is running at the same time as its child processes. If the parent is relying on the result of the child process this could interfere with the parent process. It could also interfere with the unfinished child process.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

File descriptors of pipes consume resources, and using too many of these resources can cause resource leaks. The reading process of a pipe may also never occur because it is waiting for EOF from the pipe that was left open.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

execvp() would execute cd as a child process and it would not change anything in the parent process after exiting.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

realloc() would need to be used to make room for more commands (or reduce the space to hold less commands). One trade-off would be increased runtime from frequent reallocation of memory. You would also need to consider system limits to avoid using too many resources.
