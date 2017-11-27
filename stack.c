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
 *   Soubor: stack.c                                                         *
 *                                                                           *
 *   Popis: Obsahuje pomocné funkce pro práci se zásobníkem, který je vyu¾it *
 *          pøi syntaktické analýze výrazù.                                  *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "hash.h"
#include "stack.h"
#include "defines.h"

extern progErrors error;
extern errorMessages errorMsg;

/**
 * Inicializace zasobniku.
 * @param sStack* ukazatel na zasobnik
 */
void sInit(sStack* s) {
    s->top = NULL;
    s->count = 0;
}

/**
 * Vlozeni prvku na vrchol zasobniku.
 * @param sStack* ukazatel na zasobnik
 * @param sItem prvek ktery se ma vlozit
 * @return bool informace o spravnem ulozeni
 */
bool sPush(sStack* s, sItem d) {
    if (!s)
        return false;
    sItem* i = malloc(sizeof (sItem));
    if (!i) {
        error = OTHER_ERROR;
        errorMsg = MEM_MSG;
        return false;
    }

    i->item = d.item;

    i->type = d.type;
    i->next = s->top;
    s->top = i;
    s->count++;
    return true;
}

/**
 * Smazani prvku z vrcholu zasobniku.
 * @param sStack* ukazatel na zasobnik
 */
void sPop(sStack* s) {
    if (!s)
        return;
    if (sIsEmpty(s))
        return;
    sItem* temp = s->top;
    s->top = temp->next;
    s->count--;
    free(temp);
}

/**
 * Ziskani hodnoty prvku na vrcholu zasobniku.
 * @param sStack* ukazatel na zasobnik
 * @param sItem* ukazatel kam se maji data ulozit
 * @return bool informace o spravnem ziskani dat
 */
bool sTop(sStack* s, sItem* d) {
    if (!s)
        return false;
    if (sIsEmpty(s))
        return false;
    d->type = s->top->type;
    d->item = s->top->item;
    return true;
}

/**
 * Ziskani hodnoty prvku na vrcholu zasobniku a nasledne smazani.
 * @param sStack* ukazatel na zasobnik
 * @param sItem* ukazatel kam se maji data ulozit
 * @return bool informace o spravnem ziskani dat a jeho smazani
 */
bool sTopPop(sStack* s, sItem* d) {
    if (!s)
        return false;
    if (sIsEmpty(s))
        return false;
    d->type = s->top->type;
    d->item = s->top->item;

    sItem* temp = s->top;
    s->top = temp->next;
    s->count--;
    free(temp);
    return true;
}

/**
 * Smazani zasobniku
 * @param sStack* ukazatel na zasobnik
 */
void sDelete(sStack* s) {
    if (!s)
        return;
    while (s->top != NULL) {
        sItem* temp = s->top;
        s->top = temp->next;
        free(temp);
    }
}

/**
 * Zjisteni jestli je zasobnik prazdny
 * @param sStack* ukazatel na zasobnik
 * @return bool informace o prazdnosti zasobniku
 */
bool sIsEmpty(sStack* s) {
    if (!s)
        return true;
    if (!s->count)
        return true;
    return false;
}
