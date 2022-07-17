#include <isa.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <memory/paddr.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

// a enum of token type, to divide diffrent of token
// some operator type token's value also as the index of the priority_table item
enum {

    // TODO add more token types
    //? myCodes begin ----------------------------------------------------------
    TK_NOTYPE = 256,
    TK_DEC,
    TK_HEX,
    TK_REG,
    TK_LE = 260,
    TK_GE = 261,
    TK_DEREF = 264,
    TK_EQ = 259,
    //? myCodes end -----------------------------------------------------------
};

// the rules to match the expression
static struct rule {
    const char* regex;
    int token_type;
} rules[] = {

    // TODO Add more rules
    //! Pay attention to the precedence level of different rules.
    //? myCodes begin ----------------------------------------------------------
    {" +", TK_NOTYPE},                // spaces
    {"\\$[0-9||A-Z||a-z]+", TK_REG},  // register
    {"0x[0-9||A-F||a-f]+", TK_HEX},   // hex
    {"[0-9]+", TK_DEC},               // dec
    {"==", TK_EQ},                    // equal
    {">=", TK_GE},                    // great or equal
    {"<=", TK_LE},                    // less or equal
    {"\\+", '+'},                     // plus
    {"-", '-'},                       // decrease
    {"\\*", '*'},                     // multi
    {"/", '/'},                       // divide
    {"\\(", '('},                     // left bra
    {"\\)", ')'},                     // right bra
    //? myCodes end -----------------------------------------------------------
};

// NR_REGEX, the num of the rules
#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

// Rules are used for many times. Therefore we compile them only once before any usage.
void init_regex() {
    int i;
    char error_msg[128];
    int ret;
    for (i = 0; i < NR_REGEX; i++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg,
                  rules[i].regex);
        }
    }
}

// info like type、 value of a token
typedef struct token {
    int type;
    char str[32];  // save the value of the token
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

// parse the expression to a token list
static bool make_token(char* e) {
    int position = 0;
    int i;
    regmatch_t pmatch;
    nr_token = 0;
    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
                pmatch.rm_so == 0) {
                char* substr_start = e + position;
                int substr_len = pmatch.rm_eo;
                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
                    rules[i].regex, position, substr_len, substr_len, substr_start);
                position += substr_len;

                // TODO for each token, add to the tokenList;
                //? myCodes begin ----------------------------------------------------------
                switch (rules[i].token_type) {
                    case TK_NOTYPE:
                        break;
                    case TK_DEC: {
                        tokens[nr_token].type = TK_DEC;
                        char* tmp = tokens[nr_token].str;
                        memcpy(tmp, substr_start, substr_len);
                        break;
                    }
                    case TK_HEX: {
                        tokens[nr_token].type = TK_HEX;
                        char* tmp = tokens[nr_token].str;
                        memcpy(tmp, substr_start, substr_len);
                        break;
                    }
                    case TK_REG: {
                        tokens[nr_token].type = TK_REG;
                        char* tmp = tokens[nr_token].str;
                        memcpy(tmp, substr_start, substr_len);
                        break;
                    }
                    default:
                        tokens[nr_token].type = rules[i].token_type;
                        break;
                }
                nr_token++;
                break;
                //? myCodes end -----------------------------------------------------------
            }
        }
        // no match rule, so exit
        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }
    return true;
}

//? myCodes begin ----------------------------------------------------------
// check the brackets
static bool check_brackets(Token* p, Token* q) {
    char stack[32] = {};
    size_t top = 0;
    for (Token* i = p; i <= q; i++) {
        // the leftest bracket is not match with the rightest one
        if (i != p && top == 0)
            return false;
        switch (i->type) {
            case '(':
                stack[top++] = '(';
                break;
            case ')':
                if (stack[top - 1] == '(')
                    top--;
                break;
            default:
                break;
        }
    }
    // if there are brackets left, it is a bad expression
    return top == 0;
}

// a table to record the priority of each operator
static u_int8_t priority_table[10] = {0, 0, 7, 5, 10, 5, 10, 7, 10, 0};
//                                   {<=,>=, *, +, deref, -, . , /,num, ==}

// we use this function to get the lowest level operator token's ptr;
Token* get_op_ptr(Token* p, Token* q) {
    size_t cnt = 0;  // it record now whether we in a pair of bracket
    Token* op = NULL;
    // check the tokens one by one
    for (Token* ptr = p; ptr <= q; ptr++) {
        switch (ptr->type) {
            // ignore the operator in the brackets
            case '(':
                cnt++;
                break;
            case ')':
                cnt--;
                break;
            // ignore the numbers and register
            case TK_DEC:
            case TK_HEX:
            case TK_REG:
                break;
            // the other cases are operators
            default:
                if (op == NULL || (!cnt && priority_table[op->type % 10] >= priority_table[ptr->type % 10])) {
                    op = ptr;
                }
                break;
        }
    }
    return op;
}

static word_t eval(Token* p, Token* q, bool* success);

// this function finish a calculate between two expr and one operator
// we choose the lowest level operator to calculate
static word_t calculate(Token* p, Token* q, bool* success) {
    Token* op = get_op_ptr(p, q);
    word_t b = eval(op + 1, q, success);
    if (*success != true)
        return -1;
    // the DEREF operator only has one arguments
    if (op->type == TK_DEREF) {
        // return one byte length data begin at b
        return paddr_read(b, 4);
    }
    word_t a = eval(p, op - 1, success);
    if ((*success) != true)
        return -1;
    // now we get the two parts of the expression
    switch (op->type) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        case TK_EQ:
            return a == b;
        case TK_LE:
            return a <= b;
        case TK_GE:
            return a >= b;
        default:
            *success = false;
            return -1;
    }
}

// eval function, calculate the value of the expression which
// has been parse to tokens saved in the token list;
static word_t eval(Token* p, Token* q, bool* success) {
    if (*success != true)
        goto BAD_EVAL;
    if (p > q)
        // it obviously is a wrong expression;
        goto BAD_EVAL;
    if (p == q) {
        // in this case, it can be a register/dec number/hex number
        // otherwise the expression is wrong;
        char* tmp;
        // return the value by parse its type
        switch (p->type) {
            case TK_DEC:
                return atoi(p->str);
            case TK_HEX:
                return strtoul(p->str, &tmp, 16);
            case TK_REG:
                return isa_reg_str2val(p->str + 1, success);
            default:
                goto BAD_EVAL;
        }
    } else if (check_brackets(p, q)) {
        // check it if it is surrounded by a pair of brackets
        return eval(p + 1, q - 1, success);
    } else {
        /* in this case, the expression must be concated by an
         * operator, the operator could be  == >= <= + - * / deref
         * we should choose the most low level operator and calculate it first
         * we call the calculate function to finish this job */
        word_t ret = calculate(p, q, success);
        if (*success == false)
            goto BAD_EVAL;
        return ret;
    }
BAD_EVAL:
    *success = false;
    return -1;
}

// try to get value from an expression
word_t expr(char* e, bool* success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    // before calculate, we should recognize the DEREF operator first
    for (size_t i = 0; i < nr_token; i++) {
        if (tokens[i].type == '*' &&
            (tokens[i - 1].type == '+' || tokens[i - 1].type == '-' ||
             tokens[i - 1].type == '*' || tokens[i - 1].type == '/' ||
             tokens[i - 1].type == TK_EQ || tokens[i - 1].type == TK_GE ||
             tokens[i - 1].type == TK_LE || i == 0)) {
            tokens[i].type = TK_DEREF;
        }
    }
    // todo: calculate the value of the expr and return;
    return eval(tokens, tokens + nr_token - 1, success);
}

//? myCodes end -----------------------------------------------------------
