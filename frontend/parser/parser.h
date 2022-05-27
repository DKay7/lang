#include <vector>
#include "nodes.hpp"
#include <optional>

namespace lang
{   
    namespace lexer
    {   
        enum class lexem_id
        {
            LRB,
            RRB,
            DEFUN,
            COMMA,
            ARROW,
            LCB,
            RCB,
            SEMICOLON,
            PLUS,
            MINUS,
            MULTIPLY,
            DIV,
            UNARY_MINUS,
            IF_KWORD,
            ELSE_KWORD,
            WHILE_KWORD,
            LET_KWORD,
            EQUAL,
            INT_TYPE,
            GREATER,
            GREATER_OR_EQUAL,
            EQUALS,
            LESS,
            LESS_OR_EQUAL,
            NOT_EQUALS,
            FOR_KWORD,
            IN_KWORD,
            ELLIPSIS,
            RETURN_KWORD,
            NUMBER,
            NAME
        };

        struct lexem
        {
            lexem_id id;
            std::string_view value;
        };
    }

    class dummy_parser
    {
        using lexem_vector_it = std::vector<lexer::lexem>::iterator;

    private:
        lexem_vector_it& lexem_iterator;

        std::optional<_program*>        get_programm        (); 
        std::optional<_function*>       get_function        (); 
        std::optional<_body*>           get_body            (); 
        std::optional<_statement*>      get_statement       (); 
        std::optional<_expression*>     get_expression      (); 
        std::optional<_term*>           get_term            (); 
        std::optional<_if*>             get_if              (); 
        std::optional<_while*>          get_while           (); 
        std::optional<_assignment*>     get_assigment       (); 
        std::optional<_arg*>            get_arg             (); 
        std::optional<_reassignment*>   get_reassigment     (); 
        std::optional<_type*>           get_type            (); 
        std::optional<_condition*>      get_condition       ();
        std::optional<_bool*>           get_bool            ();
        std::optional<_for*>            get_for             (); 
        std::optional<_return*>         get_return          (); 
        std::optional<_function_call*>  get_function_call   ();
        std::optional<_number*>         get_number          ();
        std::optional<_name*>           get_name            ();
        bool                            eat_lexem (lexer::lexem_id lexem_id, bool move_iter=true);

    public:
        dummy_parser (lexem_vector_it& lexem): lexem_iterator(lexem)
        {}
        
        std::optional<_program*> parse ();
        
    };
}