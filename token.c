
#include "solution.h"

//splitting a string into tokens, creating an array of tokens
int string_parsing(char* str, struct token*** p_token_mas){

    struct token** token_mas_loc;
    int token_mas_index_loc = 0;

    int len = strlen(str);

    token_mas_loc = (struct token**)malloc(len*sizeof(struct token*));

    int i = 0;

    while(i <= len){

        struct token *temp = (struct token*)malloc(sizeof(struct token));
        temp->name = NULL;

        while (str[i] == ' ')
            i++;

        if (str[i] == '&' && i + 1 < len && str[i+1] == '&'){
            temp->token = AND;
            i += 2;
            token_mas_loc[token_mas_index_loc++] = temp;
            continue;
        }

        if (str[i] == '>' && i + 1 < len && str[i+1] == '>'){
            temp->token = APPEND;
            i += 2;
            token_mas_loc[token_mas_index_loc++] = temp;
            continue;
        }

        if (isalpha(str[i]) || isdigit(str[i]) || str[i] == '.' || str[i] == '-' || str[i] == '/' || str[i] == '_'){

            int start = i;

            while (isalpha(str[i]) || isdigit(str[i]) || str[i] == '.' || str[i] == '-'|| str[i] == '/'|| str[i] == '_')
                i++;

            char *temp_str = (char*)malloc((i - start + 1)*sizeof(char));

            int size = i - start;
            for (int k = 0; k < size; k++){
                temp_str[k] = str[start + k];
            }

            temp_str[size] = '\0';

            temp->token = NAME;
            temp->name = temp_str;

            token_mas_loc[token_mas_index_loc++] = temp;
            continue;

        }

        switch(str[i]){
            case '(':
                temp->token = L_BRACKET;
                break;
            case ')':
                temp->token = R_BRACKET;
                break;
            case '|':
                temp->token = PIPE;
                break;
            case '&':
                temp->token = BGRND;
                break;
            case '<':
                temp->token = FLOWIN;
                break;
            case '>':
                temp->token = FLOWOUT;
                break;
            case ';':
                temp->token = SIMUL;
                break;
            case '\0':
                temp->token = EOI;
                break;
            default:

                return -1;
        }
        token_mas_loc[token_mas_index_loc++] = temp;
        i++;
    }
    token_mas_index_loc = 0;
    *p_token_mas = token_mas_loc;
    return 0;
}