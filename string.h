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
 *   Soubor: string.h                                                        *
 *                                                                           *
 *   Popis: Obsahuje definice pomocn�ch funkc� pro pr�ci s nekone�n�         *
 *          dlouh�mi �et�zci. D�le obsahuje definici datov� struktury        *
 *          reprezentuj�c� �et�zec                                           *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef STRING_H
#define STRING_H

// velikost bloku po kterych se alokuje/realokuje pole pro ulozeni retezce
#define BLOCK_SIZE 8

// struktura stringu
typedef struct {
    char* str;
    unsigned int length;
    unsigned int allocSize;
} string;

/**
 * Funkce pro vytvoreni prazdneho retezce.
 * @return string* pri nedostatku pameti vraci NULL, jinak ukazatel na string
 */
string* strInit();

/**
 * Funkce pro vycisteni stringu, nemaze ho.
 * @param string* ukazatel na string, ktery se ma vycistit
 * @return bool informace o provedeni
 */
bool strClean(string* src);

/**
 * Funkce pro smazani stringu.
 * @param string* ukazatel na string, ktery se ma mazat
 */
void strFree(string* src);

/**
 * Funkce pro pridani znaku na konec retezce, v pripade neuspesne
 * realokace je puvodni retezec zachovan.
 * @param string* ukazatel na cilovy string
 * @param char znak, ktery se ma vlozit
 * @return bool informace o provedeni
 */
bool strAddChar(string* dest, char c);

/**
 * Funkce pro pridani retezce na konec retezce, v pripade neuspesne
 * realokace je puvodni retezec zachovan.
 * @param string* ukazatel na cilovy string
 * @param string* ukazatel na string, ktery se ma pridat
 * @return bool informace o uspesnem provedeni
 */
bool strAddStr(string* dest, string* src);

/**
 * Zpopiruje zdrojovy retezec do ciloveho, tj. prepise ho.
 * @param string* ukazatel na cilovy string
 * @param string* ukazatel na zdrojovy string
 * @return bool informace o uspesnem provedeni
 */
bool strCopy(string* dest, string* src);

/**
 * Hleda podretezec v retezci a vraci jeho pozici
 * Knuth-Moris-Prattuv algoritmus.
 * @param string* ukazatel na string, ve kterem se ma hledat
 * @param string* ukazatel na string, ktery se ma hledat
 * @return int pozice, na niz se podretezec nachazi. Pokud nebyl nalezen vraci 0
 */
int strFind(string* src, string* subString);

/**
 * Funkce pro setrideni stringu, vyuziva algoritmu merge-sort.
 * @param string* ukazatel na string, ktery se ma setridit
 * @return string* ukazatel na setrideny string, v pripade chyby NULL
 */
string* strSort(string* src);

#endif
