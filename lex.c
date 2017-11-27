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
 *   Soubor: lex.c                                                           *
 *                                                                           *
 *   Popis: Obsahuje automat a pomocné funkce pro lexikalní analýzu.         *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "string.h"
#include "lex.h"
#include "defines.h"

extern FILE *input;
extern sOut parserToken;
extern progErrors error;
extern errorMessages errorMsg;

/**
 * Funkce urci, zda je retezec klicove slovo. Pokud ano ulozi do code typ tokenu.
 * @param string* zdrojovy klic, ktery se bude kontrolovat
 * @return bool vraci vysledek kontroly
 */
bool isKeyword(string* src) {
    // kontrola zda jde o nektere z klicovych slov
    if (!strcmp(src->str, "begin"))
        parserToken.code = T_KW_BEGIN;
    else if (!strcmp(src->str, "do"))
        parserToken.code = T_KW_DO;
    else if (!strcmp(src->str, "double"))
        parserToken.code = T_KW_DOUBLE;
    else if (!strcmp(src->str, "div"))
        parserToken.code = T_DIVIDE;
    else if (!strcmp(src->str, "else"))
        parserToken.code = T_KW_ELSE;
    else if (!strcmp(src->str, "end"))
        parserToken.code = T_KW_END;
    else if (!strcmp(src->str, "find"))
        parserToken.code = T_KW_FIND;
    else if (!strcmp(src->str, "if"))
        parserToken.code = T_KW_IF;
    else if (!strcmp(src->str, "integer"))
        parserToken.code = T_KW_INTEGER;
    else if (!strcmp(src->str, "readln"))
        parserToken.code = T_KW_READLN;
    else if (!strcmp(src->str, "sort"))
        parserToken.code = T_KW_SORT;
    else if (!strcmp(src->str, "string"))
        parserToken.code = T_KW_STRING;
    else if (!strcmp(src->str, "then"))
        parserToken.code = T_KW_THEN;
    else if (!strcmp(src->str, "var"))
        parserToken.code = T_KW_VAR;
    else if (!strcmp(src->str, "while"))
        parserToken.code = T_KW_WHILE;
    else if (!strcmp(src->str, "write"))
        parserToken.code = T_KW_WRITE;
    else if (!strcmp(src->str, "find"))
        parserToken.code = T_KW_FINDSTRING;
    else if (!strcmp(src->str, "sort"))
        parserToken.code = T_KW_SORTSTRING;
    else // retezec neni klicove slovo
        return false;
    return true;
}

/**
 * Funkce provede lexikalni kontrolu a vrati dalsi token.
 * @return bool vraci vysledek kontroly
 */
bool lexGetNextToken() {
    // smazani dat predchoziho tokenu
    if(!strClean(parserToken.key))
        return false;

    // inicializace potrebnych promennych
    string* escape = NULL;
    bool numDot = false;
    bool numExponent = false;
    bool numDouble = false;
    bool numExpectingNumber = false;
    bool numPosibleSign = false;
    states nextState = S_IDDLE;

    // nacteni prvniho symbolu
    int symbol = fgetc(input);

    /* zacatek automatu lex */
    while (true) {
        switch (nextState) {
        case S_IDDLE:   // startovni stav
            switch (symbol) {
            case ' ':
            case '\n':
            case '\f':
            case '\r':
            case '\t':  // preskoceni prazdnych znaku
                nextState = S_VOID;
                break;
            case ':':   // nalezeni zacatku dvojtecky
                nextState = S_DOUBLEDOT;
                break;
            case ';':   // koncovy stav nalezen token strednik
                parserToken.code = T_SEMICOLON;
                return true;
            case ',':   // koncovy stav nalezen token carka
                parserToken.code = T_COMA;
                return true;
            case '.':   // koncovy stav nalezen token tecka
                parserToken.code = T_DOTA;
                return true;
            case '\'':  // nalezeni zacatku stringu
                nextState = S_STRING;
                break;
            case '>':   // nalezeni zacatku vetsi
                nextState = S_GREATER;
                break;
            case '<':   // nalezeni zacatku mensi
                nextState = S_LESS;
                break;
            case '=':   // koncovy stav nalezene token rovno
                parserToken.code = T_EQUAL;
                return true;
            case '+':   // koncovy stav nalezen toke plus
                parserToken.code = T_ADD;
                return true;
            case '-':   // koncovy stav nalezen token minus
                parserToken.code = T_MINUS;
                return true;
                break;
            case '*':   // koncovy stav nalezen token nasobeni
                parserToken.code = T_MULT;
                return true;
            case '(':   // koncovy stav nalezen token leva zavorka
                parserToken.code = T_LEFT_BAR;
                return true;
            case ')':   // koncovy stav nalezen token prava zavorka
                parserToken.code = T_RIGHT_BAR;
                return true;
            case '{':   // nalezen zacatek komentare
                nextState = S_COMENT;
                break;
            case EOF:   // koncovy stav nalezen token eof
                parserToken.code = T_EOF;
                return true;
            default:    // ostatni stavy
                if ((symbol == '_') || (isalpha(symbol))) {
                    nextState = S_ID;   // nalezeni zacatku identifikatoru
                }
                else if (isdigit(symbol)) {
                    nextState = S_NUM;  // nalezeni zacatku cisla
                }
                else {
                    error = LEX_ERROR;  // nalezeni nepodporovaneho znaku
                    errorMsg = UNKNOWN_CHAR_MSG;
                    return false;
                }
            }
            break;
        case S_ID:      // koncovy stav nalezeni tokenu id
            strAddChar(parserToken.key, symbol);

            while (true) {
                symbol = fgetc(input);
                // nacitani jen znaku ktere muzou byt v identifikatoru
                if ((isalpha(symbol)) || (isdigit(symbol))) {
                    strAddChar(parserToken.key, symbol);
                }
                else {
                    ungetc(symbol, input);
                    // nastaveni ze se jedna o klicove slovo nebo identifikator
                    if (isKeyword(parserToken.key) == false) {
                        parserToken.code = T_ID;
                    }
                    return true;
                }
            }
        case S_DOUBLEDOT:
            symbol = fgetc(input);
            // kontrola jestli se jedna o prirazeni nebo jen dvojtecku
            if (symbol == '=') {
                parserToken.code = T_SET;
                return true;
            }
            else {
                ungetc(symbol, input);
                parserToken.code = T_DOUBLEDOT;
                return true;
            }
        case S_NUM:
            strAddChar(parserToken.key, symbol);
            /* automat na nacteni cisla */
            while (true) {
                symbol = fgetc(input);
                if (isdigit(symbol)) {  // dalsi simbol je cislo
                    strAddChar(parserToken.key, symbol);
                    numExpectingNumber = false;
                    numPosibleSign = false;
                }
                else if (symbol == '.') {
                    // exponent je po nalezeni tecky ukonceny spravne
                    if (numExponent) {
                        ungetc(symbol, input);
                        break;
                    }
                    // tecka muze nasledovat pouze za znakem cislo a muze byt
                    // v celem cisle jen jednou
                    if (numExpectingNumber || numDot) {
                        error = LEX_ERROR;
                        errorMsg = BAD_NUM_MSG;
                        return false;
                    }
                    strAddChar(parserToken.key, symbol);
                    numExpectingNumber = true;
                    numDot = true;
                    numDouble = true;
                }
                else if (symbol == 'e' || symbol == 'E') {
                    // znak exponenut muze nasledovat jen za znakem cisla
                    // a muze se nachazet v cisle jen jednou
                    if (numExpectingNumber || numExponent) {
                        error = LEX_ERROR;
                        errorMsg = BAD_NUM_MSG;
                        return false;
                    }
                    strAddChar(parserToken.key, symbol);
                    numExpectingNumber = true;
                    numPosibleSign = true;
                    numExponent = true;
                    numDouble = true;
                }
                else if (symbol == '+' || symbol == '-') {
                    // znamenko se muze nachazet pouze za znakem exponentu
                    // a muze byt pouze jen jednou
                    if (!numPosibleSign) {
                        ungetc(symbol, input);
                        break;
                    }
                    strAddChar(parserToken.key, symbol);
                    numPosibleSign = false;
                }
                else {
                    // za cislem muze byt znak jen kdyz bude nasledovat div
                    if (isalpha(symbol)) {
                        if (symbol != 'd') {
                            error = LEX_ERROR;
                            errorMsg = ID_UNEXPECT_MSG;
                            return false;
                        }
                        symbol = fgetc(input);
                        if (symbol != 'i') {
                            error = LEX_ERROR;
                            errorMsg = ID_UNEXPECT_MSG;
                            return false;
                        }
                        symbol = fgetc(input);
                        if (symbol != 'v') {
                            error = LEX_ERROR;
                            errorMsg = ID_UNEXPECT_MSG;
                            return false;
                        }
                        symbol = fgetc(input);
                        // za divem nesmi nasledovat cislo nebo identifikator
                        if (isdigit(symbol) || isalpha(symbol) || symbol == '_') {
                            error = LEX_ERROR;
                            errorMsg = ID_UNEXPECT_MSG;
                            return false;
                        }
                        fseek(input, -4, SEEK_CUR);
                        break;
                    }   // jinak je cislo ukoncene spravne
                    else {
                        ungetc(symbol, input);
                        break;
                    }
                }
            }
            /* konec automatu na nacteni cisla */
            // pokud se jedna o cislo double
            if (numDouble == true) {
                // kontrola na preteceni
                if (atof(parserToken.key->str) == HUGE_VAL ||
                    atof(parserToken.key->str) == -HUGE_VAL ||
                    numExpectingNumber) {
                    error = LEX_ERROR;
                    errorMsg = NUM_OVERFLOW_MSG;
                    return false;
                }
                parserToken.code = T_DOUBLE;
            }   // jinak se jedna o integer
            else {
                // kontrola na preteceni
                if (atoi(parserToken.key->str) < 0) {
                    error = LEX_ERROR;
                    errorMsg = NUM_OVERFLOW_MSG;
                    return false;
                }
                parserToken.code = T_INT;
            }
            return true;
        case S_STRING:
            /* automat pro nacteni stringu */
            while (true) {
                symbol = fgetc(input);
                // nalezeni konce stringu nebo zacatek escape sekvence nebo
                // vlozeni symbolu '
                if (symbol == '\'') {
                    symbol = fgetc(input);
                    if (symbol == '#') {    // nasleduje escape sekvence
                        escape = strInit();
                        symbol = fgetc(input);
                        // nacteni vsech cisel
                        while(isdigit(symbol)) {
                            strAddChar(escape, symbol);
                            symbol = fgetc(input);
                        }
                        // musi byt ukoncene zacatkem stringu
                        if(symbol != '\'') {
                            error = LEX_ERROR;
                            errorMsg = STR_EXPECT_MSG;
                            strFree(escape);
                            return false;
                        }
                        // prevod na cislo a kontrola jestli je v rozmezi
                        symbol = atoi(escape->str);
                        strFree(escape);
                        if (symbol < 1 || symbol > 31) {
                            error = LEX_ERROR;
                            errorMsg = BAD_ESCAPE_MSG;
                            return false;
                        }
                    }   // pokud se nejedna o vlozeni znaku ' tak je konec stringu
                    else if(symbol != '\'') {
                        ungetc(symbol, input);
                        parserToken.code = T_STRING;
                        return true;
                    }
                }   // pokud narazi na eof nebo nepovoleny znak pred koncem
                // strignu tak se jedna o chybu
                else if (symbol == EOF || symbol < 32) {
                    error = LEX_ERROR;
                    errorMsg = BAD_CHAR_MSG;
                    return false;
                }
                strAddChar(parserToken.key, symbol);
            }
            break;
        case S_GREATER:
            symbol = fgetc(input);
            // jedna se bud o >= nebo o >
            if (symbol == '=') {
                parserToken.code = T_GOE;
                return true;
            }
            else {
                ungetc(symbol, input);
                parserToken.code = T_GREATER;
                return true;
            }
        case S_LESS:
            symbol = fgetc(input);
            // jedna se bud o <= nebo o <> nebo o <
            if (symbol == '=') {
                parserToken.code = T_LOE;
                return true;
            }
            else if (symbol == '>') {
                parserToken.code = T_OR;
                return true;
            }
            else {
                ungetc(symbol, input);
                parserToken.code = T_LESS;
                return true;
            }
        case S_COMENT:
            // zahozeni celeho komentare
            while (symbol != '}') {
                if(symbol == EOF) {
                    error = LEX_ERROR;
                    errorMsg = EOF_UNEXPECT_MSG;
                    return false;
                }
                symbol = fgetc(input);
            }
        case S_VOID:
            symbol = fgetc(input);
            nextState = S_IDDLE;
            break;
        default:
            return false;
            break;
        }
    }
    /* konec automatu lex */
}
