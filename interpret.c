/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *   IFJ09 - Implementace interpretu imperativního jazyka IFJ09 spoleèný     *
 *           projekt pøedmìtù IFJ a IAL                                      *
 *                                                                           *
 *   Autoøi: Bosternák Balázs, xboste00@stud.fit.vutbr.cz                    *
 *           Hlaváè Jan, xhlava18@stud.fit.vutbr.cz                          *
 *           Hrdina Pavel, xhrdin05@stud.fit.vutbr.cz                        *
 *           Chmiel Filip, xchmie02@stud.fit.vutbr.cz                        *
 *                                                                           *
 *   Datum dokonèení: 13.12.2009                                             *
 *                                                                           *
 *   Soubor: interpret.c                                                     *
 *                                                                           *
 *   Popis: Obsahuje deklarace funkcí potøebných pro interprataci instrukcí  *
 *          a pro práci se seznamem instrukcí.                               *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "string.h"
#include "hash.h"
#include "interpret.h"
#include "defines.h"

extern progErrors error;
extern errorMessages errorMsg;

/**
 * Provede inicializaci seznamu instrukci.
 * @param iList ukazatel na seznam instrukci
 */
void iListInit(iList *L) {
    L->first = NULL;
    L->act = NULL;
    L->last = NULL;
}

/**
 * Provede smazani seznamu instrukci.
 * @param iList ukazatel na seznam instrukci
 */
void iListDispose(iList *L) {
    // dokud nenarazim na konec seznamu
    while (L->first != NULL) {
        L->act = L->first;
        L->first = L->first->next;
        free(L->act);
    }
    // zrusim odkazy na aktualni a posledni prvek
    L->act = NULL;
    L->last = NULL;
}

/**
 * Vlozi novou instrukci na konec seznamu.
 * @param iList ukazatel na seznam instrukci
 * @param iInstruct instrukce
 * @param htableItem* prvni zdrojova adresa
 * @param htableItem* druha zdrojova adresa
 * @param htableItem* cilova adresa
 * @return bool vrati informaci o spravnem ulozeni instrukce
 */
bool iInsertInstruction(iList *L, iInstruct ins, htableItem* src1, htableItem* src2, htableItem* dest) {
    iListItemPtr insert;
    insert = malloc(sizeof (struct iListItem));
    if (insert == NULL) {
        error = OTHER_ERROR;
        errorMsg = MEM_MSG;
        return false;
    } else {
        insert->instruction = ins;
        insert->src1 = src1;
        insert->src2 = src2;
        insert->dest = dest;
        insert->next = NULL;
        if (L->last != NULL) {
            L->last->next = insert;
        }
        if (L->first == NULL) {
            L->first = insert;
        }
        L->last = insert;
        return true;
    }
}

/**
 * Vlozi novou instrukci na konec seznamu.
 * @param iList ukazatel na seznam instrukci
 * @return bool vrati informaci o spravnem provedeni iterpretu
 */
bool interprate(iList *L) {
    string* sorted;
    L->act = L->first;
    // cyklus probiha dokud neni ukonceny nejakou vnitrni chybou nebo instrukci end
    while (true) {
        switch (L->act->instruction) {
            // secteni dvou cisel nebo spojeni dvou retezcu
        case I_ADD:
            switch (L->act->dest->type) {
            case H_INT:
                L->act->dest->value.iVal = L->act->src1->value.iVal + L->act->src2->value.iVal;
                break;
            case H_DBL:
                L->act->dest->value.dVal =
                    (L->act->src1->type == H_INT ? L->act->src1->value.iVal : L->act->src1->value.dVal)
                    + (L->act->src2->type == H_INT ? L->act->src2->value.iVal : L->act->src2->value.dVal);
                break;
            case H_STR:
                if (!strCopy(L->act->dest->value.sVal, L->act->src1->value.sVal))
                    break;
                strAddStr(L->act->dest->value.sVal, L->act->src2->value.sVal);
                break;
            default:
                break;
            }
            break;
            // odecteni dvou cisel
        case I_SUB:
            switch (L->act->dest->type) {
            case H_INT:
                L->act->dest->value.iVal = L->act->src1->value.iVal - L->act->src2->value.iVal;
                break;
            case H_DBL:
                L->act->dest->value.dVal =
                    (L->act->src1->type == H_INT ? L->act->src1->value.iVal : L->act->src1->value.dVal)
                    - (L->act->src2->type == H_INT ? L->act->src2->value.iVal : L->act->src2->value.dVal);
                break;
            default:
                break;
            }
            break;
            // vynasobeni dvou cisel
        case I_MUL:
            switch (L->act->dest->type) {
            case H_INT:
                L->act->dest->value.iVal = L->act->src1->value.iVal * L->act->src2->value.iVal;
                break;
            case H_DBL:
                L->act->dest->value.dVal =
                    (L->act->src1->type == H_INT ? L->act->src1->value.iVal : L->act->src1->value.dVal)
                    * (L->act->src2->type == H_INT ? L->act->src2->value.iVal : L->act->src2->value.dVal);
                break;
            default:
                break;
            }
            break;
            // vydeleni dvou cisel
        case I_DIV:
            if (L->act->src2->value.iVal == 0) {
                error = INT_ERROR;
                errorMsg = DIV_BY_ZERO_MSG;
                break;
            }
            L->act->dest->value.iVal = L->act->src1->value.iVal / L->act->src2->value.iVal;
            break;
            // porovnani cisel nebo retezcu
        case I_LESS:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal < L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal < L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) < 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
        case I_LESSOREQUAL:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal <= L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal <= L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) <= 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
        case I_GREATER:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal > L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal > L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) > 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
        case I_GREATEROREQUAL:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal >= L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal >= L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) >= 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
        case I_DIFFERENT:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal != L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal != L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) != 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
        case I_SAME:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = (L->act->src1->value.iVal == L->act->src2->value.iVal ? 1 : 0);
                break;
            case H_DBL:
                L->act->dest->value.iVal = (L->act->src1->value.dVal == L->act->src2->value.dVal ? 1 : 0);
                break;
            case H_STR:
                L->act->dest->value.iVal = (strcmp(L->act->src1->value.sVal->str, L->act->src2->value.sVal->str) == 0 ? 1 : 0);
                break;
            default:
                break;
            }
            break;
            // zkopirovani ze zdroje do cile
        case I_MOV:
            switch (L->act->src1->type) {
            case H_INT:
                L->act->dest->value.iVal = L->act->src1->value.iVal;
                break;
            case H_DBL:
                L->act->dest->value.dVal = L->act->src1->value.dVal;
                break;
            case H_STR:
                strCopy(L->act->dest->value.sVal, L->act->src1->value.sVal);
                break;
            default:
                break;
            }
            break;
            // negace zdroje ulozena do cile
        case I_NOT:
            L->act->dest->value.iVal = !L->act->src1->value.iVal;
            break;
            // nacteni ze stdin
        case I_READ:;
                    char c;
                    switch(L->act->dest->type) {
                    case H_INT:
                        if(!scanf("%d", &L->act->dest->value.iVal)) {
                            error = INT_ERROR;
                            errorMsg = INT_EXPECT_MSG;
                        }
                        break;
                    case H_DBL:
                        if(!scanf("%lf", &L->act->dest->value.dVal)) {
                            error = INT_ERROR;
                            errorMsg = DBL_EXPECT_MSG;
                        }
                        break;
                    case H_STR:
                        strClean(L->act->dest->value.sVal);
                        while((c = getchar()) != '\n' && !error) {
                            if(c == EOF) {
                                error = INT_ERROR;
                                errorMsg = EOF_UNEXPECT_MSG;
                                break;
                            }
                            strAddChar(L->act->dest->value.sVal, c);
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                    // zahozeni zbytku radku ze stdin
        case I_CLEAR:
                    while((c = getchar()) != '\n' && !error) {
                        if(c == EOF) {
                            error = INT_ERROR;
                            errorMsg = EOF_UNEXPECT_MSG;
                            break;
                        }
                    }
                    break;
                    // vypsani dat na stdout
        case I_WRITE:
                    switch(L->act->src1->type) {
                    case H_INT:
                        printf("%d", L->act->src1->value.iVal);
                        break;
                    case H_DBL:
                        printf("%g", L->act->src1->value.dVal);
                        break;
                    case H_STR:
                        printf("%s", L->act->src1->value.sVal->str);
                        break;
                    default:
                        break;
                    }
                    break;
                    // podmineny skok pokud je zadana adresa podminky, jinak nepodmineny
        case I_GOTO:
                    if(L->act->src1) {
                        if(L->act->src1->type == H_INT && !L->act->src1->value.iVal)
                            break;
                    }
                    L->act = L->act->dest->value.lVal;
                    break;
                    // provete setrideni zdrojoveho stringu a vysledek ulozi do ciloveho stringu
        case I_SORT:
                    sorted = strSort(L->act->src1->value.sVal);
                    strCopy(L->act->dest->value.sVal, sorted);
                    strFree(sorted);
                    break;
                    // nalezeni druheho zdrojoveho stringu v prvni zdrojovem stringu,
                    // pozici ulozi do cilove hodnoty
        case I_FIND:
                    L->act->dest->value.iVal = strFind(L->act->src1->value.sVal, L->act->src2->value.sVal);
                    break;
                    // zalozka, provede se jeji preskoceni
        case I_LAB:
                    break;
                    // ukoncovaci instrukce
        case I_ENDDOT:
                    return true;
        }
        // pri vzniku chyby se intepretace ukonci
        if(error)
            return false;
        L->act = L->act->next;
    }
}
