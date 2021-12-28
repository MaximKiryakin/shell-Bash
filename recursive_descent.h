

int command(struct node *my_struct, struct token*** pp_token_mas, int* token_mas_index);
int conveyor(struct node *my_struct, struct token*** pp_token_mas, int* token_mas_index);
int command_list(struct node* my_struct,struct token*** pp_token_mas, int* token_mas_index);
int simple_command(struct node *my_struct, struct token*** pp_token_mas, int* token_mas_index);
struct node* command_shell(struct token*** pp_token_mas);
