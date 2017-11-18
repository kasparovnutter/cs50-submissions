/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define CHARSET 27


enum specchars{
    apostrophe = 26,
};

#include "dictionary.h"

FILE* dic;

typedef struct _node{
    bool isword;
    struct _node* prev;
    struct _node* next[CHARSET];
}node;

node* root = NULL;
node* curs = NULL;
static int count = 0;
/**
 * Returns true if word is in dictionary else false.
 */

 // (remember index is CHARSET
 //  0 - 26 (a-z)
 //  27 ('\'')
int token(const char a){
    int chkidx = 0;
    if (a == '\''){
        chkidx = apostrophe;
        return chkidx;
    }
    chkidx = tolower(a) - 'a';
    return chkidx;
}

bool _check(const char* a, int len, node* b){
    //check if node has a pointer to current letter
    //if letter found, b->next[indexed letter]


    for (int i = 0; i < len; i++){
        //chklet = indexified letter of *word
        int chklet = token(a[i]);

        // b->next[chklet] == current letter you're checking//
        //
        // (e.g) a = root-> R -> U -> L -> E
        //       b = root-> R -> U -> L -> NULL  (aka dictionary entry doesn't exist)
        //       b->next[ ] = R
        //       b->next[ ] = U
        //       b->next[ ] = L
        //       bb->next[ ] = NULL
        //
        if (b->next[chklet] != NULL){
            b = b->next[chklet];
            //fprintf(stdout, "%c", chklet + 'a');
            continue;
        }
        return false;
    }
    //fprintf(stdout, " ");
    return (b->isword == true)? true : false;
}

bool check(const char *word)
{
    int len = strlen(word);
    node* temp = root;
    bool flag = _check(word, len, temp);
    return flag;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    dic = fopen(dictionary, "r");
    if (dic == NULL){
        return false;
    }

    //                      curs->prev       curs
    //      curs->prev      curs            malloc
    //      root >>     root->next[wi] >>
    //      [1]2[3]4    2 -> [1]2[3]4


    //
        // if index == 0
        //      isalpha(c)
        //      temp = root (root->next not malloc'd)
        //      (if) create to root->next[windex]
        //          temp = root (with root->next malloc'd)
        //      go to root->next[windex]
        //          temp = root;
        //          root = root->next[windex];
        //          root->prev = temp;
        //          curs = root;
        //  if index > 0
        //      '\'''
        //          index++
        //          (if) create curs->next[windex]
        //          go to curs->next[windex]
        //              temp = curs;
        //              curs = curs->next[windex]
        //
        //
        //      ''\0' or '\n'
        //          index = 0
        //          curs->isword = true
        //          curs = root

    root = calloc(1, sizeof(node));

    // get->check each character from file
    // index = position of word
    // windex = tolower(c) - 'a' || 26 if its an apostrophe
    // indexstrt =
    for (int c = fgetc(dic), index = 0 , windex = 0; c != EOF; c = fgetc(dic)){

    //=================================
            // if index = 0 and isalpha
            if (index == 0 && isalpha(c)){
                windex = tolower(c) - 'a';
                //malloc root->next[windex] if needed
                if (root->next[windex] == NULL){
                    root->next[windex] = calloc(1, sizeof(node));
                    if (root->next[windex] == NULL){
                        fprintf(stderr,"(%d) malloc err [root]\n", __LINE__);
                        return false;
                    }
                }

                // set root->prev to current
                // go to root->next[windex]
                // index++

                if (root->next[windex]){
                    curs = root->next[windex];
                    curs->prev = root;
                    index++;
                }
            }
            //===========
            // if index > 0
            else if (index > 0){
                windex = (c == '\'') ? apostrophe : tolower(c) - 'a' ;
                // if c == '\'''
                if (c == '\'' || isalpha(c)){
                    // malloc curs->next[windex]
                    if (curs->next[windex] == NULL){
                        curs->next[windex] = calloc(1, sizeof(node));
                        if (curs->next[windex] == NULL){
                            fprintf(stderr,"(%d) malloc err [curs]\n", __LINE__);
                            return false;
                        }
                    }
                    if (curs->next[windex]){
                        node* temp = curs;
                        curs = curs->next[windex];
                        curs->prev = temp;
                        index++;
                    }
                }
                else if (c == '\0' || c == '\n'){
                    curs->isword = true;
                    // rewind current curs (+ children, all calloc'd) till root position
                    //
                    // update root with current curs
                    while (curs != root){
                        curs = curs->prev;
                    }
                    root = curs;
                    index = 0;
                }
            }

    //=================================
    }


return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */


//SIZE
//=====================
void _count(node* a){
    if (a->isword == true){
        count++;
    }

    if (a != NULL){
        for (int i = 0; i < CHARSET; i++){
            if (a->next[i] == NULL){
                continue;
            }
            _count(a->next[i]);
        }
    }
}

unsigned int size(void)
{
    if (root == NULL){
        return 0;
    }
    node* temp = root;
    _count(temp);

    return count;
}
//=====================


/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
//=====================
 void _unload(node* a){

    for (int i = 0; i < CHARSET; i++){
        if (a->next[i] == NULL){
            continue;
        }
        _unload(a->next[i]);
    }
    //<while> ignores first 'isword == true'
    node* b = a->prev;
    free(a);
    a = b;
}

bool unload(void)
{
    // the 0 is a placeholder statement
    (root) ? _unload(root) : 0;
    (dic) ? fclose(dic), dic = NULL : 0;
    return (root && dic)? false: true;
}
//=====================

