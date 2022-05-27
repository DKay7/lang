#pragma once 

#include <vector>
#include <variant>
#include <string>
#include <optional>

namespace lang
{   
    class _program;
    class _function;
    class _body;
    class _statement;
    class _expression;
    class _term;
    class _if;
    class _while;
    class _assignment;
    class _reassignment;
    class _arg;
    class _type;
    class _name;
    class _number;
    class _condition;
    class _bool;
    class _plus;
    class _minus;
    class _multiply;
    class _div;
    class _unary_minus;
    class _function_call;
    class _for;
    class _return;

    class node
    {   
        public:
            //virtual void assemblate () = 0;
            virtual void draw () = 0;
    };
    
    
    class _program: public node
    {   
        public:
            std::vector<_function*> functions;
            _program (std::vector<_function*> program_functions): functions (program_functions) {}
            // void assemblate() override;
            void draw () override;
    };

    class _function: public node
    {   
        public:
            _name* name;
            std::vector<_arg*> args;
            std::optional<_type*> return_type;
            _body* body;
            _function (_name* func_name, std::vector<_arg*> func_args, std::optional<_type*> func_return_type, _body* func_body)
                : name(func_name), args(func_args), return_type(func_return_type), body(func_body) {}
            // void assemblate() override;
            void draw () override;
    };

    class _body: public node
    {
        public:
            std::vector<_statement*> statements;
            _body(std::vector<_statement*> body_statements): statements(body_statements) {}
            // void assemblate() override;
            void draw () override;
    };
    
    class _statement: public node
    {   
        public:
            using child_type = std::variant<_if*, _while*, _for*, _assignment*, _expression*, _reassignment*, _return*>; 

            child_type options;
            _statement (child_type statemen_child): options(statemen_child) {}
            // void assemblate() override;
            void draw () override;

    };

    class _expression: public node
    {   
        public:
            using child_type = std::variant<_plus*, _minus*, _multiply*, _div*, _term*>; 

            child_type options;
            _expression (child_type child): options(child) {}
            void draw () override;
            // void assemblate() override;
    };

    class _term: public node
    {
        
        public:
            using child_type = std::variant<_unary_minus*, _function_call*, _number*, _name*, _expression*>; 

            child_type options;
            _term(child_type child): options(child) {}
            void draw () override;
            // void assemblate() override;

    };

    class _if: public node
    {
        public:
            _condition* condition;
            _body* body;
            std::optional<_body*> else_body;
            _if (_condition* if_condition, _body* if_body, std::optional<_body*> else_body_)
                : condition(if_condition), body(if_body), else_body(else_body_) {}
            void draw () override;
            // void assemblate() override;
    };

    class _while: public node
    {
        public:
            _condition* condition;
            _body* body;
            _while (_condition* while_condition, _body* while_body)
                : condition(while_condition), body(while_body) {}
            // void assemblate() override;
            void draw () override;
    };

    class _assignment: public node
    {
        public:
            _arg* arg;
            _expression* expression;
            _assignment (_arg* assigment_arg, _expression* assigment_expression)
                : arg(assigment_arg), expression(assigment_expression) {}
            // void assemblate() override;
            void draw () override;
    };

    class _reassignment: public node
    {
        public:
            _name* name;
            _expression* expression;
            _reassignment (_name* reassigment_name, _expression* reassigment_expression)
                : name(reassigment_name), expression(reassigment_expression) {}            
            // void assemblate() override;
            void draw () override;
    };

    class _arg: public node
    {
        public:
            _name* name;
            std::optional<_type*> type;
            _arg (_name* arg_name, std::optional<_type*> arg_type): name(arg_name), type(arg_type) {}
            // void assemblate() override;
            void draw () override;
    };

    class _type: public node
    {
        public:
            // void assemblate() override;
            void draw () override;
    };

    class _name: public node
    {
        public:
            std::string_view name;
            _name (std::string_view name_): name(name_) {}
            // void assemblate() override;
            void draw () override;
    };

    class _number: public node
    {
        public:
            _type* number_type;
            int    number;
            _number ( _type* number_type_, int num_): number_type(number_type_), number(num_) {}
            // void assemblate() override;
            void draw () override;
    };

    class _condition: public node
    {
        public:
            _bool* condition;
            _condition (_bool* condition_): condition(condition_) {}
            // void assemblate() override;
            void draw () override;
    };

    class _bool: public node
    {   
        public:
            enum compare_operator
            {   
                not_valid,
                less,
                less_or_equal,
                grand,
                grand_or_equal,
                equal,
                not_equal
            };

            _expression* left_expr;
            _expression* right_expr;
            compare_operator cmp_operator;
            _bool(_expression* bool_left, _expression* bool_right, compare_operator bool_cmp)
                : left_expr(bool_left), right_expr(bool_right), cmp_operator(bool_cmp) {}
            // void assemblate() override;
            void draw () override;

    };

    class _plus: public node
    {   
        public:
            _expression* left_expr;
            _expression* right_expr;
            _plus (_expression* plus_left, _expression* plus_right)
                : left_expr (plus_left), right_expr (plus_right) {}
            // void assemblate() override;
            void draw () override;
    };

    class _minus: public node
    {   
        public:
            _expression* left_expr;
            _expression* right_expr;
            _minus (_expression* minus_left, _expression* minus_right)
                : left_expr (minus_left), right_expr (minus_right) {}
            // void assemblate() override;
            void draw () override;
    };

    class _multiply: public node
    {   
        public:
            _term* left_expr;
            _term* right_expr;
            _multiply (_term* multiply_left, _term* multiply_right)
                : left_expr (multiply_left), right_expr (multiply_right) {}
            // void assemblate() override;
            void draw () override;
    };

    class _div: public node
    {   
        public:
            _term* left_expr;
            _term* right_expr;
            _div (_term* div_left, _term* div_right)
                : left_expr (div_left), right_expr (div_right) {}
            // void assemblate() override;
            void draw () override;
    };

    class _unary_minus: public node
    {   
        public:
            _expression* expression;
            _unary_minus (_expression* minus_expr)
                : expression (minus_expr) {}
            // void assemblate() override;
            void draw () override;
    };
    
    class _function_call: public node
    {   
        public:
            _name* name;
            std::vector<_expression*> expressions;
            _function_call (_name* fn_call_name, std::vector<_expression*> fn_call_expressions)
                : name(fn_call_name), expressions(fn_call_expressions) {}
            // void assemblate() override;
            void draw () override;
    };

    class _for: public node
    {
        public:
            _name* name;
            _term* iter_range_start;
            _term* iter_range_stop;
            _body* body;
            _for (_name* for_name, _term* for_start, _term* for_stop, _body* for_body)
                : name(for_name), iter_range_start(for_start), iter_range_stop(for_stop), body(for_body) {}
            // void assemblate() override;
            void draw () override;
    };

    class _return: public node
    {
        public:
            _expression* expression;
            _return (_expression* ret_expr): expression(ret_expr) {}
            // void assemblate() override;
            void draw () override;
    };
}