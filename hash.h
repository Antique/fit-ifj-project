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
 *   Soubor: hash.h                                                          *
 *                                                                           *
 *   Popis: Obsahuje definice výètù, struktur a enum potøebné pro práci      *
 *          s tabulkou symbolù, hash tabulkou a definice funkcí.             *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HASH_H
#define HASH_H

// vycet typu klicu
typedef enum {VAR, CONST, LABEL} keyType;

// vycet typu ulozenych dat
typedef enum {H_INT, H_DBL, H_STR, H_NON, H_LAB} dataType;

// union pro ruzne typy ulozenych dat
typedef union {
    int iVal;
    double dVal;
    string* sVal;
    struct iListItem* lVal;
} uData;

// struktura polozky hash tabulky
typedef struct sHtableItem {
    string *key;
    dataType type;
    uData value;
    struct sHtableItem *next;
} htableItem;

// struktura hash tabulky
typedef struct {
    unsigned int htableSize;
    unsigned int currConstKey;
    unsigned int currLabelKey;
    htableItem** uk;
} htableTable;

// struktura iteratoru, ktery je potreba jen pro urychleni nekterych internich funkcí
typedef struct {
    htableTable *htPtr;
    unsigned int index;
    htableItem *ptr;
} htableIterator;

/**
 * Provede inicializaci hash tabulky.
 * @return htableTable* ukazatel na inicializovanou hash tabulku
 */
htableTable* htableInit();

/**
 * Vyhledani klice v hash tabulce.
 * @param htableTable* ukazatel na hash tabulku
 * @param string* ukazatel na klic
 * @return htableItem* ukazatel na nalezenou polozku nebu NULL
 */
htableItem* htableSearch(htableTable *t, string* key);

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
htableItem* htableInsert(htableTable *t, dataType type, uData data, string* key, keyType kType);

/**
 * Smaze hash tabulku.
 * @param htableTable* ukazatel na hash tabulku
 */
void htableFree(htableTable *t);

#endif
