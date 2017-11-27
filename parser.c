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
 *   Soubor: parser.c                                                        *
 *                                                                           *
 *   Popis: Obsahuje hlavní a pomocné funkce pro syntaktickou analýzu        *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
#include "hash.h"
#include "interpret.h"
#include "lex.h"
#include "parser.h"
#include "stack.h"
#include "defines.h"

extern FILE *input;
extern htableTable *h_table;
extern iList tripleAddressL;
extern sOut parserToken;
extern progErrors error;
extern errorMessages errorMsg;

// pomocne globalni promenne
htableItem* tempItem;
uData tempData;

// precedentni tabulka
precedentKey precedentTable[14][14] = {
    /*          +      -      *      /      <      <=     >      =>     <>     =      (      )      i      $  */
    /*  + */ {SRigh, SRigh, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  - */ {SRigh, SRigh, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  * */ {SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  / */ {SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  < */ {Sleft, Sleft, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /* <= */ {Sleft, Sleft, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  > */ {Sleft, Sleft, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /* => */ {Sleft, Sleft, Sleft, Sleft, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /* <> */ {Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, SRigh, SRigh, Sleft, SRigh, Sleft, SRigh},
    /*  = */ {Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, SRigh, Sleft, SRigh},
    /*  ( */ {Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, SEqua, Sleft, SErro},
    /*  ) */ {SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SErro, SRigh, SErro, SRigh},
    /*  i */ {SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SRigh, SErro, SRigh, SErro, SRigh},
    /*  $ */ {Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, Sleft, SErro, Sleft, SEqua},
};

// tabulka pro kontrolu spravnych typu pri scitani
bool checkAddTable[3][3] = {
    /*       I      D      S*/
    /*I*/ {true, true, false},
    /*D*/ {true, true, false},
    /*S*/ {false, false, true}
};

// tabulka pro kontrolu spravnych typu pri odcitani a nasobeni
bool checkSubMulTable[3][3] = {
    /*       I      D      S*/
    /*I*/ {true, true, false},
    /*D*/ {true, true, false},
    /*S*/ {false, false, false}
};

// tabulka pro zjisteni vysledneho typu po provedeni operace
dataType checkTypeTable[3][3] = {
    /*        I      D      S*/
    /*I*/ {H_INT, H_DBL, H_NON},
    /*D*/ {H_DBL, H_DBL, H_NON},
    /*S*/ {H_NON, H_NON, H_STR}
};

/**
 * Pripraveni dat ze vstupu pro ulozeni na zasobnik
 * @param sItem* ukazatel kam se ulozi pripravene data
 * @param int* ukazatel na pocet nactenych levych zavorek
 * @return bool vrati informaci o spravnem provedeni
 */
bool parserPrepareSourceData(sItem* exprData, int* numLeftBar) {
    switch (parserToken.code) {
        // pripraveni operace
    case T_ADD:
    case T_MINUS:
    case T_MULT:
    case T_DIVIDE:
    case T_GOE:
    case T_GREATER:
    case T_LOE:
    case T_LESS:
    case T_OR:
    case T_EQUAL:
    case T_LEFT_BAR:
    case T_RIGHT_BAR:
        // pri nacteni leve zavorky se zvysi pocet nactenych zavorek
        if (parserToken.code == T_LEFT_BAR)
            (*numLeftBar)++;
        // pri nacteni prave zavorky se zkontroluje pocet levych, pokud je 0
        // tak vyraz konci, jinak se tento pocet snizi
        if (parserToken.code == T_RIGHT_BAR) {
            if(*numLeftBar)
                (*numLeftBar)--;
            else {
                exprData->type = S_END;
                break;
            }
        }
        exprData->type = parserToken.code;
        break;
        // pri nacteni promenne nebo constanty se nastavi typ S_IDENTIFIER
        // a u konstatn se provede prevod na patricny typ dat
    case T_ID:
        exprData->item = htableSearch(h_table, parserToken.key);
        if (!exprData->item) {
            error = SEM_ERROR;
            errorMsg = ID_UNDECLARED_MSG;
            return false;
        }
        exprData->type = S_IDENTIFIER;
        break;
    case T_INT:
        tempData.iVal = atoi(parserToken.key->str);
        exprData->type = S_IDENTIFIER;
        exprData->item = htableInsert(h_table, H_INT, tempData, NULL, CONST);
        break;
    case T_DOUBLE:
        tempData.dVal = atof(parserToken.key->str);
        exprData->type = S_IDENTIFIER;
        exprData->item = htableInsert(h_table, H_DBL, tempData, NULL, CONST);
        break;
    case T_STRING:
        tempData.sVal = parserToken.key;
        exprData->type = S_IDENTIFIER;
        exprData->item = htableInsert(h_table, H_STR, tempData, NULL, CONST);
        break;
        // seznam koncovych stavu
    case T_COMA:
    case T_KW_BEGIN:
    case T_KW_DOUBLE:
    case T_KW_ELSE:
    case T_KW_FIND:
    case T_KW_IF:
    case T_KW_INTEGER:
    case T_KW_READLN:
    case T_KW_SORT:
    case T_KW_STRING:
    case T_KW_VAR:
    case T_KW_WHILE:
    case T_KW_WRITE:
    case T_SEMICOLON:
    case T_KW_THEN:
    case T_KW_DO:
    case T_KW_END:
        exprData->type = S_END;
        break;
        // ostatni stavy ve vyrazu vyvolaji syntaktickou chybu
    default:
        error = SYN_ERROR;
        errorMsg = TOKEN_UNEXPECT_MSG;
        return false;
    }
    return true;
}

/**
 * Zkontroluje syntaktickou a semantickou spravnost operace
 * @param sItem prvni prvek
 * @param sItem druhy prvek
 * @param dataType* ukazatel na vysledny typ dat operace
 * @param sType kontrolovana operace
 * @return bool vrati informaci o spravnem provedeni
 */
bool parserCheckOperation(sItem sData1, sItem sData2, dataType* type, sType operation) {
    bool ok = false;
    // operace muze probihat jen mezi nonterminalama - syntakticka kontrola
    if (sData1.type == S_NONTERMINAL && sData2.type == S_NONTERMINAL) {
        *type = checkTypeTable[sData1.item->type][sData2.item->type];
        switch (operation) {
        case S_PLUS:
            ok = checkAddTable[sData1.item->type][sData2.item->type];
            break;
        case S_MINUS:
            ok = checkSubMulTable[sData1.item->type][sData2.item->type];
            break;
        case S_MULTIPLY:
            ok = checkSubMulTable[sData1.item->type][sData2.item->type];
            break;
        case S_DIVIDE:
            ok = sData1.item->type == H_INT && sData2.item->type == H_INT;
            break;
        default:
            *type = H_INT;
            ok = sData1.item->type == sData2.item->type;
            break;
        }
        // semanticka kontrola operace
        if (!ok || *type == H_NON) {
            error = SEM_ERROR;
            errorMsg = INCOMPAT_TYPE_MSG;
            return false;
        }
        return true;
    }
    error = SYN_ERROR;
    errorMsg = BAD_EXPRESSION_MSG;
    return false;
}

/**
 * Provede pravidlo mezi dvema nonterminalama
 * @param sItem nalezena operace na zasobniku
 * @param sStack* ukazatel na zasobnik
 * @return bool vrati informaci o spravnem provedeni
 */
bool parserDoRule(sItem exprData, sStack* stack) {
    dataType type = H_NON;
    sItem sData1, sData2, sData3;
    switch (exprData.type) {
        // provede pravidlo pro binarni operace
    case S_PLUS:
    case S_MINUS:
    case S_MULTIPLY:
    case S_DIVIDE:
    case S_LES:
    case S_LESSSAME:
    case S_MORE:
    case S_MORESAME:
    case S_OR:
    case S_SAME:
        sTopPop(stack, &sData1);
        sPop(stack);
        sTopPop(stack, &sData2);
        if (!parserCheckOperation(sData1, sData2, &type, exprData.type))
            return false;
        sData3.type = S_NONTERMINAL;
        switch(type) {
        case H_INT:
            tempData.iVal = 0;
            break;
        case H_DBL:
            tempData.dVal = 0.0;
            break;
        case H_STR:
            tempData.sVal = NULL;
            break;
        default:
            break;
        }
        sData3.item = htableInsert(h_table, type, tempData, NULL, CONST);
        if(!iInsertInstruction(&tripleAddressL, exprData.type, sData2.item, sData1.item, sData3.item))
            break;
        sPush(stack, sData3);
        break;
        // zahodi levou zavorku ze saboniku, uz neni potreba pro vykonavani pravidel
    case S_LBAR:
        sTopPop(stack, &sData3);
        sPop(stack);
        sPush(stack, sData3);
        break;
        // prava zavorka se na zasobniku nikdy nevyskytne
    case S_RBAR:
        break;
        // zmeni identifikator na nonterminal
    case S_IDENTIFIER:
        sTopPop(stack, &sData3);
        sData3.type = S_NONTERMINAL;
        sPush(stack, sData3);
        break;
        // posledni nonterminal ze zasobniku preda jako vysledek
    case S_END:
        sTopPop(stack, &sData3);
        if(sData3.type == S_NONTERMINAL)
            tempItem = sData3.item;
        else {
            error = SYN_ERROR;
            errorMsg = BAD_EXPRESSION_MSG;
        }
        sTopPop(stack, &sData2);
        if(sData2.type != S_END) {
            error = SYN_ERROR;
            errorMsg = BAD_EXPRESSION_MSG;
        }
        break;
        // jina operace vyvola syntaktickou chybu
    default:
        break;
    }
    if(error)
        return false;
    return true;
}

/**
 * Nalezeni operace, ktere se bude vykonavat
 * @param sStack* ukazatel na zasobnik
 * @param sItem* ukazatel kam se ulozi nalezena operace
 */
void parserFindOperation(sStack* s, sItem* d) {
    sItem helpData;
    sStack helpStack;
    sInit(&helpStack);
    while (true) {
        sTop(s, d);
        // preckosi nonterminaly a prvni terminal urcuje operaci
        if (d->type == S_NONTERMINAL) {
            sPush(&helpStack, *d);
            sPop(s);
        } else {
            while (!sIsEmpty(&helpStack)) {
                sTopPop(&helpStack, &helpData);
                sPush(s, helpData);
            }
            sDelete(&helpStack);
            break;
        }
    }
}

/**
 * Funkce volana pro spusteni syntakticke analyzy
 * @return bool informace o provedeni
 */
bool parser() {
    if (!lexGetNextToken())               // nacteni prvniho tokenu
        return false;
    if(!parserProgram())                  // spusteni syntakticke analyzy
        return false;
    return true;
}

/**
 * Funkce reprezentujici nonterminal <program>
 * @return bool informace o provedeni
 */
bool parserProgram() {
    // ### simulace pravidla 1 ###
    // <program> --> <declrList> begin <stateList> end . eof
    if (parserToken.code == T_KW_BEGIN || parserToken.code == T_KW_VAR) {
        if (!parserDeclrList())               // <declrList>
            return false;
        if (parserToken.code != T_KW_BEGIN) { // begin
            error = SYN_ERROR;
            errorMsg = BEGIN_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!parserStateList())               // <stateList>
            return false;
        if (parserToken.code != T_KW_END) {   // end
            error = SYN_ERROR;
            errorMsg = END_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_DOTA) {     // .
            error = SYN_ERROR;
            errorMsg = DOT_EXPECT_MSG;
            return false;
        }
        if (!iInsertInstruction(&tripleAddressL, I_ENDDOT, NULL, NULL, NULL))
            return false;
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_EOF) {      // 'EOF'
            error = SYN_ERROR;
            errorMsg = EOF_EXPECT_MSG;
            return false;
        }
        return true;
    }
    error = SYN_ERROR;
    errorMsg = VAR_OR_BEGIN_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <declrList>
 * @return bool informace o provedeni
 */
bool parserDeclrList() {
    // ### simulace pravidla 2 ###
    // <declrList> --> var id : <dataType> ; <declare>
    if (parserToken.code == T_KW_VAR) {
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_ID) {       // id
            error = SYN_ERROR;
            errorMsg = ID_EXPECT_MSG;
            return false;
        }
        string* tempString = strInit();
        if (!strCopy(tempString, parserToken.key)) {
            strFree(tempString);
            return false;
        }
        if (!lexGetNextToken()) {              // nacteni noveho tokenu
            strFree(tempString);
            return false;
        }
        if (parserToken.code != T_DOUBLEDOT) { // :
            error = SYN_ERROR;
            errorMsg = DBLDOT_EXPECT_MSG;
            strFree(tempString);
            return false;
        }
        if (!lexGetNextToken()) {              // nacteni noveho tokenu
            strFree(tempString);
            return false;
        }
        if (!parserDataType(tempString)) {     // <dataType>
            strFree(tempString);
            return false;
        }
        strFree(tempString);
        if (parserToken.code != T_SEMICOLON) { // ;
            error = SYN_ERROR;
            errorMsg = SEMCOL_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserDeclare())                  // <declare>
            return false;
        return true;
    }
    // ### simulace pravidla 3 ###
    // <declrList> --> [epsilon]
    else
        return true;
}

/**
 * Funkce reprezentujici nonterminal <declare>
 * @return bool informace o provedeni
 */
bool parserDeclare() {
    // ### simulace pravidla 4 ###
    // <declare> --> id : <dataType> ; <declare>
    if (parserToken.code == T_ID) {
        string* tempString = strInit();
        if (!strCopy(tempString, parserToken.key)) {
            strFree(tempString);
            return false;
        }
        if (!lexGetNextToken()) {              // nacteni noveho tokenu
            strFree(tempString);
            return false;
        }
        if (parserToken.code != T_DOUBLEDOT) { // :
            error = SYN_ERROR;
            errorMsg = DBLDOT_EXPECT_MSG;
            strFree(tempString);
            return false;
        }
        if (!lexGetNextToken()) {              // nacteni noveho tokenu
            strFree(tempString);
            return false;
        }
        if (!parserDataType(tempString)) {     // <dataType>
            strFree(tempString);
            return false;
        }
        strFree(tempString);
        if (parserToken.code != T_SEMICOLON) { // ;
            error = SYN_ERROR;
            errorMsg = SEMCOL_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserDeclare())                  // <declare>
            return false;
        return true;
    }
    // ### simulace pravidla 5 ###
    // <declare> --> [epsilon]
    else
        return true;
}

/**
 * Funkce reprezentujici nonterminal <dataType>
 * @param string* ukazatel na nazev promenne
 * @return bool informace o provedeni
 */
bool parserDataType(string* tempString) {
    uData insert;
    // ### simulace pravidla 6 ###
    // <dataType> --> integer
    if (parserToken.code == T_KW_INTEGER) {
        insert.iVal = 0;
        // ulozeni deklarovane promenne do tabulky symbolu
        if (!htableInsert(h_table, H_INT, insert, tempString, VAR)) {
            if (error)
                return false;
            error = SEM_ERROR;
            errorMsg = REDECLARE_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        return true;
    }
    // ### simulace pravidla 7 ###
    // <dataType>  --> double
    else if (parserToken.code == T_KW_DOUBLE) {
        insert.dVal = 0;
        // ulozeni deklarovane promenne do tabulky symbolu
        if (!htableInsert(h_table, H_DBL, insert, tempString, VAR)) {
            if (error)
                return false;
            error = SEM_ERROR;
            errorMsg = REDECLARE_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        return true;
    }
    // ### simulace pravidla 8 ###
    // <dataType>  --> string
    else if (parserToken.code == T_KW_STRING) {
        insert.sVal = NULL;
        // ulozeni deklarovane promenne do tabulky symbolu
        if (!htableInsert(h_table, H_STR, insert, tempString, VAR)) {
            if (error)
                return false;
            error = SEM_ERROR;
            errorMsg = REDECLARE_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        return true;
    }
    error = SYN_ERROR;
    errorMsg = DECLARE_TYPE_EXPECTED_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <stateList>
 * @return bool informace o provedeni
 */
bool parserStateList() {
    // ### simulace pravidla 9 ###
    // <stateList> --> <state> <stateList2>
    if (parserToken.code == T_KW_READLN || parserToken.code == T_KW_WRITE ||
        parserToken.code == T_KW_IF     || parserToken.code == T_KW_WHILE ||
        parserToken.code == T_ID        || parserToken.code == T_KW_BEGIN) {
        if (!parserState())              // <state>
            return false;
        if (!parserStateList2())         // <stateList2>
            return false;
        return true;
    }
    // ### simulace pravidla 10 ###
    // <stateList> --> [epsilon]
    else
        return true;
}

/**
 * Funkce reprezentujici nonterminal <stateList2>
 * @return bool informace o provedeni
 */
bool parserStateList2() {
    // ### simulace pravidla 11 ###
    // <stateList2> --> ; <state> <stateList2>
    if (parserToken.code == T_SEMICOLON) {
        // ; - byl jiz prijat
        if (!lexGetNextToken())          // nacteni noveho tokenu
            return false;
        if (!parserState())              // <state>
            return false;
        if (!parserStateList2())         // <stateList2>
            return false;
        return true;
    }
    // ### simulace pravidla 12 ###
    // <stateList2> --> [epsilon]
    else
        return true;
}

/**
 * Funkce reprezentujici nonterminal <state>
 * @return bool informace o provedeni
 */
bool parserState() {
    // ### simulace pravidla 13 ###
    // <state> --> readln ( <idList> )
    if (parserToken.code == T_KW_READLN) {
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_LEFT_BAR) {  // (
            error = SYN_ERROR;
            errorMsg = LEFT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserIdList())                   // <idList>
            return false;
        if (parserToken.code != T_RIGHT_BAR) { // )
            error = SYN_ERROR;
            errorMsg = RIGHT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        // uspesna syntakticka analyza
        return true;
    }
    // ### simulace pravidla 14 ###
    // <state> --> write ( <exprList> )
    else if (parserToken.code == T_KW_WRITE) {
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_LEFT_BAR) {  // (
            error = SYN_ERROR;
            errorMsg = LEFT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserExprList())                 // <exprList>
            return false;
        if (parserToken.code != T_RIGHT_BAR) { // )
            error = SYN_ERROR;
            errorMsg = RIGHT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        return true;
    }
    // ### simulace pravidla 15 ###
    // <state> --> if <expression> then <state> else <state>
    else if (parserToken.code == T_KW_IF) {
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserExpresionAnalize())         // <expression>
            return false;
        if (tempItem->type != H_INT) {
            error = SEM_ERROR;
            errorMsg = INT_EXPR_EXPECT_MSG;
            return false;
        }
        // Vytvorim novou pomocnou promennou pro vysledek negace
        tempData.iVal = 0;
        htableItem* negResult = htableInsert(h_table, H_INT, tempData, NULL, CONST);
        if(!negResult)
            return false;
        // Vlozeni instrukce pro negaci
        if (!iInsertInstruction(&tripleAddressL, I_NOT, tempItem, NULL, negResult))
            return false;
        // Vytvoreni nove zalozky lab1
        tempData.lVal = NULL;
        htableItem* lab1 = htableInsert(h_table, H_LAB, tempData, NULL, LABEL);
        if(!lab1)
            return false;
        // Vytvoreni nove zalozky lab2
        tempData.lVal = NULL;
        htableItem* lab2 = htableInsert(h_table, H_LAB, tempData, NULL, LABEL);
        if(!lab2)
            return false;
        // Vlozeni podmineneho skoku na zacatek else
        if (!iInsertInstruction(&tripleAddressL, I_GOTO, negResult, NULL, lab1))
            return false;
        if (parserToken.code != T_KW_THEN) {   // then
            error = SYN_ERROR;
            errorMsg = THEN_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken()) {              // nacteni noveho tokenu
            return false;
        }
        if (!parserState()) {                  // <state>
            return false;
        }
        // Vlozeni skoku na konec else
        if (!iInsertInstruction(&tripleAddressL, I_GOTO, NULL, NULL, lab2)) {
            return false;
        }
        // Vlozeni zalozky zacatek else
        if (!iInsertInstruction(&tripleAddressL, I_LAB, NULL, NULL, lab1))
            return false;
        lab1->value.lVal = tripleAddressL.last;
        if (parserToken.code != T_KW_ELSE) {   // else
            error = SYN_ERROR;
            errorMsg = ELSE_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserState())                    // <state>
            return false;
        // Vlozeni zalozky konec else
        if (!iInsertInstruction(&tripleAddressL, I_LAB, NULL, NULL, lab2))
            return false;
        lab2->value.lVal = tripleAddressL.last;
        return true;
    }
    // ### simulace pravidla 16 ###
    // <state> --> while <expression> do <state>
    else if (parserToken.code == T_KW_WHILE) {
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        // Vytvoreni nove zalozky lab1
        tempData.lVal = NULL;
        htableItem* lab1 = htableInsert(h_table, H_LAB, tempData, NULL, LABEL);
        if(!lab1)
            return false;
        // Vlozeni zalozky zacatek cyklu
        if (!iInsertInstruction(&tripleAddressL, I_LAB, NULL, NULL, lab1))
            return false;
        lab1->value.lVal = tripleAddressL.last;
        if (!parserExpresionAnalize())         // <expression>
            return false;
        // nalezeni vysledku <expression> v tabulce symbolu
        if (tempItem->type != H_INT) {
            error = SEM_ERROR;
            errorMsg = INT_EXPR_EXPECT_MSG;
            return false;
        }
        // Vytvoreni novou pomocnou promennou pro vysledek negace
        tempData.iVal = 0;
        htableItem* negResult = htableInsert(h_table, H_INT, tempData, NULL, CONST);
        if(!negResult)
            return false;
        // Vlozeni negace
        if (!iInsertInstruction(&tripleAddressL, I_NOT, tempItem, NULL, negResult))
            return false;
        // Vytvoreni nove zalozky lab2
        tempData.lVal = NULL;
        htableItem* lab2 = htableInsert(h_table, H_LAB, tempData, NULL, LABEL);
        if(!lab2)
            return false;
        // Vlozeni podmineneho skoku za konec cyklu
        if (!iInsertInstruction(&tripleAddressL, I_GOTO, negResult, NULL, lab2))
            return false;
        if (parserToken.code != T_KW_DO) {     // do
            error = SYN_ERROR;
            errorMsg = DO_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserState())                    // <state>
            return false;
        // Vlozeni skoku pred vyhodnoceni podminky cyklu
        if (!iInsertInstruction(&tripleAddressL, I_GOTO, NULL, NULL, lab1))
            return false;
        // Vlozeni zalozky za konec cyklu
        if (!iInsertInstruction(&tripleAddressL, I_LAB, NULL, NULL, lab2))
            return false;
        lab2->value.lVal = tripleAddressL.last;
        return true;
    }
    // ### simulace pravidla 17 ###
    // <state> --> id := <value>
    if (parserToken.code == T_ID) {
        // vyhledani promenne v tabulce symbolu
        htableItem* temp = htableSearch(h_table, parserToken.key);
        if (!temp) {
            error = SEM_ERROR;
            errorMsg = ID_UNDECLARED_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_SET) {       // :=
            error = SYN_ERROR;
            errorMsg = ASSIG_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserValue(temp))                // <value>
            return false;
        return true;
    }
    // ### simulace pravidla 18 ###
    // <state> --> begin <stateList> end
    if (parserToken.code == T_KW_BEGIN) {
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        if (!parserStateList())                // <stateList>
            return false;
        if (parserToken.code != T_KW_END) {    // end
            error = SYN_ERROR;
            errorMsg = END_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())                // nacteni noveho tokenu
            return false;
        return true;
    }
    error = SYN_ERROR;
    errorMsg = STATE_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <value>
 * @param htableItem* ukazatel promennou do ktere se prirazuje
 * @return bool informace o provedeni
 */
bool parserValue(htableItem* tempState) {
    // ### simulace pravidla 19 ###
    // <value> --> <expression>
    if (parserToken.code == T_LEFT_BAR || parserToken.code == T_ID ||
        parserToken.code == T_INT      || parserToken.code == T_DOUBLE ||
        parserToken.code == T_STRING) {
        if (!parserExpresionAnalize())        // <expression>
            return false;
        // pretypovani neni povolene
        if (tempState->type != tempItem->type) {
            error = SEM_ERROR;
            errorMsg = INCOMPAT_TYPE_MSG;
            return false;
        }
        if (!iInsertInstruction(&tripleAddressL, I_MOV, tempItem, NULL, tempState))
            return false;
        return true;
    }
    // ### simulace pravidla 20 ###
    // <value> --> find ( <strParam> , <strParam> )
    else if (parserToken.code == T_KW_FIND) {
        // vyledek lze priradit pouze do integer
        if (tempState->type != H_INT) {
            error = SEM_ERROR;
            errorMsg = INCOMPAT_TYPE_MSG;
            return false;
        }
        // find - byl jiz prijat
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_LEFT_BAR) { // (
            error = SYN_ERROR;
            errorMsg = LEFT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!parserStrParam())                // <strParam>
            return false;
        if (parserToken.code != T_COMA) {     // ,
            error = SYN_ERROR;
            errorMsg = COMA_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        htableItem* src1 = tempItem;
        if (!parserStrParam())                // <strParam>
            return false;
        if (parserToken.code != T_RIGHT_BAR){ // )
            error = SYN_ERROR;
            errorMsg = RIGHT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!iInsertInstruction(&tripleAddressL, I_FIND, src1, tempItem, tempState))
            return false;
        return true;
    }
    // ### simulace pravidla 21 ###
    // <value> --> sort ( <strParam> )
    else if (parserToken.code == T_KW_SORT) {
        // vyledek lze priradit pouze do string
        if (tempState->type != H_STR) {
            error = SEM_ERROR;
            errorMsg = STR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_LEFT_BAR) { // (
            error = SYN_ERROR;
            errorMsg = LEFT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!parserStrParam())                // <strParam>
            return false;
        if (parserToken.code != T_RIGHT_BAR) {// )
            error = SYN_ERROR;
            errorMsg = RIGHT_BAR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!iInsertInstruction(&tripleAddressL, I_SORT, tempItem, NULL, tempState))
            return false;
        return true;
    }
    error = SYN_ERROR;
    errorMsg = RIGHT_SIDE_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <idList>
 * @return bool informace o provedeni
 */
bool parserIdList() {
    // ### simulace pravidla 22 ###
    // <idList> --> id <idList2>
    if (parserToken.code == T_ID) {
        // nalezeni promenne v tabulce symbolu
        htableItem* temp = htableSearch(h_table, parserToken.key);
        if (temp == NULL) {
            error = SEM_ERROR;
            errorMsg = ID_UNDECLARED_MSG;
            return false;
        }
        // vygenerovani instrukce
        if (!iInsertInstruction(&tripleAddressL, I_READ, NULL, NULL, temp))
            return false;
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        // pri readln stringu je povolen jen jeden parametr
        if (temp->type == H_STR && parserToken.code != T_RIGHT_BAR) {
            error = SYN_ERROR;
            errorMsg = RIGHT_BAR_EXPECT_MSG;
            return false;
        }
        if (!parserIdList2())                 // <idList2>
            return false;
        // zahozeni zbyvajich znaku na vstupu
        if (temp->type != H_STR) {
            if (!iInsertInstruction(&tripleAddressL, I_CLEAR, NULL, NULL, NULL))
                return false;
        }
        return true;
    }
    // ### simulace pravidla 23 ###
    // <idList> --> [epsilon]
    else if (parserToken.code == T_RIGHT_BAR) {
        if (!iInsertInstruction(&tripleAddressL, I_CLEAR, NULL, NULL, NULL))
            return false;
        return true;
    }
    error = SYN_ERROR;
    errorMsg = ID_OR_RIGHT_BAR_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <idList2>
 * @return bool informace o provedeni
 */
bool parserIdList2() {
    // ### simulace pravidla 24 ###
    // <idList2> --> , id <idList2>
    if (parserToken.code == T_COMA) {
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (parserToken.code != T_ID) {       // id
            error = SYN_ERROR;
            errorMsg = ID_EXPECT_MSG;
            return false;
        }
        // nalezeni promenne v tabulce symbolu
        tempItem = htableSearch(h_table, parserToken.key);
        if (tempItem == NULL) {
            error = SEM_ERROR;
            errorMsg = ID_UNDECLARED_MSG;
            return false;
        }
        if (tempItem->type == H_STR) {        // pri vice param. neni string povolen
            error = SYN_ERROR;
            errorMsg = STR_UNEXPECT_MSG;
            return false;
        }
        // vygenerovani instrukce
        if (!iInsertInstruction(&tripleAddressL, I_READ, NULL, NULL, tempItem))
            return false;

        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!parserIdList2())                 // <idList2>
            return false;
        return true;
    }
    // ### simulace pravidla 25 ###
    // <idList2> --> [epsilon]
    else if (parserToken.code == T_RIGHT_BAR)
        return true;
    error = SYN_ERROR;
    errorMsg = RIGHT_BAR_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <exprList>
 * @return bool informace o provedeni
 */
bool parserExprList() {
    // ### simulace pravidla 25 ###
    // <exprList> --> <expression> <exprList2>
    if (parserToken.code == T_LEFT_BAR || parserToken.code == T_ID ||
        parserToken.code == T_INT      || parserToken.code == T_DOUBLE ||
        parserToken.code == T_STRING) {
        if (!parserExpresionAnalize())   // <expression>
            return false;
        // vygenerovani instrukce
        if (!iInsertInstruction(&tripleAddressL, I_WRITE, tempItem, NULL, NULL))
            return false;
        if (!parserExprList2())          // <exprList2>
            return false;
        return true;
    }
    // ### simulace pravidla 27 ###
    // <exprList> --> [epsilon]
    else if (parserToken.code == T_RIGHT_BAR)
        return true;
    error = SYN_ERROR;
    errorMsg = EXPR_OR_RIGHT_BAR_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonteminal <exprList2>
 * @return bool informace o provedeni
 */
bool parserExprList2() {
    // ### simulace pravidla 28 ###
    // <exprList2> --> , <expression> <exprList2>
    if (parserToken.code == T_COMA) {
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        if (!parserExpresionAnalize())        // <expression>
            return false;
        // vygenerovani instrukce
        if (!iInsertInstruction(&tripleAddressL, I_WRITE, tempItem, NULL, NULL))
            return false;
        if (!parserExprList2())               // <exprList2>
            return false;
        return true;
    }
    // ### simulace pravidla 29 ###
    // <exprList2> --> [epsilon]
    else if (parserToken.code == T_RIGHT_BAR)
        return true;
    error = SYN_ERROR;
    errorMsg = RIGHT_BAR_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <strParam>
 * @return bool informace o provedeni
 */
bool parserStrParam() {
    // ### simulace pravidla 30 ###
    // <strParam> --> id
    if (parserToken.code == T_ID) {
        // vyhledani promenne v tabulce symbolu
        tempItem = htableSearch(h_table, parserToken.key);
        if (tempItem == NULL) {
            error = SEM_ERROR;
            errorMsg = ID_UNDECLARED_MSG;
            return false;
        }
        if (tempItem->type != H_STR) {        // povolen pouze string
            error = SEM_ERROR;
            errorMsg = STR_EXPECT_MSG;
            return false;
        }
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        return true;
    }
    // ### simulace pravidla 31 ###
    // <strParam> --> id
    if (parserToken.code == T_STRING) {
        tempData.sVal = parserToken.key;
        tempItem = htableInsert(h_table, H_STR, tempData, NULL, CONST);
        if(!tempItem)   
            return false;
        if (!lexGetNextToken())               // nacteni noveho tokenu
            return false;
        return true;
    }
    error = SEM_ERROR;
    errorMsg = STR_EXPECT_MSG;
    return false;
}

/**
 * Funkce reprezentujici nonterminal <expression>
 * @return bool informace o provedeni
 */
bool parserExpresionAnalize() {
    sItem dataSource, dataStack;
    precedentKey action;
    sStack stack;
    int numLeftBar = 0;

    // inicializace promennych
    sInit(&stack);
    dataSource.type = 0;
    dataSource.item = NULL;
    dataStack.type = S_END;
    dataStack.item = NULL;

    // vlozeni koncoveho stavu na zasobnik, slouzi jako zarazka vyhodnoceni vyrazu
    sPush(&stack, dataStack);
    // pripaveni dat na zasobnik
    parserPrepareSourceData(&dataSource, &numLeftBar);
    // kontrola jestli vyraz neni prazdny
    if(dataSource.type == S_END) {
        error = SYN_ERROR;
        errorMsg = BAD_EXPRESSION_MSG;
    }
    // nacitani a zpracovani vyrazu
    while ((!sIsEmpty(&stack) || dataSource.type != S_END) && error == 0) {
        // nalezeni operace podle zasobniku
        parserFindOperation(&stack, &dataStack);
        // ziskani akce podle precedentni tabulky
        action = precedentTable[dataStack.type][dataSource.type];
        switch (action) {
            // akce pro ulozeni pripravenych dat na zasobnik a nacteni novych dat
        case Sleft:
            sPush(&stack, dataSource);
            if (dataSource.type != S_END) {
                if (!lexGetNextToken()) {
                    continue;
                }
            }
            parserPrepareSourceData(&dataSource, &numLeftBar);
            break;
            // akce pro nalezeni operace a jeji nasledne provedeni
        case SRigh:
            parserFindOperation(&stack, &dataStack);
            parserDoRule(dataStack, &stack);
            break;
        case SEqua:
            parserFindOperation(&stack, &dataStack);
            parserDoRule(dataStack, &stack);
            if (dataSource.type == S_RBAR) {
                if (!lexGetNextToken()) {
                    continue;
                }
                parserPrepareSourceData(&dataSource, &numLeftBar);
            }
            break;
            // pri zjisteni chyby podle precedentni tabulky zkonci analyza vyrazu
        default:
            error = SYN_ERROR;
            errorMsg = BAD_EXPRESSION_MSG;
            break;
        }
    }
    // smazani zasobniku
    sDelete(&stack);
    // kontrola na neuspesny pruchod vyrazem
    if (error)
        return false;
    return true;
}
