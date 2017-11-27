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
 *   Soubor: hash.c                                                          *
 *                                                                           *
 *   Popis: Obsahuje deklarace funkcí potøebných pro hashovací tabulku,      *
 *          která reprezentuje tabulku symbolù.                              *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "string.h"
#include "hash.h"
#include "interpret.h"
#include "defines.h"

extern progErrors error;
extern errorMessages errorMsg;

/**
 * Vytvori hash ze zadaneho klice.
 * @param char* Ukazatel na klic
 * @param unsigned int velikost tabulky
 * @return unsigned int hash klice
 */
unsigned int hashFunction(char* key, unsigned int size)
{
    unsigned int h=0;
    unsigned char *p;
    for(p=(unsigned char*)key; *p!='\0'; p++)
        h = 33*h + *p;
    return h % size;
}

/**
 * Provede inicializaci hash tabulky.
 * @return htableTable* ukazatel na inicializovanou hash tabulku
 */
htableTable *htableInit() {
    htableTable *t = malloc(sizeof(htableTable));
    if (t) {
        t->htableSize = 100000;
        t->currConstKey = 1;
        t->currLabelKey = 1;
        t->uk = malloc(t->htableSize*sizeof(htableItem *));
        if(t->uk) {
            for(int i = 0; i < t->htableSize; i++)
                t->uk[i] = NULL;		//vynulovani ukazatelu na seznamy
        }
        else {
            error = OTHER_ERROR;
            errorMsg = MEM_MSG;
            free(t);
            t = NULL;
        }
    }
    return t;
}

/**
 * Vycisti hash tabulku.
 * @param htableTable* ukazatel na hash tabulku
 * @return htableTable* ukazatel na inicializovanou hash tabulku
 */
void htableClear(htableTable *t) {
    // kontrola na existenci tabulky
    if (t == NULL)
        return;

    htableItem *item = NULL;
    unsigned int i = 0;
    while(i < t->htableSize) {  // projiti vsech radku tabulek
        while(t->uk[i] != NULL) {   // projiti vsech polozek seznamu
            item = t->uk[i];
            t->uk[i] = item->next;
            strFree(item->key);
            if(item->type == H_STR)   // jen kdyz typ dat je string
                strFree(item->value.sVal);
            free(item);
        }
        i++;
    }
}

/**
 * Smaze hash tabulku.
 * @param htableTable* ukazatel na hash tabulku
 */
void htableFree(htableTable *t)
{
    htableClear(t);
    free(t->uk);
    free(t);
    t = NULL;
}

/**
 * Vyhledani klice v hash tabulce.
 * @param htableTable* ukazatel na hash tabulku
 * @param string* ukazatel na klic
 * @return htableItem* ukazatel na nalezenou polozku nebu NULL
 */
htableItem* htableSearch(htableTable *t, string* key) {
    // kontrola na existenci tabulky a klice
    if (t == NULL || key == NULL)
        return NULL;

    unsigned int hash = hashFunction(key->str, t->htableSize);
    htableItem *item = t->uk[hash];

    while(item != NULL) {   // projiti vsech polozek radku urceneho pomoci hashe
        if (!strcmp(key->str, item->key->str)) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

/**
 * Vlozi do tabulky novou polozku.
 * @param htableTable* ukazatel na tabulku
 * @param dataType typ vkladanych dat
 * @param uData data vkladanych dat, musi odpovidat typu vkladanych dat
 * @param string* klic ktery se pouziva pouze pro ulozeni nazvu promenne, pri
 * vkladani constatny nebo stitku je tato hodnota NULL
 * @param keyType urcuje jaky typ polozky se vklada
 * @return htableItem* ukazatel na vytvorenou polozku, pri chybe vraci NULL,
 * pokud vkladana polozka existuje taky vraci NULL
 */
htableItem* htableInsert(htableTable *t, dataType type, uData data, string* key, keyType kType) {
    // kontrola na existenci tabulky
    if (!t)
        return NULL;

    htableItem *new = malloc(sizeof(htableItem));
    if (!new) {
        error = OTHER_ERROR;
        errorMsg = MEM_MSG;
        return NULL;
    }

    /* zacatek vytvoreni klice */
    new->key = strInit();
    if (!new->key) {
        free(new);
        return NULL;
    }
    // pokud je typ polozky promenna, tak se klic zkopiruje
    if (kType == VAR) {
        if (!strCopy(new->key, key)) {
            strFree(new->key);
            free(new);
            return NULL;
        }
    }   // jinak se vytvory novy podle zadaneho typu polozky
    else {
        int i;
        if (kType == LABEL) {
            strAddChar(new->key, '#');
            i = t->currLabelKey;
        }
        else {
            strAddChar(new->key, '$');
            i = t->currConstKey;
        }
        while (i != 0)
        {
            strAddChar(new->key, (char)(i % 10 + '0'));
            i = i / 10;
        }
    }
    /* konec vytvoreni klice */

    // ulozeni dat podle typu ukladanych dat
    new->type = type;
    switch (new->type) {
    case H_INT:
        new->value.iVal = data.iVal;
        break;
    case H_DBL:
        new->value.dVal = data.dVal;
        break;
    case H_STR:
        new->value.sVal = strInit();
        if(new->value.sVal == NULL) {
            strFree(new->key);
            free(new);
            return NULL;
        }
        if(!strCopy(new->value.sVal, data.sVal) && error)
            return NULL;
        break;
    case H_LAB:
        new->value.lVal = data.lVal;
        break;
    default:
        strFree(new->key);
        free(new);
        return NULL;
    }
    new->next = NULL;

    // vytvoreni hashe klice
    unsigned int hash = hashFunction(new->key->str, t->htableSize);
    htableItem *item = t->uk[hash];
    // kontrola na existenci vkladaneho klice, pri nenalezeni ulozi na konec seznamu
    if (item != NULL) {
        do {
            if (!strcmp(new->key->str, item->key->str)) {   // pri nalezeni se vrati NULL
                strFree(new->key);
                if(new->type == H_STR)
                    strFree(new->value.sVal);
                free(new);
                return NULL;
            }
            if (item->next)
                item = item->next;
        } while(item->next != NULL);
        item->next = new;
    }
    else
        t->uk[hash] = new;

    // pokud se pridavala constanta nebo stitek, tak jejich aktualni index zvysim
    if (kType != VAR) {
        if (kType == LABEL) {
            t->currLabelKey++;
        }
        else {
            t->currConstKey++;
        }
    }

    return new;
}
