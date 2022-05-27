#include "lexer.h"
#include <string.h>
#include <ctype.h>

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

int main ()
{   
    char* code_str = (char*) calloc (1200, sizeof (char));
    sprintf (code_str, R"(ABOBAS x120 times yamete kudasai  put it outta me  yamete 
    kudasai baka senpai <3 
                        
                        baka senpai)");

    lexer lexer = {};
    LexerCtor (&lexer, code_str);

    LexicalAnalysis (&lexer);
    StackDump (lexer.stack);

    LexerDtor(&lexer);

    return 0;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

bool LexicalAnalysis (lexer* lexer)
{
    assert (lexer);

    while (get_current_char (lexer) != '\0')
    {   
        token token = CheckAllLexems (lexer);
        
        if (token.token == WRONG_LEXEM)
            return false;
        
        StackPush (lexer->stack, token);
        lexer->current_index += token.len;
    }

    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

token CheckAllLexems (lexer* lexer)
{
    assert (lexer);
    assert (lexer->stack);
    assert (lexer->str_pointer);
    assert (lexer->str_to_token);
    
    #define compare_token(pair, lexer)                                                     \
        if (strncmp (pair.key, get_cur_str_pointer (lexer), (size_t) pair.value.len) == 0) \
            return pair.value;  

    // iterating over all hash-table and compare lexemms with code;
    for (uint64_t bucket_idx = 0; bucket_idx < lexer->str_to_token->size; ++bucket_idx)
    {
        if (lexer->str_to_token->buckets[bucket_idx].status == BUCKET_EMPTY)
            continue;
        
        uint64_t next_list_element_idx = lexer->str_to_token->buckets[bucket_idx].start_index;
        uint64_t bucket_size = lexer->str_to_token->buckets[bucket_idx].len;
        
        for (uint64_t list_idx = 0; list_idx < bucket_size; ++list_idx)
        {   
            compare_token (lexer->str_to_token->values->list[next_list_element_idx].data, lexer);
            next_list_element_idx = (uint64_t) lexer->str_to_token->values->list[next_list_element_idx].next;
        }
    }
    
    // if we are here than token is just a dinamic or the wrong lexem.
    return CheckDinamicLexems (lexer);
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

token CheckDinamicLexems (lexer* lexer)
{
    assert (lexer);
    
    token result = { 
        .token = WRONG_LEXEM, 
        .token_start_pointer =  (lexer->str_pointer + lexer->current_index),
        .len = 0
        };

    int index = lexer->current_index;

    #define collect_and_return_token(TOKEN, condition)  \
    {                                                   \
        while ((condition))                             \
            index++;                                    \
        result.token = TOKEN;                           \
        result.len = index - lexer->current_index;      \
        return result;                                  \
    }

    // check for comment
    if (get_last_token (lexer) == KWORD_COMMENT)
        collect_and_return_token (COMMENT_STR, get_ith_char (lexer, index) != '\n');

    // skip spaces
    if (isspace (get_current_char (lexer)))
        collect_and_return_token (SPACE, isspace (get_ith_char (lexer, index)));

    // if first symbol is digit than it's a number 
    if (isdigit (get_current_char (lexer)))
        collect_and_return_token (NUMBER, isdigit (get_ith_char (lexer, index)));
    
    // if first symbol is letter or '_' than it's a name 
    if (isalpha (get_current_char (lexer)) || get_current_char (lexer) == '_')
        // symbols that are not valid for 'name'-token
        collect_and_return_token (NAME, isalnum (get_ith_char (lexer, index)) || 
                                        get_ith_char (lexer, index) == '_' ||
                                        get_ith_char (lexer, index) == '-');

    // if it's not a name or a digit than it's wrong lexem
    return result;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void LexerCtor (lexer* lexer, char* code_str)
{   
    assert (lexer);
    assert (code_str);

    HashTable<const char*, token>* str_to_lexem = (HashTable<const char*, token>*) calloc (1, sizeof (*str_to_lexem));
    Stack<token>* tokens_stack = (Stack<token>*) calloc (1, sizeof (*tokens_stack));
    ASS (str_to_lexem, (void) 0);
    ASS (tokens_stack, (void) 0);

    HashTableCtor (str_to_lexem, 10, const_str_hash);
    StackCtor (tokens_stack, 10, token_printer);

    FillStrToToken (str_to_lexem);

    (*lexer) = {
        .str_pointer = code_str, 
        .current_index = 0, 
        .stack = tokens_stack,
        .str_to_token = str_to_lexem
        };
    
    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void LexerDtor (lexer* lexer)
{
    assert (lexer);
    HashTableDtor (lexer->str_to_token);
    StackDtor (lexer->stack);
    
    free (lexer->str_to_token);
    free (lexer->stack);
    free (lexer->str_pointer);

    (*lexer) = { .str_pointer = NULL, .current_index = 0, .stack = NULL, .str_to_token = NULL };

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void FillStrToToken (HashTable<const char*, token>* str_to_token)
{   
    assert (str_to_token);

    #define ins_token(enum_token, str_token)   \
        HashTableInsert<const char*, token> (str_to_token, str_token, {enum_token, str_token, strlen (str_token)})

    ins_token (OPER_ASSIGMENT,      "koko");
    ins_token (OPER_PLUS,           "helps cum");
    ins_token (OPER_MINUS,          "susck");
    ins_token (OPER_MULTIPLY,       "twosome");
    ins_token (OPER_DIVISION,       "shares dick with");

    ins_token (OPER_STRONG_LESS,    "is so small");
    ins_token (OPER_STRONG_MORE,    "is so big");
    ins_token (OPER_LESS_EQ,        "is smaller than mine");
    ins_token (OPER_MORE_EQ,        "is so big but not as mine");
    ins_token (OPER_EQUALITY,       "is just as mine");
    ins_token (OPER_INEQUALITY,     "is not just as mine");
    
    ins_token (PARENTHNESIS,        "<3");
    ins_token (L_CURLY_BRACKET,     "ooki");
    ins_token (R_CURLY_BRACKET,     "yamete kudasai");
    
    ins_token (KWORD_IF,            "yoku naai");
    ins_token (KWORD_WHILE,         "yamenaide");
    ins_token (KWORD_FUNC_DEF,      "onichan whats this");
    ins_token (KWORD_RETURN,        "kane");
    ins_token (KWORD_FUNC_PREPARAM, "baka");
    ins_token (KWORD_COMMENT,       "put it outta me");

    ins_token (COMMA,               "senpai");
    ins_token (LINE_END,            "ehhh");

    #undef ins_token

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

char get_current_char (lexer* lexer)
{
    assert (lexer);
    assert (lexer->str_pointer);

    return lexer->str_pointer[lexer->current_index];
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

char get_ith_char (lexer* lexer, int i)
{
    assert (lexer);
    assert (lexer->str_pointer);
    assert (i >= 0);

    return lexer->str_pointer[i];
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

char* get_cur_str_pointer (lexer* lexer)
{
    assert (lexer);
    assert (lexer->str_pointer);

    return &lexer->str_pointer [lexer->current_index];
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

LexemTokens get_last_token (lexer* lexer)
{
    assert (lexer);
    assert (lexer->stack);

    if (lexer->stack->size >= 1)
        return StackPeek (lexer->stack).token;
    
    else
        return WRONG_LEXEM;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void token_printer (FILE* file, token token)
{   
    fprintf (file, "TOKEN: \'");

    for (int i = 0; i < token.len; i++)
        fprintf (file, "%c", *(token.token_start_pointer + i));
    
    fprintf (file, "\'; LEN: %d; ENUM: %d; ", token.len, token.token);

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void token_ht_printer (FILE* file, HT_Pair<const char*, token>* pair)
{
    // TODO this is CRINGE AND FUCKING DOT IS CRINGE BUT IDK HOW TO PRINT FUCKING "<" INSIDE DOT-HTML TABLE!!!

    if (pair->value.token == PARENTHNESIS)
        fprintf (file, "<table>"
                        "<tr><td>KEY:</td><td>%s:</td></tr>"
                        "<tr><td>len:</td><td>%d;</td></tr>"
                        "<tr><td>token:</td><td>%d;</td></tr>"
                        "<tr><td>str: </td><td>%s;</td></tr>"
                        "</table>\n", "HEART", pair->value.len, pair->value.token, "HEART");
    else
        fprintf (file, "<table>"
                    "<tr><td>KEY:</td><td>%s:</td></tr>"
                    "<tr><td>len:</td><td>%d;</td></tr>"
                    "<tr><td>token:</td><td>%d;</td></tr>"
                    "<tr><td>str: </td><td>%s;</td></tr>"
                    "</table>\n", pair->key, pair->value.len, pair->value.token, pair->value.token_start_pointer);

}