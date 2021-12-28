#include "solution.h"
#include "tree_exec.h"
#include "token.h"
#include "recursive_descent.h"

void read_expression(char** ans){
    char * read_str = (char*)malloc(sizeof(char));
    int read_str_size = 1, c, len = 0;
    while((c = getchar()) != '\n' && c != EOF){
        len++;
        if (len >= read_str_size){
            read_str_size *= 2;
            read_str = (char*)realloc(read_str, read_str_size*sizeof(char));
        }
        read_str[len - 1] = c;
    }
    read_str[len] = '\0';

    *ans = read_str;
}


int main() {
    char *a;
    struct token** token_mas;

    read_expression(&a);

    if (string_parsing(a, &token_mas)== -1){
        return 1;
    }

    free(a);

    struct node *aa = command_shell(&token_mas);
    
    int i = 0;
    
    while(token_mas[i]->token != EOI){
        free(token_mas[i]);
        i++;
    }
    free(token_mas[i]);
    free(token_mas);

    if (aa == NULL){
        return 1;
    }else {
        tree_process(aa, aa);
        memory_feed(aa);
    }
    return 0;
}
