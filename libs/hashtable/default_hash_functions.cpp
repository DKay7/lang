#include <inttypes.h>
#include <string.h>
#include <malloc.h>
#include "default_hash_functions.h"


//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint64_t int_hash (int x)
{
    return (uint64_t) ((x * 2654435761) % (2L << 32));
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint64_t const_int_hash (const int x)
{
    return int_hash (x);
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint64_t long_hash(uint64_t x)
{
    uint64_t hash = x;
     
    hash = (hash ^ (hash >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    hash = (hash ^ (hash >> 27)) * UINT64_C(0xbf58476d1ce4e5b9);
    hash = (hash ^ (hash >> 31));

    return hash;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// implementation of sdbm hash
uint64_t str_hash(char* str)
{
    uint64_t hash = (uint64_t) 'sdbm';
    int c;

    while ((c = *str++))
        hash = (uint64_t) c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint64_t const_str_hash(const char* str)
{   
    char* heaped_str = strdup (str);
    uint64_t hash = str_hash(heaped_str);
    free (heaped_str);
    return hash;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

uint64_t char_hash(char x)
{
    return int_hash ((int) x);
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

