#pragma once

#include "libs/stack_templates/stack.h"
#include "hashtable/hashtable.h"
#include "hashtable/default_hash_functions.h"


//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define ASS(cond, ret)                                                  \
    if(!(cond))                                                         \
    {                                                                   \
        fprintf (stderr, "Soft assertation ( " #cond " ) has failed "   \
                         "in file %s:%d \n", __FILE__, __LINE__);       \
        return ret;                                                     \
    }

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    

enum LexemTokens
{   
    WRONG_LEXEM, // this is cringe but we need this to return if we didn't get any lexems in function

    OPER_ASSIGMENT,
    OPER_PLUS,
    OPER_MINUS,
    OPER_MULTIPLY,
    OPER_DIVISION,
    
    OPER_STRONG_LESS,
    OPER_STRONG_MORE,
    OPER_LESS_EQ,
    OPER_MORE_EQ,
    OPER_EQUALITY,
    OPER_INEQUALITY,

    PARENTHNESIS,

    L_CURLY_BRACKET,
    R_CURLY_BRACKET,

    KWORD_IF,
    KWORD_WHILE,
    KWORD_FUNC_DEF,
    KWORD_FUNC_PREPARAM,
    KWORD_VAR_DEF,
    KWORD_RETURN,
    KWORD_COMMENT,

    COMMA,
    LINE_END,

    NAME,
    NUMBER,
    COMMENT_STR,
    SPACE,
};

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

struct token
{
    LexemTokens token;
    const char* token_start_pointer;
    int len;
};

struct lexer
{
    char* str_pointer;
    int current_index;
    Stack<token>* stack;
    HashTable<const char*, token>* str_to_token;
};

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

bool LexicalAnalysis (lexer* lexer);
token CheckAllLexems (lexer* lexer);
token CheckComments(lexer* lexer);
token CheckDinamicLexems (lexer* lexer);

void LexerCtor (lexer* lexer, char* code_str);
void LexerDtor (lexer* lexer);
void FillStrToToken (HashTable<const char*, token>* str_to_token);

char get_current_char (lexer* lexer);
char get_ith_char (lexer* lexer, int i);
char* get_cur_str_pointer (lexer* lexer);
LexemTokens get_last_token (lexer* lexer);

void token_printer (FILE* file, token token);
void token_ht_printer (FILE* file, HT_Pair<const char*, token>* pair);
