#include "solution.h"
#include"tree_exec.h"

void memory_feed(struct node* my_struct){
    if (my_struct == NULL)
        return;
    if (my_struct->argv != NULL){
        int i = 0;
        while (my_struct->argv[i] != NULL){
            free(my_struct->argv[i]);
            i++;
        }
        free(my_struct->argv);
        my_struct->argv = NULL;
    }
    if (my_struct->infile != NULL){
        free(my_struct->infile);
        my_struct->infile = NULL;
    }
    if (my_struct->outfile != NULL){
        free(my_struct->outfile);
        my_struct->outfile = NULL;
    }
    if (my_struct->next!= NULL){
        memory_feed(my_struct->next);
        my_struct->next = NULL;

    }
    if (my_struct->pipe!= NULL){
        memory_feed(my_struct->pipe);
        my_struct->pipe = NULL;
    }
    if (my_struct->psubcmd!= NULL){
        memory_feed(my_struct->psubcmd);
        my_struct->psubcmd = NULL;
    }

    free(my_struct);
    my_struct = NULL;

}


int tree_process (struct node* my_struct,  struct node* root){

    struct node* current_struct = my_struct;
    int fd[2];
    pid_t pipe_son_pid_1, pipe_son_pid_2;
    int data = 0, first_iter_flag = 1;
    pid_t last_process, last_process_1 ;
    int process_res = 0;
    //main loop
    while(1) {
        //pipe check
        if (current_struct->pipe != NULL) {
            struct node *temp_struct = current_struct;
            int mas_size = 1, len = 0;
            int* pid_mas = (int*)malloc(sizeof(int));
            //main conveyor loop
            while (1) {

                pipe(fd);
                if (!(pipe_son_pid_1 = fork())) {
                    
                    free(pid_mas);

                    if (!first_iter_flag) {
                        dup2(data, 0);
                    }else{

                        if (current_struct->infile != NULL) {
                            int read_file = open(current_struct->infile, O_RDONLY);
                            if(read_file == -1) {
                                memory_feed(root);
                                exit(1);
                            }
                            dup2(read_file, 0);
                        }
                    }

                    if (current_struct->pipe != NULL) {
                        dup2(fd[1], 1);
                    }else{

                        if (current_struct->append_flag == 1 && current_struct->outfile != NULL) {
                            int write_file = open(current_struct->outfile,  O_WRONLY | O_CREAT | O_APPEND, 0666);
                            dup2(write_file, 1);
                        }else {
                            if (current_struct->outfile != NULL) {
                                int write_file = open(current_struct->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                dup2(write_file, 1);
                            }
                        }
                    }

                    close(fd[1]);
                    close(fd[0]);

                    if (current_struct->psubcmd != NULL){

                        process_res = tree_process(current_struct->psubcmd, root);
                        memory_feed(root);
                        exit(process_res);

                    }else {
                        execvp(current_struct->argv[0], current_struct->argv);
                    }
                }

                len++;
                if (len >= mas_size){
                    mas_size *= 2;
                    pid_mas = (int*)realloc(pid_mas, mas_size*sizeof(int));
                }
                pid_mas[len - 1] = pipe_son_pid_1;

                if (!first_iter_flag) {
                    close(data);
                }
                data = fd[0];
                close(fd[1]);
                first_iter_flag = 0;

                if (current_struct->pipe == NULL) {
                    break;
                }
                current_struct = current_struct->pipe;
            }

            current_struct = temp_struct;

            if (current_struct->check_flag == 1){

                int status1;
                waitpid(pipe_son_pid_1, &status1, 0);

                if (WIFEXITED(status1) && WEXITSTATUS(status1) == 0) {

                    current_struct = current_struct->next;

                    for(int j = 0; j < len; j++){
                        waitpid(pid_mas[j], &status1, 0);
                    }
                    free(pid_mas);

                    continue;
                }else{

                    free(pid_mas);
                    while(current_struct->check_flag == 1){
                        current_struct = current_struct-> next;
                    }
                    continue;
                }
            }else{
                free(pid_mas);
            }

        }
        else {

            if (current_struct->psubcmd == NULL) {
                if (!(last_process = fork())) {

                    if (current_struct->append_flag == 1 && current_struct->outfile != NULL) {
                        int write_file = open(current_struct->outfile,  O_WRONLY | O_CREAT | O_APPEND, 0666);
                        dup2(write_file, 1);
                    }else {
                        if (current_struct->outfile != NULL) {
                            int write_file = open(current_struct->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                            dup2(write_file, 1);
                        }
                    }

                    if (current_struct->infile != NULL) {
                        int read_file = open(current_struct->infile, O_RDONLY);
                        if(read_file == -1) {
                            memory_feed(root);
                            exit(1);
                        }
                        dup2(read_file, 0);
                    }
                    execvp(current_struct->argv[0], current_struct->argv);
                    exit(1);
                }
                if(!current_struct->backgnd ){
                    waitpid(last_process, &process_res, 0);
                }
            } else {
                if (!(last_process_1 = fork())) {

                    if (current_struct->append_flag == 1 && current_struct->outfile != NULL) {
                        int write_file = open(current_struct->outfile,  O_WRONLY | O_CREAT | O_APPEND, 0666);
                        dup2(write_file, 1);
                    }else {
                        if (current_struct->outfile != NULL) {
                            int write_file = open(current_struct->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                            dup2(write_file, 1);
                        }
                    }

                    if (current_struct->infile != NULL) {
                        int read_file = open(current_struct->infile, O_RDONLY);
                        if(read_file == -1) {
                            memory_feed(root);
                            exit(1);
                        }
                        dup2(read_file, 0);
                    }

                    int status;
                    process_res = tree_process(current_struct->psubcmd, root);
                    memory_feed(root);

                    exit(process_res);
                }
                if(!current_struct->backgnd ){
                    waitpid(last_process_1, &process_res, 0);
                }
            }
        }

        //checking that it is a conjunction
        if (current_struct->check_flag == 1){
            if (!(WIFEXITED(process_res) && WEXITSTATUS(process_res) == 0)) {
                break;
            }

        }
        if (current_struct-> next == NULL) {
            break;
        }

        current_struct = current_struct-> next;
    }

    while(wait(NULL) != -1);

    if (WIFEXITED(process_res)) {
        return WEXITSTATUS(process_res);
    }
    if (WIFSIGNALED(process_res)) {
        return 128 + WTERMSIG(process_res);
    }
}