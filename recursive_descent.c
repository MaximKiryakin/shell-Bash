#include "solution.h"
#include "tree_exec.h"

void struct_init(struct node* my_struct){
    my_struct->argv = NULL;
    my_struct->infile = NULL;
    my_struct->outfile = NULL;
    my_struct->psubcmd = NULL;
    my_struct->pipe = NULL;
    my_struct->next = NULL;
    my_struct->wait_flag = 0;
    my_struct->append_flag = 0;
    my_struct->check_flag = 0;
    my_struct->backgnd = 0;
}

int command_list(struct node* my_struct,struct token*** pp_token_mas, int* token_mas_index);

int simple_command(struct node *my_struct,struct token*** pp_token_mas, int* token_mas_index ){
    struct token** p_token_mas = *pp_token_mas;
    int mas_name_size = 1;    //array size
    int index_mas_name = 0;   //array index 
    char** mas_name;
    mas_name = (char**)malloc(sizeof(char*));
    while (p_token_mas[*token_mas_index]->token == NAME){
        index_mas_name++;

        if (index_mas_name >= mas_name_size){
            mas_name_size *= 2;
            mas_name = (char**) realloc(mas_name, mas_name_size*sizeof(char*));
        }
        mas_name[index_mas_name - 1] = p_token_mas[*token_mas_index]->name;
        (*token_mas_index)++;
    }

    mas_name[index_mas_name] = NULL;

    my_struct->argv = mas_name;

    enum analise_tokens temp_token = p_token_mas[*token_mas_index]->token;
    if (temp_token != FLOWIN && temp_token != FLOWOUT && temp_token != APPEND){
        return 0;
    }
    
    if (temp_token == FLOWIN) {
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME) {
            return (1);
        }
        //connecting a file for reading
        my_struct->infile = p_token_mas[*token_mas_index - 1]->name;
        temp_token = p_token_mas[*token_mas_index]->token;
        if (temp_token != FLOWOUT && temp_token != APPEND ){
            exit(1);
        }
        if (temp_token == FLOWOUT){
            (*token_mas_index)++;
            temp_token = p_token_mas[(*token_mas_index)++]->token;
            if (temp_token != NAME)
                exit(1);
            //connecting a file for writing
            my_struct->outfile = p_token_mas[*token_mas_index-1]->name;
            return 0;
        }
        return 0;
    }
    if (temp_token == FLOWOUT) {
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME) {
            return (1);
        }
        my_struct->outfile = p_token_mas[*token_mas_index - 1]->name;
        temp_token = p_token_mas[*token_mas_index]->token;
        if (temp_token != FLOWIN && temp_token != APPEND ){
            exit(1);
        }
        if (temp_token == FLOWIN){
            (*token_mas_index)++;
            temp_token = p_token_mas[(*token_mas_index)++]->token;
            if (temp_token != NAME)
                exit(1);
            my_struct->infile = p_token_mas[*token_mas_index-1]->name;
            temp_token = p_token_mas[*token_mas_index]->token;
            return 0;
        }
        return 0;
    }

    if (temp_token == APPEND){
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME)
            return -1;
        my_struct->outfile = p_token_mas[*token_mas_index-1]->name;
        my_struct->append_flag = 1;
    }
}

int command(struct node *my_struct, struct token*** pp_token_mas, int* token_mas_index){
    struct token** p_token_mas = *pp_token_mas;
    int check_res;
    enum analise_tokens temp_token = p_token_mas[*token_mas_index]->token;
    if (temp_token == NAME){
        check_res = simple_command(my_struct, pp_token_mas, token_mas_index);
        if(check_res == -1){
            return -1;
        }
        return 0;
    }
    if(temp_token == L_BRACKET){
        (*token_mas_index)++;
        //create a subprocess - brackets
        struct node *bracket_process = (struct node*)malloc(sizeof(struct node));
        struct_init(bracket_process);
        my_struct->psubcmd = bracket_process;
        check_res = command_list(my_struct->psubcmd, pp_token_mas, token_mas_index);
        if(check_res == -1){
            return -1;
        }
    }else{
        return -1;
    }
    //checking if file is after parentheses
    temp_token = p_token_mas[*token_mas_index]->token;
    if (temp_token == FLOWIN){
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME){
            exit(1);
        }
        my_struct->infile = p_token_mas[*token_mas_index-1]->name;
    }
    temp_token = p_token_mas[*token_mas_index]->token;
    if (temp_token == FLOWOUT){
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME){
            exit(1);
        }
        my_struct->outfile = p_token_mas[*token_mas_index-1]->name;
    }
    temp_token = p_token_mas[*token_mas_index]->token;
    if (temp_token == APPEND){
        (*token_mas_index)++;
        temp_token = p_token_mas[(*token_mas_index)++]->token;
        if (temp_token != NAME){
            exit(1);
        }
        my_struct->outfile = p_token_mas[*token_mas_index-1]->name;
        my_struct->append_flag  = 1;
    }

    return 0;
}


int conveyor(struct node *my_struct, struct token*** pp_token_mas, int* token_mas_index){
    int check_res;
    struct token** p_token_mas = *pp_token_mas;
    check_res = command(my_struct, pp_token_mas, token_mas_index);
    if(check_res == -1){
        return -1;
    }

    enum analise_tokens temp_token = p_token_mas[*token_mas_index]->token;
    if(temp_token == PIPE){
        (*token_mas_index)++;
        //создаю структуру
        struct node *pipe_struct = (struct node*)malloc(sizeof(struct node));
        struct_init(pipe_struct);
        my_struct->pipe = pipe_struct;
        check_res = conveyor(my_struct->pipe, pp_token_mas, token_mas_index);
        if(check_res == -1){
            return -1;
        }
    }
}



int command_list(struct node* my_struct,struct token*** pp_token_mas, int* token_mas_index){
    int check_res;
    struct token** p_token_mas = *pp_token_mas;
    check_res = conveyor(my_struct, pp_token_mas, token_mas_index);
    //checking that there were no errors
    if(check_res == -1){
        return -1;
    }

    enum analise_tokens temp_token = p_token_mas[(*token_mas_index)++]->token;
    if(temp_token == BGRND || temp_token == AND || temp_token == SIMUL){
        if (temp_token == BGRND){
            if (p_token_mas[*token_mas_index]->token == EOI) {
                my_struct->backgnd = 1;
                return 0;
            }
            struct node *wait_struct = (struct node*)malloc(sizeof(struct node));
            struct_init(wait_struct);
            my_struct->next = wait_struct;
            my_struct->backgnd = 1;
            check_res = command_list(my_struct->next, pp_token_mas, token_mas_index);
            if(check_res == -1){
                return -1;
            }
        }

        if (temp_token == SIMUL){

            my_struct->wait_flag = 1;
            return 0;

        }

        if (temp_token == AND){
            struct node *conveyor_struct = (struct node*)malloc(sizeof(struct node));
            struct_init(conveyor_struct);
            my_struct->next = conveyor_struct;
            my_struct->check_flag = 1;
            check_res = command_list(my_struct->next, pp_token_mas, token_mas_index);
            if(check_res == -1){
                return -1;
            }
        }
    }
}



struct node* command_shell(struct token*** pp_token_mas){

    struct token** p_token_mas = *pp_token_mas;
    int check_res;
    int token_mas_index = 0;
    struct node *my_struct = (struct node*)malloc(sizeof(struct node));
    struct_init(my_struct);
    check_res = command_list(my_struct, pp_token_mas, &token_mas_index);
    if(check_res == -1){
        memory_feed(my_struct);
        return NULL;
    }
    return my_struct;
}


