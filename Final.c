#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

#define INPUT_SIZE 1000

typedef struct Node
{
    float data;
    struct Node *next;
} Node;

typedef struct
{
    Node *top;
} Stack;

Stack* initialize ();
float pop (Stack *s);
void push (Stack *s, float value);
float peek (Stack *s);
int isEmpty(Stack *s);

int priority(char exp);
char what_operator(char* str);
int isNumeric(const char *str);
float calculator(float operand1, float operand2, char operator);

char* infixTopostfix(char *infix);
float evaluatePostfix(char* postfix);

char* get_string();
void print_float(float value); // function to print like the pdf

int main()
{
while(1){

    printf("(Enter e to exit)\nInput (Infix): ");
    char* input = get_string();

    if(input == NULL){
        free(input);
        continue; // error message displayed inside function: get_string()
    }

    if(!strcmp(input, "e") || !strcmp(input, "E")){
        free(input);
        printf("Bye\n");
        return 0;
    }

    char* postfix = infixTopostfix(input);
    printf("Output (Postfix): %s\n", postfix);
    float result = evaluatePostfix(postfix);
    print_float(result);

    free(postfix);
    free(input);
}
}

/*----------------Stack Functions----------------*/

Stack* initialize()
{
    Stack *s = malloc(sizeof(Stack));
    s->top = NULL;
    return s;
}
float pop(Stack *s)
{
    if(s->top == NULL){
        printf("ERROR, invalid infix");
        exit(1);
    }
    float data = s->top->data;
    Node *temp = s->top;
    s->top = temp->next;
    free(temp);
    return data;
}
void push (Stack *s, float value)
{
    Node *n = malloc(sizeof(Node));
    n->data = value;
    n->next = s->top;
    s->top = n;
}
float peek (Stack *s)
{
    if(s->top == NULL){
        printf("ERROR, invalid infix");
        exit(1);
    }
    return s->top->data;
}
int isEmpty(Stack *s)
{
    return (s->top == NULL);
}

/*-----------------------------------------------*/

char what_operator(char* str)
{
    if(!strcmp(str, "+")) return '+';
    if(!strcmp(str, "-")) return '-';
    if(!strcmp(str, "*")) return '*';
    if(!strcmp(str, "/")) return '/';
    if(!strcmp(str, "%")) return '%';
    if(!strcmp(str, "^")) return '^';
    if(!strcmp(str, "(")) return '(';
    if(!strcmp(str, ")")) return ')';
    //else:
    return 0;
}

int priority(char exp)
{
    if(exp=='+'||exp=='-')
        return 0;
    else if(exp=='%'||exp=='/'||exp=='*')
        return 1;
    else if(exp=='^')
        return 2;
    else
        return 3;
}

int isNumeric(const char *str)
{
    if (str == NULL || *str == '\0')
        return 0;

    if (*str == '-' || *str == '+')// Handle negative numbers
        str++;

    int hasDigits = 0;
    int hasDecimal = 0;

    // Check each character
    while (*str)
    {
        if (isdigit(*str))
        {
            hasDigits = 1;
        }
        else if (*str == '.' && !hasDecimal)
        {
            hasDecimal = 1;
        }
        else
        {
            return 0;  // Invalid character found
        }
        str++;
    }

    return hasDigits;  // Must have at least one digit to be valid
}

float calculator(float operand1, float operand2, char operator)
{
    float value = 0;

    switch(operator){
    case '+':
        value = operand1 + operand2;
        break;
    case '-':
        value = operand1 - operand2;
        break;
    case '/':
        if (operand2 == 0.0) {
            printf("ERROR: Division by zero\n");
            exit(1);
        }
        
        value = operand1 / operand2;
        break;
    case '*':
        value = operand1 * operand2;
        break;
    case '%':
        if (operand2 == 0.0) {
            printf("ERROR: Division by zero\n");
            exit(1);
        }

        value = fmod(operand1, operand2);
        break;
    case '^':
        value = pow(operand1, operand2);
        break;
    default:
        printf("Error\n");
        exit(1);
    }

    return value;
}

char *infixTopostfix(char *infix)
{
    char *postfix = malloc((strlen(infix)+2)); //+2 for the \0 at the end and extra space in postfix expression
    *postfix = '\0';
    Stack *s = initialize();
    char *token;
    token = strtok(infix," ");
    while (token != NULL)
    {
        if(isNumeric(token))
        {
            strcat(postfix,token);
            strcat(postfix," ");
        }
        else
        {
            char operator = what_operator(token);
            char temp[2];//is string to put in it operator
            if(isEmpty(s))
            {
                push(s,operator);
            }
            else if(operator==')')
            {
                while(!isEmpty(s) && peek(s)!='(')
                {
                    temp[0] = pop(s);
                    temp[1] = '\0';
                    strcat(postfix,temp);
                    strcat(postfix," ");
                }
                pop(s);
            }
            else
            {
                // if variable is ^ then it will be pushed into stack and nothing is poped (even if in stack is ^)
                // so for consecutive ^, it will be from right to left instead of from ledt to right like the rest of operations
                while (!isEmpty(s) && peek(s) != '(' && 
                    ( operator != '^' && priority(operator) <= priority(peek(s)) ) )
                // s not empty && not '(' in s && operator in s is of higher priority than variable except if variable is ^
                {
                    temp[0] = pop(s);
                    temp[1] = '\0';
                    strcat(postfix,temp);
                    strcat(postfix," ");
                }
                push(s,operator);
            }
        }
        token = strtok(NULL," ");
    }
    while(!isEmpty(s))
    {
        char temp[2];//is string to put in it operator
        temp[0] = pop(s);
        temp[1] = '\0';
        strcat(postfix,temp);
        strcat(postfix," ");
    }

    free(s);
    return postfix;
}

float evaluatePostfix(char* postfix)
{
    Stack* s = initialize(); // stack holding values
    float operand1, operand2, result = 0;
    char operator;
    
    char* token = strtok(postfix, " ");
    while(token){
        operator = what_operator(token);

        if(!operator){
            push(s, atof(token));
        }
        else{
            operand2 = pop(s);
            operand1 = pop(s);

            result = calculator(operand1, operand2, operator);

            push(s, result);
        }

        token = strtok(NULL, " ");
    }

    float value = pop(s);
    free(s);

    return value;
}

char* get_string()
{
    char* str = malloc(INPUT_SIZE+2); // +2, 1 for \n and the other for the \0 at the end
                                      // note: \n will be removed before using th string in our functions
    

    char* error_test = fgets(str, INPUT_SIZE+2, stdin);
    
    if (error_test == NULL){
            printf("Error reading input\n");
            return NULL;
    }

    // if input was too long (no newline in string)
    if (strchr(str, '\n') == NULL){ // if no \n in str
        int c;
        while ((c = getchar()) != '\n' && c != EOF); // removing extra input (input exceeded fgets)
    
        printf("ERROR: Input exceeds maximum length of %d characters.\n", INPUT_SIZE);
        return NULL;
    }
    
    str[strcspn(str, "\n")] = '\0'; // replacing \n with \0

    return str;
}

void print_float(float value)
{
    if (value == floorf(value)){ // if value has no decimal places
        printf("Value: %.1f\n", value);  // printf 1 decimal place of 0
    }
    else{
        printf("Value: %g\n", value); // prints value but removes trailing zeros
    }
}