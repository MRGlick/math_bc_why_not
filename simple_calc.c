// printf()
#include <stdio.h>
// malloc(), free()
#include <stdlib.h>
// true, false, bool
#include <stdbool.h>
// strlen(), gets()
#include <string.h>

#define is_num(c) ((c <= '9' && c >= '0') || c == '.')
#define is_operator(c) (c == '+' || c == '-' || c == '/' || c == '*')
#define is_bracket(c) (c == '(' || c == ')')

typedef struct Part {
    int idx;
    int len;
} Part;

typedef enum TokenType {
    OPERATOR,
    VALUE,
    EXPRESSION,
    EMPTY
} TokenType;

typedef struct Token {
    union {
        double val;
        char operator;
        struct {
            struct Token *expression_tokens;
            int exp_tokens_len;
        };
    };

    TokenType type;
} Token;

#define first_digit(d) (((int)d) % 10)
#define frac(d) (d - (int)d)

int get_nth_digit(int num, int n) {
    for (int i = 0; i < n; i++) {
        num /= 10;
    }

    return num % 10;
}

void print_double(double d) {

    char buf[1000] = {0};

    sprintf(buf, "%.10f", d);


    int len;
    for (len = strlen(buf) - 1; len >= 0; len--) {
        if (buf[len] == '.') {
            len--;
            break;
        }
        if (buf[len] != '0') break;
    }

    if (len == -1 && buf[0] == '0') {
        printf("0");
    }

    for (int i = 0; i <= len; i++) {
        printf("%c", buf[i]);
    }

}


void print_tokens(const Token *tokens, int len);


Part *lex(const char *input, int len, int *parts_len) {
    Part *arr = calloc(sizeof(Part), len);
    int arr_ptr = 0;
    *parts_len = 0;

    int i = 0;
    while (i < len) {

        while (i < len && input[i] == ' ') i++;

        int current_len = 0;

        while (i + current_len < len && is_num(input[i + current_len])) {
            current_len++;
        }
        if (current_len) {
            arr[arr_ptr++] = (Part){
                .idx = i,
                .len = current_len
            };

            *parts_len += 1;

            i += current_len - 1;
            goto ipp;
        }

        if (is_operator(input[i]) || is_bracket(input[i])) {
            arr[arr_ptr++] = (Part){
                .idx = i,
                .len = 1
            };

            *parts_len += 1;
            goto ipp;
        }


        ipp: i++;

    }

    return arr;
}

double str_to_double(const char *str, int len) {
    double res = 0;
    bool past_dot = false;
    int decimal_idx = 1;
    for (int i = 0; i < len; i++) {
        if (str[i] == '.') {
            past_dot = true;
            continue;
        }

        if (!is_num(str[i])) continue;

        if (!past_dot) {
            res = res * 10 + (str[i] - '0');
        } else {
            double val = (str[i] - '0');
            for (int j = 0; j < decimal_idx; j++) {
                val /= 10;
            }
            res += val;
            decimal_idx++;
        }
    }

    return res;
}

// 5 + 3 * 2 - (4 + 1)
Token *tokenize(const char *input, Part *parts, int parts_len, int *tokens_len) {
    Token *tokens = calloc(sizeof(Token), parts_len);
    int tokens_ptr = 0;

    for (int i = 0; i < parts_len; i++) {
        Part part = parts[i];



        if (is_num(input[part.idx])) {
            Token token = {
                .type = VALUE, 
                .val = str_to_double(input + part.idx, part.len)
            };
            tokens[tokens_ptr++] = token;
        } else if (is_operator(input[part.idx])) {

            Token token = {
                .type = OPERATOR,
                .operator = input[part.idx]
            };

            tokens[tokens_ptr++] = token;
        } else if (input[part.idx] == '(') {
            int j = i;
            while (j < parts_len && input[parts[j].idx] != ')') j++;
            
            int exp_tokens_len;
            Token *expression_tokens = tokenize(input, parts + i + 1, j - i, &exp_tokens_len);

            Token token = {
                .type = EXPRESSION,
                .expression_tokens = expression_tokens,
                .exp_tokens_len = exp_tokens_len
            };

            tokens[tokens_ptr++] = token;

            i = j;

        }

    }

    if (tokens_len != NULL)
        *tokens_len = tokens_ptr;

    return tokens;
}

double apply_operator(double val1, double val2, char op) {
    if (op == '+') {
        return val1 + val2;
    } else if (op == '-') {
        return val1 - val2;
    } else if (op =='/') {
        return val1 / val2;
    } else if (op == '*') {
        return val1 * val2;
    }

    return -1;
}

int operator_score(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    // ...
    return -1;
}

double resolve_tokens(Token *tokens, int tokens_len, int nesting_level) {

    int best_op_idx = 999; // arbitrary value

    for (int i = 0; i < nesting_level; i++) printf("  >  ");
    print_tokens(tokens, tokens_len);
    printf("\n");

    while (true) {
        best_op_idx = -1;
        for (int i = 0; i < tokens_len; i++) {

            if (tokens[i].type == EMPTY) continue;

            if (tokens[i].type == EXPRESSION) {
                tokens[i] = (Token) {
                    .type = VALUE,
                    .val = resolve_tokens(tokens[i].expression_tokens, tokens[i].exp_tokens_len, nesting_level + 1),
                };
            }

            if (tokens[i].type == OPERATOR) {

                if (best_op_idx == -1 ||  operator_score(tokens[i].operator) > operator_score(tokens[best_op_idx].operator)) {
                    best_op_idx = i;
                }
            }

        }

        if (best_op_idx == 0 || best_op_idx == tokens_len - 1) {
            printf("Empty or operators on bounds! \n");
            exit(-1);
        }

        if (best_op_idx == -1) {
            break;
        }

        int left = best_op_idx - 1;
        int op = best_op_idx;
        int right = best_op_idx + 1;
        while (tokens[right].type == EMPTY) {
            right++;
            if (right >= tokens_len) {
                printf("Operator technically out of bounds! \n");
                exit(-1);
            }
        }
        while (tokens[left].type == EMPTY) {
            left--;
            if (left < 0) {
                printf("Operator technically out of bounds! \n");
                exit(-1);
            }
        }
        if (tokens[right].type == EXPRESSION) {
            tokens[right] = (Token) {
                .type = VALUE,
                .val = resolve_tokens(tokens[right].expression_tokens, tokens[right].exp_tokens_len, nesting_level + 1),
            };
        }

        tokens[left] = (Token) {
            .type = VALUE,
            .val = apply_operator(tokens[left].val, tokens[right].val, tokens[op].operator)
        };

        tokens[op] = (Token) {.type = EMPTY};

        tokens[right] = (Token) {.type = EMPTY};

        for (int i = 0; i < nesting_level; i++) printf("  >  ");
        print_tokens(tokens, tokens_len);
        printf("\n");
    }
    
    double res = tokens[0].val;

    free(tokens);

    return res; // it always goes to the first token
    
}





void print_token(Token t);

void print_tokens(const Token *tokens, int len) {

    for (int i = 0; i < len; i++) {
        print_token(tokens[i]);
    }
}

void print_token(Token t) {
    switch (t.type) {
        case EXPRESSION:
            printf("(");
            print_tokens(t.expression_tokens, t.exp_tokens_len);
            printf(")");
            break;
        case VALUE:
            print_double(t.val);
            printf(" ");
            break;
        case OPERATOR:
            printf("%c ", t.operator);
            break;
        default:
            break;
    }
}
double eval(const char *input) {

    int parts_len;

    Part *parts = lex(input, strlen(input), &parts_len);

    int tokens_len;

    Token *tokens = tokenize(input, parts, parts_len, &tokens_len);

    double result = resolve_tokens(tokens, tokens_len, 1);

    free(parts);

    return result;
}


int main() {

    printf("SIMPLE CALC 1.0\n-------------------------\n");

    while (true) {
        printf("\n>>> ");

        char input[1024] = {0};

        gets(input);

        double result = eval(input);

        printf("result: ");

        print_double(result);
        printf("\n");
        
        
    }
    return 0;
}