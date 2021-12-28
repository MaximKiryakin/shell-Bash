#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


enum analise_tokens {COMMAND, L_BRACKET, COMMAND_LIST, EOI, R_BRACKET,
    AND, PIPE, BGRND, NAME, FLOWIN, FLOWOUT, APPEND, SIMUL};

struct token{
    enum analise_tokens token;    
    char* name;          

};

struct node{
    char ** argv;         //list of command name and arguments
    char *infile;         //reassigned stdin file
    char *outfile;        //reassigned stdout file
    int backgnd;          // =1, if running in the background
    struct node *psubcmd; //command to run in child shell
    struct node *pipe;    //command after |
    struct node *next;    //the next after";"
    int check_flag;       //= 1,if the previous command in && succeeded
    int append_flag;      // =1, if you need to add to the end of the file
    int wait_flag;        //=1, if you need to wait for completion
};
