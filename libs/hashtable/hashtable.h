#pragma once

#include "linked_list_templates/list.h"
#include <assert.h>
#include <malloc.h>
#include <inttypes.h>

// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// DEFINES
// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define ASS(cond, ret)                                                  \
    if(!(cond))                                                         \
    {                                                                   \
        fprintf (stderr, "Soft assertation ( " #cond " ) has failed "   \
                         "in file %s:%d \n", __FILE__, __LINE__);       \
        return ret;                                                     \
    }

// DE(A)(PRESS)F
#define POISON 0xDEDF

#define MAX_LIST_ELEMENT (hash_table->values->size + 1)
#define MAX_HT_ELEMENT   (hash_table->size + 1)
#define HT_SIZE_INCREASE_COEF 2
#define HT_INCREASE_DECISION_COEF 1.5

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

enum BUCKET_STATUSES 
{
    BUCKET_EMPTY       = 0xBEEF,
    BUCKET_NOT_EMPTY   = 0xBACA, 
};

// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// STRUCTURES
// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

struct HT_Bucket
{
    size_t start_index;
    size_t len;
    BUCKET_STATUSES status;
};

template <typename K, typename V>
struct HT_Pair
{
    K key;
    V value;
};

template <typename K, typename V>
struct HashTable
{
    uint64_t (*hash_function)(K key);
    bool     (*key_equality_func)(K first, K second);

    HT_Bucket*  buckets;
    LinkedList<HT_Pair<K, V>>* values;

    size_t size;
    size_t buckets_capacity;
};

// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTIONS 
// flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
HT_Pair<K, V> HashTablePairCtor (K key, V value)
{
    return {key, value};
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K>
bool SimpleKeyEqualityFunction (K first, K second)
{
    return (first == second);
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

HT_Bucket* HashTableBucketsArrayCtor (uint64_t size)
{
    HT_Bucket* buckets = (HT_Bucket*) calloc (size, sizeof (*buckets));
    ASS (buckets, NULL);
    memset (buckets, POISON,  size * sizeof (*buckets));

    for (size_t i = 0; i < size; i++)
        buckets[i].status = BUCKET_EMPTY;

    return buckets;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
void HashTableCtor (HashTable<K, V>* hash_table, size_t table_size, 
                    uint64_t (*HashFunction)(K), 
                    bool (*KeyEqualityFunc)(K, K) = SimpleKeyEqualityFunction)
{
    assert (hash_table);
    assert (HashFunction);

    ASS (table_size > 1 && "size should be bigger than 1", (void) 0);

    hash_table->hash_function = HashFunction;
    hash_table->key_equality_func = KeyEqualityFunc;

    hash_table->size = table_size;
    
    hash_table->buckets = HashTableBucketsArrayCtor (table_size);
    ASS (hash_table->buckets, (void) 0);

    LinkedList<HT_Pair<K, V>>* values_list = (LinkedList<HT_Pair<K, V>>*) calloc (1, sizeof (*values_list));
    ASS (values_list, (void) 0);

    LLCtor (values_list, (int) table_size);

    hash_table->values = values_list;

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
void HashTableDtor (HashTable<K, V>* hash_table)
{
    assert (hash_table);

    LLDtor (hash_table->values);
    free (hash_table->values);
    hash_table->values  = NULL;

    memset (hash_table->buckets, POISON,  hash_table->size * sizeof (*hash_table->buckets));
    free (hash_table->buckets);
    hash_table->buckets = NULL;

    hash_table->size = 0;
    hash_table->buckets_capacity = 0;
    hash_table->key_equality_func  = NULL;
    hash_table->hash_function      = NULL;
    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
uint64_t __HashTableGetPosition (HashTable<K, V>* hash_table, K key)
{
    uint64_t key_hash = hash_table->hash_function (key);

    // it works only if hash table's sise is the power of 2
    // return key_hash & (hash_table->size - 1);
    return key_hash % hash_table->size;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
uint64_t __HashTableSearchForKey (HashTable<K, V>* hash_table, K key)
{
    uint64_t position = __HashTableGetPosition (hash_table, key);

    if (hash_table->buckets[position].status == BUCKET_EMPTY)
        return (uint64_t) MAX_LIST_ELEMENT;
    
    else
    {   
        uint64_t val_pos =  hash_table->buckets[position].start_index;
        uint64_t bucket_size = hash_table->buckets[position].len;

        for (uint64_t i = 0; i < bucket_size; i++)
        {   
            if (hash_table->key_equality_func (hash_table->values->list[val_pos].data.key, key))
                return val_pos;

            val_pos = (uint64_t) hash_table->values->list[val_pos].next;
        }
    }

    return (uint64_t) MAX_LIST_ELEMENT;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
bool HashTableInsert (HashTable<K, V>* hash_table, K key, V value)
{
    assert (hash_table);

    // check if key is already in the table
    if (__HashTableSearchForKey(hash_table, key) != (uint64_t) MAX_LIST_ELEMENT)
        return false;
    // check capacity and increase if needed
    if ((double) hash_table->buckets_capacity *  HT_INCREASE_DECISION_COEF >= (double) hash_table->size)
        HashTableRehash (hash_table, false);

    HT_Pair<K, V> pair = HashTablePairCtor (key, value);
    uint64_t position = __HashTableGetPosition (hash_table, key);
    
    if (hash_table->buckets[position].status == BUCKET_EMPTY)
    {   
        // just insert
        int list_position = LLInsertAfter (hash_table->values, hash_table->values->list[0].prev, pair);
        ASS (list_position > 0, false);
        hash_table->buckets[position] = { .start_index = (uint64_t) list_position, 
                                          .len = 1, 
                                          .status = BUCKET_NOT_EMPTY };

        hash_table->buckets_capacity += 1;
        return true;
    }

    else 
    {
        // check size and go through list
        uint64_t index_to_insert_after = hash_table->buckets[position].start_index;

        int list_position = LLInsertAfter (hash_table->values, (int) index_to_insert_after, pair);
        ASS (list_position > 0, false);
        hash_table->buckets[position].len += 1;
        hash_table->buckets_capacity += 1;

        return true;
    }
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
uint64_t HashTableFind (HashTable<K, V>* hash_table, K key)
{
    assert (hash_table);

    uint64_t bucket_position = __HashTableGetPosition (hash_table, key);
    ASS (hash_table->buckets[bucket_position].status != BUCKET_EMPTY, (uint64_t) MAX_LIST_ELEMENT);

    uint64_t bucket_size = hash_table->buckets[bucket_position].len;
    uint64_t val_pos = hash_table->buckets[bucket_position].start_index;
    bool found = false;

    for (uint64_t i = 0; i < bucket_size; i++)
    {      
        if (hash_table->key_equality_func (hash_table->values->list[val_pos].data.key, key))
        {
            found = true;
            break;
        }
        
        val_pos = (uint64_t) hash_table->values->list[val_pos].next;

    }

    if (found)
        return val_pos;
    else
        return (uint64_t) MAX_LIST_ELEMENT;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
bool HashTableDelete (HashTable<K, V>* hash_table, K key)
{
    assert (hash_table);

    uint64_t bucket_position = __HashTableGetPosition (hash_table, key);
    uint64_t pair_position = HashTableFind (hash_table, key);
    ASS (pair_position != (uint64_t) MAX_LIST_ELEMENT, false);
    hash_table->buckets_capacity -= 1;
    
    // case if we delete element  which is the first in the bucket
    if (pair_position == hash_table->buckets[bucket_position].start_index)
    {
        if (hash_table->buckets[bucket_position].len == 1)
        {
            LLDelete (hash_table->values, (int) pair_position);
            
            memset (&hash_table->buckets[bucket_position], POISON, sizeof (hash_table->buckets[bucket_position]));
            hash_table->buckets[bucket_position].len    = 0;
            hash_table->buckets[bucket_position].status = BUCKET_EMPTY;
            return true;
        }

        else
        {
            uint64_t next_index = (uint64_t) hash_table->values->list[pair_position].next;
            LLDelete (hash_table->values, (int) pair_position);

            hash_table->buckets[bucket_position].len -= 1;
            hash_table->buckets[bucket_position].start_index = next_index;
            return true;
        }
    }

    LLDelete (hash_table->values, (int) pair_position);
    hash_table->buckets[bucket_position].len -= 1;
    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
bool HashTableRehash (HashTable<K, V>* hash_table, bool keep_size=true)
{
    assert (hash_table);

    uint64_t new_size = hash_table->size;
    
    if (!keep_size)
        new_size *= HT_SIZE_INCREASE_COEF;

    uint64_t old_size = hash_table->size;
    hash_table->size = new_size;

    LinkedList<HT_Pair<K, V>>* new_list = (LinkedList<HT_Pair<K, V>>*) calloc (1, sizeof (*new_list));
    ASS (new_list, false);
    LLCtor (new_list, (int) new_size);

    HT_Bucket* new_buckets = HashTableBucketsArrayCtor (new_size);
    ASS (new_buckets, false);

    HT_Bucket* old_buckets = hash_table->buckets;
    LinkedList<HT_Pair<K, V>>* old_list = hash_table->values;
    hash_table->buckets = new_buckets;
    hash_table->values = new_list;
    hash_table->buckets_capacity = 0;

    for (uint64_t i = 0; i < old_size; i++)
    {
        if (old_buckets[i].status == BUCKET_NOT_EMPTY)
        {   
            uint64_t bucket_size = old_buckets[i].len;
            uint64_t val_idx = old_buckets[i].start_index;
            
            for (uint64_t inner_inx = 0; inner_inx < bucket_size; inner_inx++)
            {
                bool inserted = HashTableInsert (hash_table, old_list->list[val_idx].data.key, 
                                                             old_list->list[val_idx].data.value);
                ASS (inserted && "Table is incorrect now! Sorry :(", false);

                val_idx = (uint64_t) old_list->list[val_idx].next;
            }
        }
    }
    
    LLDtor (old_list);
    free (old_list);
    free (old_buckets);
    old_list = NULL;
    old_buckets = NULL;

    return true;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void HashTableCompileDump (const char* filename_dot, const char* filename_out, const uint64_t filename_size)
{
    const uint64_t compile_cmd_len = 28;
    const uint64_t open_cmd_len = 23;

    char* compile_cmd = (char*) calloc (2 * filename_size + compile_cmd_len, sizeof (*compile_cmd));
    char* open_cmd =    (char*) calloc (filename_size + open_cmd_len,        sizeof (*open_cmd));
    
    sprintf (compile_cmd, "dot -Tpng %s -o %s", filename_dot, filename_out);
    sprintf (open_cmd, "xdg-open %s 2> /dev/null", filename_out);

    printf ("\nCOMPILING %s\n", compile_cmd);

    system (compile_cmd);
    system (open_cmd);

    free (compile_cmd);
    free (open_cmd);

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
void HashTableDump (HashTable<K, V>* hash_table, void (*DataPrinter)(FILE*, HT_Pair<K, V>*), bool full_dump=false)
{
    assert (hash_table);

    char filename_dot[] = "/tmp/fileXXXXXX";
    char filename_out[] = "/tmp/fileXXXXXX";

    int dot_fd = mkstemp (filename_dot);
    close (mkstemp (filename_out));

    FILE* file = fdopen (dot_fd, "w");
    ASS (file, (void) 0);

    fprintf (file,  "digraph { \n");

    if (full_dump)
    {   
        __LLDrowSubgraph (file, hash_table->values, DataPrinter);
        __HashTableDrowFullSubgraph (file, hash_table);
    }

    else
    {
        __HashTableDrowMinSubgraph (file, hash_table, DataPrinter);
    }

    fprintf (file, "}\n");
    fclose (file);

    HashTableCompileDump (filename_dot, filename_out, sizeof (filename_dot));

    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
void __HashTableDrowFullSubgraph (FILE* file, HashTable<K, V>* hash_table)
{   
    fprintf (file, "subgraph { ");

    fprintf (file, 
            R"(
                ht_main_node 
                [   
                    style="filled",
                    shape=plaintext, 
                    label=<
                    <table border="1" cellborder="1" cellspacing="0">
                    <tr>)");

    for (uint64_t i = 0; i < hash_table->size; i++)
    {
        fprintf (file,
                R"(
                    <td>
                    <table border="0" cellborder="1" cellspacing="0"  port="ht_node_%02lu">
                    <tr><td colspan="2">Node #%lu</td></tr>
                    <tr>
                        <td>STORED INDEX</td>
                        <td>%lu</td>
                    </tr>
                    <tr>
                        <td>STATUS</td>
                        <td>%s</td>
                    </tr>
                    <tr>
                        <td>LEN</td>
                        <td>%lu</td>
                    </tr>
                    </table>
                    </td>
                )", i, i, hash_table->buckets[i].start_index, 
                (hash_table->buckets[i].status == BUCKET_EMPTY)? "EMPTY":"NOT EMPTY",
                hash_table->buckets[i].len);
    }

    fprintf (file, 
            R"( 
                </tr>
                </table>
                >
                ];
            )");

    for (uint64_t i = 0; i < hash_table->size; i++)
    {
        if (hash_table->buckets[i].status != BUCKET_EMPTY)
            fprintf (file, "ht_main_node:<ht_node_%02lu> -> node_%02lu;\n", i, hash_table->buckets[i].start_index);
    }

    fprintf (file, "}\n");
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template <typename K, typename V>
void __HashTableDrowMinSubgraph (FILE* file, HashTable<K, V>* hash_table,  void (*DataPrinter)(FILE*, HT_Pair<K, V>*))
{   
    
    fprintf (file, "rankdir=\"LR\"; subgraph {\n");
    
    uint64_t last_ht_node = MAX_HT_ELEMENT;
    uint64_t cur_ht_node = MAX_HT_ELEMENT;
    
    for (uint64_t i = 0; i < hash_table->size; i++)
    {   

        if (hash_table->buckets[i].status == BUCKET_NOT_EMPTY)
        {               
            fprintf (file,
                R"(
                    ht_node_%02lu
                    [   
                        fillcolor="#5e6792",
                        style="filled",
                        shape=plaintext, 
                        label= <
                            <table border="0" cellborder="1" cellspacing="0">
                            <tr><td colspan="2">Bucket #%lu</td></tr>
                            <tr>
                                <td>STORED INDEX</td>
                                <td>%lu</td>
                            </tr>
                            <tr>
                                <td>LEN</td>
                                <td>%lu</td>
                            </tr>
                            <tr>
                                <td>STATUS</td>
                                <td>%s</td>
                            </tr>
                            </table>
                        >
                    ];

                )", i, i, hash_table->buckets[i].start_index, hash_table->buckets[i].len,
                (hash_table->buckets[i].status == BUCKET_NOT_EMPTY)? "bucket not empty":"bucket empty / error");

            uint64_t bucket_size = hash_table->buckets[i].len;
            uint64_t val_idx =  hash_table->buckets[i].start_index;

            fprintf(file, "ht_node_%02lu -> list_node_%02lu", i, val_idx);

            for (uint64_t bucket_idx = 0; bucket_idx < bucket_size; bucket_idx++)
            {   
                fprintf (file, 
                    R"(
                        list_node_%02lu 
                        [   
                            fillcolor="#00FF7F",
                            style="filled",
                            shape=plaintext, 
                            label=<
                            <table border="0" cellborder="1" cellspacing="0">
                            <tr><td colspan="2">Node #%lu</td></tr>
                            <tr><td colspan="2">

                    )", val_idx, val_idx);

                DataPrinter (file, &hash_table->values->list[val_idx].data);
                    
                fprintf(file,
                    R"( 
                        </td></tr>
                        <tr>
                            <td port="prev_out" > prev: </td>
                            <td port="prev_in" > %d    </td>
                        </tr>
                        <tr>
                            <td port="next_in" > next: </td>
                            <td port="next_out"> %d    </td>
                        </tr>                            
                        </table>
                        >
                        ];

                    )", 
                    hash_table->values->list[val_idx].prev, 
                    hash_table->values->list[val_idx].next);

                // drow connection for each except first and the last list nodes
                if (val_idx != hash_table->buckets[i].start_index)
                    fprintf(file, "\nlist_node_%02lu:<prev_out> -> list_node_%02d:<prev_in>\n", 
                        val_idx, hash_table->values->list[val_idx].prev);

                if (bucket_idx != bucket_size - 1)
                    fprintf(file, "\nlist_node_%02lu:<next_out> -> list_node_%02d:<next_in>\n", 
                        val_idx, hash_table->values->list[val_idx].next);

                val_idx = (uint64_t) hash_table->values->list[val_idx].next;

                last_ht_node = cur_ht_node;
                cur_ht_node = i;

                if (last_ht_node != cur_ht_node && last_ht_node != MAX_HT_ELEMENT)
                {   
                    fprintf (file, "{rank=same; ht_node_%02lu; ht_node_%02lu;};\n" 
                                   "ht_node_%02lu -> ht_node_%02lu [style=invis];\n", 
                            last_ht_node, cur_ht_node, last_ht_node, cur_ht_node);
                }

            }
        }

    }

    fprintf (file, "}\n");
    
    return;
}

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#undef ASS
#undef POISON
#undef MAX_LIST_ELEMENT
#undef MAX_HT_ELEMENT
#undef HT_SIZE_INCREASE_COEF
#undef HT_INCREASE_DECISION_COEF

//flexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
