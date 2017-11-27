/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *   IFJ09 - Implementace interpretu imperativn�ho jazyka IFJ09              *
 *           spole�n� projekt p�edm�t� IFJ a IAL                             *
 *                                                                           *
 *   Auto�i: Bostern�k Bal�zs, xboste00@stud.fit.vutbr.cz                    *
 *           Hlav�� Jan, xhlava18@stud.fit.vutbr.cz                          *
 *           Hrdina Pavel, xhrdin05@stud.fit.vutbr.cz                        *
 *           Chmiel Filip, xchmie02@stud.fit.vutbr.cz                        *
 *                                                                           *
 *   Datum dokon�en�: 13.12.2009                                             *
 *                                                                           *
 *   Soubor: stack.h                                                         *
 *                                                                           *
 *   Popis: Obsahuje definice pomocn�ch funkc� a datov�ch struktur pro pr�ci *
 *          se z�sobn�kem, kter� je vyu�it p�i syntaktick� anal�ze v�raz�.   *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef STACK_H
#define	STACK_H

// vycet moznych terminalu pro zpracovani vyrazu
typedef enum {
    S_PLUS = 0,
    S_MINUS,
    S_MULTIPLY,
    S_DIVIDE,
    S_LES,
    S_LESSSAME,
    S_MORE,
    S_MORESAME,
    S_OR,
    S_SAME,
    S_LBAR,
    S_RBAR,
    S_IDENTIFIER,
    S_END,
    S_NONTERMINAL
} sType;

// struktura polozky zasobniku
typedef struct sSItem {
    sType type;
    htableItem* item;
    struct sSItem* next;
} sItem;

// struktura zasobniku
typedef struct {
    sItem* top;
    unsigned int count;
} sStack;

/**
 * Inicializace zasobniku.
 * @param sStack* ukazatel na zasobnik
 */
void sInit(sStack* s);

/**
 * Vlozeni prvku na vrchol zasobniku.
 * @param sStack* ukazatel na zasobnik
 * @param sItem prvek ktery se ma vlozit
 * @return bool informace o spravnem ulozeni
 */
bool sPush(sStack* s, sItem d);

/**
 * Smazani prvku z vrcholu zasobniku.
 * @param sStack* ukazatel na zasobnik
 */
void sPop(sStack* s);

/**
 * Ziskani hodnoty prvku na vrcholu zasobniku.
 * @param sStack* ukazatel na zasobnik
 * @param sItem* ukazatel kam se maji data ulozit
 * @return bool informace o spravnem ziskani dat
 */
bool sTop(sStack* s, sItem* d);

/**
 * Ziskani hodnoty prvku na vrcholu zasobniku a nasledne smazani.
 * @param sStack* ukazatel na zasobnik
 * @param sItem* ukazatel kam se maji data ulozit
 * @return bool informace o spravnem ziskani dat a jeho smazani
 */
bool sTopPop(sStack* s, sItem* d);

/**
 * Smazani zasobniku
 * @param sStack* ukazatel na zasobnik
 */
void sDelete(sStack* s);

/**
 * Zjisteni jestli je zasobnik prazdny
 * @param sStack* ukazatel na zasobnik
 * @return bool informace o prazdnosti zasobniku
 */
bool sIsEmpty(sStack* s);

#endif
