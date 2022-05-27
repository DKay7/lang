#pragma once
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define STACK_DATA_POISON 0xDF

#define STACK_INCREASE_COEFFICIENT 2
#define STACK_DECREASE_COEFFICIENT 1.0 / 4.0

#define ASS(cond, ret)                                                  \
    if(!(cond))                                                         \
    {                                                                   \
        fprintf (stderr, "Soft assertation ( " #cond " ) has failed "   \
                         "in file %s:%d \n", __FILE__, __LINE__);       \
        return ret;                                                     \
    }

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
struct Stack
{       
    size_t size;
    size_t capacity;
    void (*print_function)(FILE*, T);
    T* data;
};

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
bool StackCtor (Stack<T>* stack, size_t size, void (*printer_function) (FILE*, T))
{   
    assert (stack);
    assert (size);
    assert (printer_function);

    T* data_ptr = (T*) calloc (size, sizeof (*data_ptr));
    ASS (data_ptr, false);

    (*stack) = { 
            .size = 0, 
            .capacity = size, 
            .print_function = printer_function, 
            .data = data_ptr
            };

    StackPoison (stack, 0, stack->capacity);

    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
void StackDtor (Stack<T>* stack)
{   
    assert (stack);

    StackPoison (stack, 0, stack->capacity);
    free (stack->data);
    (*stack) = { 
                .size = 0, 
                .capacity = 0, 
                .print_function = NULL, 
                .data = NULL
               };

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
void StackPoison (Stack<T>* stack, size_t start, size_t num)
{   
    assert (stack);
    assert (num <= stack->capacity);

    memset (&stack->data[start], STACK_DATA_POISON, num * sizeof (stack->data[start]));
    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
T StackPeek (Stack<T>* stack)
{
    assert (stack);
    assert(stack->data);
    assert (stack->size >= 1);

    return stack->data[stack->size - 1];
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
bool StackPush (Stack<T>* stack, T element)
{
    assert (stack);
    assert(stack->data);

    ASS(StackIncrease (stack), false);

    stack->data[stack->size] = element;
    stack->size++;
    
    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
T StackPop (Stack<T>* stack)
{
    assert (stack);

    ASS(StackDecrease (stack), 0);

    T out_element = stack->data[--stack->size];
    StackPoison (stack, stack->size, 1);
    
    return out_element;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
bool StackIncrease (Stack<T>* stack)
{
    assert (stack);
    assert (stack->data);

    if (stack->size + 1 > stack->capacity)
    {
        T* data_ptr = (T*) realloc (stack->data,  (size_t) ((STACK_INCREASE_COEFFICIENT) * stack->capacity + 1) * sizeof (*data_ptr));

        ASS (data_ptr, false);    

        stack->data = data_ptr;
        stack->capacity = (STACK_INCREASE_COEFFICIENT) * stack->capacity + 1;
    }
    
    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename T>
bool StackDecrease (Stack<T>* stack)
{
    assert (stack);
    assert (stack->data);

    if (stack->size <  (size_t) ((STACK_DECREASE_COEFFICIENT) * (double) stack->capacity))
    {
        T* data_ptr = (T*) realloc (stack->data, (size_t) ((STACK_DECREASE_COEFFICIENT) * (double) stack->capacity + 1) * sizeof (*data_ptr));

        ASS (data_ptr, false);

        stack->data = data_ptr;
        stack->capacity = (size_t) (STACK_DECREASE_COEFFICIENT) * stack->capacity + 1;
    }

    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


template <typename T>
void StackDump (Stack<T>* stack, FILE* logfile = stdout)
{   
    assert (stack);

    fprintf (logfile, 
        "\nSTACK: "            
        "\n{\n"
        " size = %lu \n"    
        " capacity = %lu\n"
        "\t{\n", 
        stack->size, stack->capacity);

    for (size_t i = 0; i < stack->capacity; i++)
    {
        fprintf (logfile, "\t");      

        if (i < stack->size)
        {                      
           fprintf  (logfile, "\t *");
        }

        else
        {                    
            fprintf (logfile, "\t  ");
        }

        fprintf  (logfile, "[%lu] = ", i);

        if (i < stack->size)
        {
            stack->print_function (logfile, stack->data[i]);
        }

        else
        {
            fprintf (logfile, "EMPTY STACK ELEMENT");
        }

        fprintf  (logfile, "\n");
    }

    fprintf (logfile, "\t}\n}\n");

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#undef STACK_DATA_POISON
#undef STACK_INCREASE_COEFFICIENT
#undef STACK_DECREASE_COEFFICIENT
#undef ASS
//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
