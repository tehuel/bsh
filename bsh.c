#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wordexp.h>
#include <signal.h>

#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

const char* BUILT_IN[] = {"","exit","mycd","mypwd"};

int handle_input(char*, wordexp_t*);
void free_input(char*, wordexp_t*);
void exec_builtin(int, char*[]);
void exec_cmd(char*[]);
int isbuiltin(char*[]);
void signal_handler(int sig);

char* prompt[255];

int main(int argc, char* argv[])
{
    char* input_str = NULL;
    int cmd = 0;
    wordexp_t cmd_argv;
    
    signal( SIGINT, SIG_IGN );
    
    while(1)
    {
        if(!handle_input(input_str, &cmd_argv)) 
        {
            free_input(input_str, &cmd_argv);
            continue;
        }

        if ((cmd = isbuiltin(cmd_argv.we_wordv)))
            exec_builtin(cmd, cmd_argv.we_wordv);   // exec built_in command
        else
            exec_cmd(cmd_argv.we_wordv);            // exec command 

        free_input(input_str, &cmd_argv);
    }
}

// Handles command line input.
// Prints the prompt and waits for user's input returning 0
// if no command was issued, or 1 otherwise with cmd_argv 
// holding the parsed command line
int handle_input(char* input_str, wordexp_t* cmd_argv)
{
    
    // initializes the prompt
    getcwd(prompt, 255);
    strcat(prompt, " # ");
    
    
    if(!(input_str = readline(prompt)))     // command input
    {
        printf("exit\n");
        exit(0);
    }

    int error = wordexp(input_str, cmd_argv, WRDE_SHOWERR | WRDE_UNDEF);
    
    if(error)
    {
        switch (error) {
            case WRDE_BADCHAR: 
                printf("Bad character. Illegal occurrence of newline or one of |, &, ;, <, >, (, ), {, }\n");
                break;
            case WRDE_BADVAL:
                printf("Undefined variable\n");
                break;
            case WRDE_SYNTAX:
                printf("Syntax error\n");
                break;
            default:
                printf("Unknown error\n");
        } 

         return 0;
    }

    if (cmd_argv->we_wordc == 0)            // no input
    {
        return 0;
    }

    add_history(input_str);

    return 1;
}
void free_input(char* cmd_str, wordexp_t* cmd_argv)
{
    free(cmd_str);
    wordfree(cmd_argv);
}

// execute given command 
void exec_cmd(char* cmd_argv[])
{
    int status;
    int child_pid = fork();

    if (child_pid != 0)                               // parent process
    {
        waitpid(child_pid, &status, 0);
    }
    else 
    {
        execvp(cmd_argv[0], cmd_argv);               // execute PROGRAM            
        perror("");
        exit(0);
    }
}

// handle built in commands
void exec_builtin(int cmd, char* cmd_argv[])
{
    char path[255];

    switch(cmd)
    {
        case 1:
            exit(0);
            break;
        case 2:
            chdir(cmd_argv[1]);
            break;
        case 3:
            getcwd(path, 255);
            printf("%s\n", path);
            break;
        default:
            printf("-bsh: command not found\n");
    }

    return;
}

// utility functions
int isbuiltin(char* cmd_argv[])
{
    for(int i = 1; i < NELEMS(BUILT_IN); i++)
        if (!strcmp(cmd_argv[0], BUILT_IN[i])) 
            return i;
    
    return 0;
}
