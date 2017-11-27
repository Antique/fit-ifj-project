/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *   IFJ09 - Implementace interpretu imperativního jazyka IFJ09              *
 *           spoleèný projekt pøedmìtù IFJ a IAL                             *
 *                                                                           *
 *   Autoøi: Bosternák Balázs, xboste00@stud.fit.vutbr.cz                    *
 *           Hlaváè Jan, xhlava18@stud.fit.vutbr.cz                          *
 *           Hrdina Pavel, xhrdin05@stud.fit.vutbr.cz                        *
 *           Chmiel Filip, xchmie02@stud.fit.vutbr.cz                        *
 *                                                                           *
 *   Datum dokonèení: 13.12.2009                                             *
 *                                                                           *
 *   Soubor: lex.h                                                           *
 *                                                                           *
 *   Popis: Obsahuje definice enumù tokenù a stavù automatu, strukturu pøez  *
 *          kterou pøedává informace parseru a definici funkce automatu.     *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef LEX_H
#define LEX_H

// vycet tokenu
typedef enum {
    T_ADD,
    T_MINUS,
    T_MULT,
    T_DIVIDE,
    T_LESS,
    T_LOE,
    T_GREATER,
    T_GOE,
    T_OR,
    T_EQUAL,
    T_LEFT_BAR,
    T_RIGHT_BAR,
    T_ID,
    T_DOUBLEDOT,
    T_STRING,
    T_COMA,
    T_INT,
    T_DOUBLE,
    T_COMENT,
    T_SET,
    T_SEMICOLON,
    T_KEYWORD,
    T_DOTA,
    T_EOF,
    T_IDDLE,
    T_KW_FINDSTRING,
    T_KW_SORTSTRING,
    T_KW_BEGIN, // klicove slovo - begin
    T_KW_DO, // klicove slovo - do
    T_KW_DOUBLE, // klicove slovo - double
    T_KW_ELSE, // klicove slovo - else
    T_KW_END, // klicove slovo - end
    T_KW_FIND, // klicove slovo - find
    T_KW_IF, // klicove slovo - if
    T_KW_INTEGER, // klicove slovo - integer
    T_KW_READLN, // klicove slovo - readln
    T_KW_SORT, // klicove slovo - sort
    T_KW_STRING, // klicove slovo - string
    T_KW_THEN, // klicove slovo - then
    T_KW_VAR, // klicove slovo - var
    T_KW_WHILE, // klicove slovo - while
    T_KW_WRITE, // klicove slovo - write
} lCodes;

// vycet moznych stavu
typedef enum {
    S_IDDLE,
    S_VOID,
    S_ID,
    S_DOUBLEDOT,
    S_NUM,
    S_STRING,
    S_GREATER,
    S_LESS,
    S_COMENT
} states;

// struktura pro predavani informaci mezi lexem a parserem
typedef struct {
    lCodes code;
    string* key;
} sOut;

// Funkce provede lexikalni kontrolu a vrati dalsi token.
bool lexGetNextToken();

#endif
