#include "dummy_parser.h"
#include "nodes.hpp"
#include <optional>
#include <cstdlib>
#include <iostream>

namespace lang
{
    bool dummy_parser::eat_lexem (lexer::lexem_id lexem_id, bool move_iter)
    {
        if (lexem_iterator->id == lexem_id)
        {   
            if (move_iter)
                lexem_iterator++;
            return true;
        }

        return false;
    }

    std::optional<_program*> dummy_parser::get_programm ()
    {   
        std::vector<_function*> programm_functions;
        std::optional<_function*> func_node = std::nullopt;

        do {
            func_node = get_function();
            
            if (func_node.has_value())
                programm_functions.push_back (func_node.value());    

        } while (func_node.has_value());

        return new _program(programm_functions);;
    }

    std::optional<_function*> dummy_parser::get_function ()
    {
        if (!eat_lexem (lexer::lexem_id::DEFUN))
            return std::nullopt;

        _name* name = new _name(lexem_iterator->value);
        std::optional<_type*> func_return_type = std::nullopt;

        if (!eat_lexem(lexer::lexem_id::LRB))
            return std::nullopt;
        
        std::vector<_arg*> func_args;
        std::optional<_arg*> arg_node = std::nullopt;
        arg_node = get_arg();
            
        if (arg_node.has_value())
            func_args.push_back (arg_node.value());    
        
        do {
            if (!eat_lexem(lexer::lexem_id::COMMA))
                break;    
            
            arg_node = get_arg();
            
            if (arg_node.has_value())
                func_args.push_back (arg_node.value());    

        } while (arg_node.has_value());

        if (!eat_lexem(lexer::lexem_id::RRB))
            return std::nullopt;

        if (eat_lexem(lexer::lexem_id::ARROW)) 
            func_return_type = get_type();


        std::optional<_body*> func_body_opt = get_body ();
        if (!func_body_opt.has_value())
            return std::nullopt;

        _body* func_body = func_body_opt.value();

        return new _function (name, func_args, func_return_type, func_body);
    }

    std::optional<_body*> dummy_parser::get_body ()
    {
        if (!eat_lexem(lexer::lexem_id::LCB))
            return std::nullopt;

        std::vector<_statement*> body_statements;
        std::optional<_statement*> statement_node = std::nullopt;

        do {
            statement_node = get_statement();
            
            if (statement_node.has_value())
                body_statements.push_back (statement_node.value());    

        } while (statement_node.has_value());

        if (!eat_lexem(lexer::lexem_id::RCB))
            return std::nullopt;
        
        return new _body(body_statements);
    } 

    std::optional<_statement*> dummy_parser::get_statement ()
    {
        _statement::child_type child;
        
        std::optional<_if*> if_node = get_if ();
        if (if_node.has_value()) 
        {
            child = if_node.value();
            return new _statement(child);
        }

        std::optional<_while*> while_node = get_while ();
        if (while_node.has_value()) 
        {
            child = while_node.value();
            return new _statement(child);
        }

        std::optional<_for*> for_node = get_for ();
        if (for_node.has_value()) 
        {
            child = for_node.value();
            return new _statement(child);
        }

        std::optional<_expression*> expr_node = get_expression();
        if (expr_node.has_value() && eat_lexem(lexer::lexem_id::SEMICOLON))
        {
            child = expr_node.value();
            return new _statement(child);
        }

        std::optional<_assignment*> assign_node = get_assigment();
        if (assign_node.has_value() && eat_lexem(lexer::lexem_id::SEMICOLON))
        {
            child = assign_node.value();
            return new _statement(child);
        }

        std::optional<_reassignment*> reassign_node = get_reassigment();
        if (reassign_node.has_value() && eat_lexem(lexer::lexem_id::SEMICOLON))
        {
            child = reassign_node.value();
            return new _statement(child);
        }

        std::optional<_return*> return_node = get_return();
        if (return_node.has_value() && eat_lexem(lexer::lexem_id::SEMICOLON))
        {
            child = return_node.value();
            return new _statement(child);
        }

        return std::nullopt;
    }

    std::optional<_expression*> dummy_parser::get_expression ()
    {
        std::optional<_expression*> expr_left_node = get_expression();
        if (expr_left_node.has_value())
        {
            if (eat_lexem(lexer::lexem_id::PLUS))
            {   
                std::optional<_expression*> expr_right_node = get_expression();
                if (expr_right_node.has_value())
                {
                    _plus* plus_node = new _plus(expr_left_node.value(), expr_right_node.value());
                    return new _expression(plus_node);
                }

                return std::nullopt;
            }

            else if (eat_lexem(lexer::lexem_id::MINUS))
            {   
                std::optional<_expression*> expr_right_node = get_expression();
                if (expr_right_node.has_value())
                {
                    _minus* minus_node = new _minus(expr_left_node.value(), expr_right_node.value());
                    return new _expression(minus_node);
                }

                return std::nullopt;
            }

            return std::nullopt;
        }

        std::optional<_term*> term_left_node = get_term();
        if (term_left_node.has_value())
        {
            if (eat_lexem(lexer::lexem_id::MULTIPLY))
            {   
                std::optional<_term*> term_right_node = get_term();
                if (term_right_node.has_value())
                {
                    _multiply* multiply_node = new _multiply(term_left_node.value(), term_right_node.value());
                    return new _expression(multiply_node);
                }

                return std::nullopt;
            }

            else if (eat_lexem(lexer::lexem_id::DIV))
            {   
                std::optional<_term*> term_right_node = get_term();
                if (term_right_node.has_value())
                {
                    _div* div_node = new _div(term_left_node.value(), term_right_node.value());
                    return new _expression(div_node);
                }

                return std::nullopt;
            }

            return std::nullopt;
        }

        std::optional<_term*> term_node = get_term();
        if (term_node.has_value())
        {
            return new _expression(term_node.value());
        }

        return std::nullopt;
    }

    std::optional<_term*> dummy_parser::get_term ()
    {
        if (eat_lexem(lexer::lexem_id::UNARY_MINUS))
        {
            std::optional<_term*> term_node = get_term();
            if (term_node.has_value())
            {
                return term_node.value();
            }

            return std::nullopt;
        }

        if (eat_lexem(lexer::lexem_id::LRB))
        {
            std::optional<_expression*> expr_node = get_expression();
            if (expr_node.has_value() && eat_lexem(lexer::lexem_id::RRB))
                return new _term(expr_node.value());
            
            return std::nullopt;
        }

        std::optional<_function_call*> fn_call = get_function_call();
        if (fn_call.has_value())
        {
            return new _term(fn_call.value());
        }

        std::optional<_number*> num_node = get_number();
        if (num_node.has_value())
            return new _term(num_node.value());
        
        std::optional<_name*> name_node = get_name();
        if (name_node.has_value())
            return new _term(name_node.value());
        
        return std::nullopt;
    }

    std::optional<_if*> dummy_parser::get_if ()
    {
        if (!eat_lexem(lexer::lexem_id::IF_KWORD))
            return std::nullopt;

        std::optional<_condition*> if_condition = get_condition();
        if (!if_condition.has_value())
            return std::nullopt;


        std::optional<_body*> if_body = get_body();
        if (!if_body.has_value())
            return std::nullopt;

        std::optional<_body*> else_body = std::nullopt;
        if (eat_lexem(lexer::lexem_id::ELSE_KWORD))
        {
            else_body = get_body();
            if (!else_body.has_value())
                return std::nullopt;
        }

        return new _if(if_condition.value(), if_body.value(), else_body);
    } 

    std::optional<_while*> dummy_parser::get_while ()
    {
        if (!eat_lexem(lexer::lexem_id::WHILE_KWORD))
            return std::nullopt;

        std::optional<_condition*> while_condition = get_condition();
        if (!while_condition.has_value())
            return std::nullopt;


        std::optional<_body*> while_body = get_body();
        if (!while_body.has_value())
            return std::nullopt;

        return new _while(while_condition.value(), while_body.value());
    }

    std::optional<_assignment*> dummy_parser::get_assigment ()
    {
        if (!eat_lexem(lexer::lexem_id::LET_KWORD))
            return std::nullopt;
        
        std::optional<_arg*> assignment_arg = get_arg();
        if (!assignment_arg.has_value())
            return std::nullopt;
        
        if (!eat_lexem(lexer::lexem_id::EQUAL))
            return std::nullopt;
        
        std::optional<_expression*> assignment_expr = get_expression();
        if (!assignment_expr.has_value())
            return std::nullopt;

        return new _assignment(assignment_arg.value(), assignment_expr.value());
    }

    std::optional<_arg*> dummy_parser::get_arg ()
    {   
        std::optional<_name*> arg_name = get_name();
        if (!arg_name.has_value())
            return std::nullopt;

        std::optional<_type*> arg_type = std::nullopt;
        if (eat_lexem(lexer::lexem_id::ARROW))
        {
            std::optional<_type*> arg_type = get_type();
            if (!arg_type.has_value())
                return std::nullopt;
        }

        return new _arg(arg_name.value(), arg_type);
    }

    std::optional<_reassignment*> dummy_parser::get_reassigment ()
    {
        std::optional<_name*> reassignment_name = get_name();
        if (!reassignment_name.has_value())
            return std::nullopt;
        
        if (!eat_lexem(lexer::lexem_id::EQUAL))
            return std::nullopt;
        
        std::optional<_expression*> reassignment_expr = get_expression();
        if (!reassignment_expr.has_value())
            return std::nullopt;
        
        return new _reassignment(reassignment_name.value(), reassignment_expr.value());        
    }

    std::optional<_type*> dummy_parser::get_type ()
    {
        if(!eat_lexem(lexer::lexem_id::INT_TYPE))
            return std::nullopt;
        
        return new _type();
    }

    std::optional<_condition*> dummy_parser::get_condition ()
    {
        if (!eat_lexem(lexer::lexem_id::LRB))
            return std::nullopt;
        
        std::optional<_bool*> bool_condition = get_bool();
        if (!bool_condition.has_value())
            return std::nullopt;

        if (!eat_lexem(lexer::lexem_id::RRB))
            return std::nullopt;

        return new _condition(bool_condition.value());
    }

    std::optional<_bool*> dummy_parser::get_bool ()
    {
        std::optional<_expression*> left_expr = get_expression();
        if (!left_expr.has_value())
            return std::nullopt;

        _bool::compare_operator cmp_operator = _bool::not_valid;

        if (eat_lexem(lexer::lexem_id::GREATER))
            cmp_operator = _bool::grand;
        
        if (eat_lexem(lexer::lexem_id::GREATER_OR_EQUAL))
            cmp_operator = _bool::grand_or_equal;
        
        if (eat_lexem(lexer::lexem_id::EQUALS))
            cmp_operator = _bool::equal;
        
        if (eat_lexem(lexer::lexem_id::NOT_EQUALS))
            cmp_operator = _bool::not_equal;
        
        if (eat_lexem(lexer::lexem_id::LESS))
            cmp_operator = _bool::less;

        if (eat_lexem(lexer::lexem_id::LESS_OR_EQUAL))
            cmp_operator = _bool::less_or_equal;

        if (cmp_operator == _bool::not_valid)
            return std::nullopt;

        std::optional<_expression*> right_expr = get_expression();
        if (!right_expr.has_value())
            return std::nullopt;
            
        return new _bool (left_expr.value(), right_expr.value(), cmp_operator);
    }

    std::optional<_for*> dummy_parser::get_for ()
    {
        if (!eat_lexem(lexer::lexem_id::FOR_KWORD))
            return std::nullopt;
        
        if (!eat_lexem(lexer::lexem_id::LRB))
            return std::nullopt;
        
        std::optional<_name*> for_var = get_name();
        if (!for_var.has_value())
            return std::nullopt;

        if (!eat_lexem(lexer::lexem_id::IN_KWORD))
            return std::nullopt;

        std::optional<_term*> for_start_term = get_term();
        if (!for_start_term.has_value())
            return std::nullopt;

        if (!eat_lexem(lexer::lexem_id::ELLIPSIS))
            return std::nullopt;

        std::optional<_term*> for_end_term = get_term();
        if (!for_end_term.has_value())
            return std::nullopt;

        if (!eat_lexem(lexer::lexem_id::RRB))
            return std::nullopt;
        
        std::optional<_body*> for_body = get_body();
        if (!for_body.has_value())
            return std::nullopt;

        if (!eat_lexem(lexer::lexem_id::LRB))
            return std::nullopt;
        
        return new _for(for_var.value(), for_start_term.value(), for_end_term.value(), for_body.value());
    }

    std::optional<_return*> dummy_parser::get_return ()
    {
        if (!eat_lexem(lexer::lexem_id::RETURN_KWORD))
            return std::nullopt;
        
        std::optional<_expression*> ret_expr = get_expression();
        if (!ret_expr.has_value())
            return std::nullopt;

        return new _return(ret_expr.value());
    }

    std::optional<_function_call*>  dummy_parser::get_function_call ()
    {
        std::optional<_name*> fn_name = get_name();
        if (!fn_name.has_value())
            return std::nullopt;
        
        if (!eat_lexem(lexer::lexem_id::LRB))
            return std::nullopt;
        
        std::vector<_expression*> fn_call_expressions;
        std::optional<_expression*> expression = get_expression();
            
        if (expression.has_value())
            fn_call_expressions.push_back (expression.value());    
        
        do {
            if (!eat_lexem(lexer::lexem_id::COMMA))
                break;    
            
            expression = get_expression();
            
            if (expression.has_value())
                fn_call_expressions.push_back (expression.value());    

        } while (expression.has_value());

        if (!eat_lexem(lexer::lexem_id::RRB))
            return std::nullopt;

        return new _function_call (fn_name.value(), fn_call_expressions);
    }

    std::optional<_number*> dummy_parser::get_number ()
    {   

        if (!eat_lexem(lexer::lexem_id::NUMBER, false))
            return std::nullopt;
        
        int number = std::atoi((lexem_iterator->value).data());

        return new _number(new _type(), number);
    }

    std::optional<_name*> dummy_parser::get_name ()
    {
        if (!eat_lexem(lexer::lexem_id::NAME, false))
            return std::nullopt;
        
        std::string_view name = lexem_iterator->value;

        return new _name(name);
    }

    std::optional<_program*> dummy_parser::parse ()
    {
        std::optional<_program*> program_node = get_programm();
        return program_node;
    }
}

int main (void)
{   
    using lang::lexer::lexem_id;

    std::vector<lang::lexer::lexem> lexems = {{lang::lexer::lexem_id::DEFUN, "1"}, {lang::lexer::lexem_id::NAME, "aboba"}, {lang::lexer::lexem_id::LRB, "1"}, {lang::lexer::lexem_id::RRB, "1"}, {lang::lexer::lexem_id::LCB, "1"},
    {lang::lexer::lexem_id::DEFUN, "1"}, {lang::lexer::lexem_id::NUMBER, "12"}, {lang::lexer::lexem_id::PLUS, "+"}, {lang::lexer::lexem_id::NUMBER, "42"}, {lang::lexer::lexem_id::SEMICOLON, ";"}, {lang::lexer::lexem_id::RCB, "}"}};

    std::vector<lang::lexer::lexem>::iterator lexem_iter = lexems.begin(); 
    lang::dummy_parser parser(lexem_iter);
    std::optional<lang::_program*> program_node = parser.parse();

    if (program_node.has_value())
        std::cout << "not fuck" << std::endl;

    return 0;
}