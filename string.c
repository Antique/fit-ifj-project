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
 *   Soubor: string.c                                                        *
 *                                                                           *
 *   Popis: Obsahuje pomocné funkce pro práci s nekoneènì dlouhými øetìzci.  *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string.h"
#include "defines.h"

extern progErrors error;
extern errorMessages errorMsg;

/**
 * Funkce pro vytvoreni prazdneho retezce.
 * @return string* pri nedostatku pameti vraci NULL, jinak ukazatel na string
 */
string* strInit() {
    error = OTHER_ERROR;
    errorMsg = MEM_MSG;
    string *src = malloc(sizeof(string));
    // chyba pri alokaci pameti
    if (!src)
        return NULL;
    src->str = malloc(BLOCK_SIZE);
    if (!src->str) {
        free(src);
        return NULL;
    }
    // retezec je prazdny
    src->str[0] = '\0';
    src->length = 0;
    src->allocSize = BLOCK_SIZE;
    error = NONE_ERROR;
    return src;
}

/**
 * Funkce pro vycisteni stringu, nemaze ho.
 * @param string* ukazatel na string, ktery se ma vycistit
 * @return bool informace o provedeni
 */
bool strClean(string* src) {
    if (src) {
        free(src->str);
        src->str = malloc(BLOCK_SIZE);
        if (!src->str) {
            error = OTHER_ERROR;
            errorMsg = MEM_MSG;
            return false;
        }
        src->str[0] = '\0';
        src->length = 0;
        src->allocSize = BLOCK_SIZE;
        return true;
    }
    return false;
}

/**
 * Funkce pro smazani stringu.
 * @param string* ukazatel na string, ktery se ma mazat
 */
void strFree(string* src) {
    if (src) {
        free(src->str);
        free(src);
        src = NULL;
    }
}

/**
 * Funkce pro pridani znaku na konec retezce, v pripade neuspesne
 * realokace je puvodni retezec zachovan.
 * @param string* ukazatel na cilovy string
 * @param char znak, ktery se ma vlozit
 * @return bool informace o provedeni
 */
bool strAddChar(string* dest, char c) {
    // je kam vkladat
    if (dest) {
        // nedostatek mista v poli - realokace na vetsi
        if (dest->length + 1 >= dest->allocSize) {
            // chyba pri realokaci pameti
            dest->str = realloc(dest->str, dest->allocSize + BLOCK_SIZE);
            if (!dest->str) {
                error = OTHER_ERROR;
                errorMsg = MEM_MSG;
                return false;
            }
            else
                dest->allocSize += BLOCK_SIZE;
        }
        dest->str[dest->length] = c;
        dest->length++;
        dest->str[dest->length] = '\0';
        return true;
    }
    return false;
}

/**
 * Funkce pro pridani retezce na konec retezce, v pripade neuspesne
 * realokace je puvodni retezec zachovan.
 * @param string* ukazatel na cilovy string
 * @param string* ukazatel na string, ktery se ma pridat
 * @return bool informace o uspesnem provedeni
 */
bool strAddStr(string* dest, string* src) {
    // je kam vkladat
    if (dest && src) {
        dest->str = realloc(dest->str, ((dest->length + src->length)/BLOCK_SIZE + 1)*BLOCK_SIZE);
        if (dest->str) {
            dest->allocSize = ((dest->length + src->length) / BLOCK_SIZE + 1)*BLOCK_SIZE;
            dest->length += src->length;
            strcat(dest->str, src->str);
        }
        else {
            error = OTHER_ERROR;
            errorMsg = MEM_MSG;
        }
        return true;
    }
    return false;
}

/**
 * Zpopiruje zdrojovy retezec do ciloveho, tj. prepise ho.
 * @param string* ukazatel na cilovy string
 * @param string* ukazatel na zdrojovy string
 * @return bool informace o uspesnem provedeni
 */
bool strCopy(string *dest, string *src) {
    // jeden z retezcu neexistuje
    if (dest && src) {
        dest->str = realloc(dest->str, src->allocSize);
        if (dest->str) {
            // prekopirovani dat do ciloveho
            dest->allocSize = src->allocSize;
            dest->length = src->length;
            strcpy(dest->str, src->str);
            return true;
        }
        else {
            error = OTHER_ERROR;
            errorMsg = MEM_MSG;
        }
    }
    return false;
}

/**
 * Funkce pro vypocet "vektoru" posunu indexu
 * @param string*
 * @param int*
 */
void calculateShift(string *src, int *p){
    int i = 0;
    int j = -1;
    int n = src->length; // delka vzorku

    p[0] = -1;

    while (i < n) {
        while ((j > -1) &&  (src->str[i] != src->str[j]))
            j = p[j];
        i++;
        j++;

        if (src->str[i] == src->str[j])
            p[i] = p[j];
        else
            p[i] = j;
    }
}


/**
 * Hleda podretezec v retezci a vraci jeho pozici
 * Knuth-Moris-Prattuv algoritmus.
 * @param string* ukazatel na string, ve kterem se ma hledat
 * @param string* ukazatel na string, ktery se ma hledat
 * @return int pozice, na niz se podretezec nachazi. Pokud nebyl nalezen vraci 0
 */
int strFind(string* src, string* subString) {
    int i, j;
    int *p; // pomocne pole posunu indexu

    i = j = 0;
    // pokud je nektery parametr prazdny -> rovnou vracime 0
    if (src->length == 0 || subString->length == 0)
        return 0;
    // alokace pomocneho pole
    p = malloc((src->length+subString->length)*sizeof(int));
    if (!p) {
        error = OTHER_ERROR;
        errorMsg = MEM_MSG;
        return 0;
    }
    // vypocet posunu indexu
    calculateShift(subString, p);

    while (j < src->length) {
        while ((i > -1) && (subString->str[i] != src->str[j]))
            i = p[i];
        i++;
        j++;
        // vzorek byl nalezen
        if (i >= subString->length) {
            free(p);
            return j-i + 1; // vratime pozici vzorku
        }
    }
    free(p);
    return 0;  // v pripade ze vzorek nalezen nebyl vracime 0
}

/**
 * Funkce pro setrideni stringu, vyuziva algoritmu merge-sort.
 * @param string* ukazatel na string, ktery se ma setridit
 * @return string* ukazatel na setrideny string, v pripade chyby NULL
 */
string* strSort(string* src) {
    string* sorted = strInit();
    string* temp = strInit();
    // urcuje smer presunu ze zdrojoveho do ciloveho
    bool ahead = true;
    // urcuje zda narazil na konec posloupnosti
    bool endL, endR, endM;
    // pocitadlo nalezenych posloupnosti
    unsigned int num;
    // indexy zacatku a konci popoli v pomocnem poli
    unsigned int firstStart, firstEnd, secondStart, secondEnd;
    // urcuje krok pro posun v pomocnem podpoli
    int step;

    if(!sorted || !temp)
        return NULL;
    // vytvoreni pomocneho dvojnasobneho pole
    if(!strCopy(temp, src) || !strAddStr(temp, src) || !strCopy(sorted, src))
        return NULL;
    // zacatek serazovani, konci az kdyz je pocet nalezenych posloupnosti = 1
    do {
        // prvni cast je zdrojova, druha cast je cilova
        if(ahead) {
            firstStart = 0;
            firstEnd = src->length-1;
            secondStart = src->length;
            secondEnd = temp->length-1;
        }
        // prvni cast je cilova, druha cast je zdrojova
        else {
            firstStart = src->length;
            firstEnd = temp->length-1;
            secondStart = 0;
            secondEnd = src->length-1;
        }
        num = 0;
        step = 1;
        // zacatek cyklu, ktery jednou projde zdrojove pole
        do {
            endL = endR = endM = false;
            // zacatek cyklu, ktery projde z obou stran zdrojoveho pole jednu posloupnost
            // pri ukladani do ciloveho pole se posloupnosti seradi
            do {
                // mensi je na zacatku zdrojoveho pole
                if (temp->str[firstStart] < temp->str[firstEnd]) {
                    temp->str[secondStart] = temp->str[firstStart];
                    // pokud jsem se dostal na stred, tak koncim
                    if (firstStart == firstEnd)
                        endM = true;
                    else {
                        // posunu zacatek zdrojoveho pole o jedno bliz ke stredu
                        firstStart++;
                        // posunu aktualni zacatek ciloveho pole bliz ke stredu
                        secondStart += step;
                        // konec posloupnosti od zacatku
                        if (temp->str[firstStart] < temp->str[firstStart-1])
                            endL = true;
                    }
                }
                // mensi je na konci zdrojoveho pole
                else {
                    temp->str[secondStart] = temp->str[firstEnd];
                    // pokud jsem se dostal na stred, tak koncim
                    if (firstStart == firstEnd)
                        endM = true;
                    else {
                        // posunu konec zdrojoveho pole o jedno bliz ke stredu
                        firstEnd--;
                        // posunu aktualni zacatek ciloveho pole bliz ke stredu
                        secondStart += step;
                        // konec posloupnosti od konce
                        if (temp->str[firstEnd] < temp->str[firstEnd+1])
                            endR = true;
                    }
                }
            } while (!(endL || endR || endM));
            // pokud jsem narazil na konec posloupnosti od zacatku, tak musim
            // zbytek posloupnosti od konce zkopirovat taky
            while (endL) {
                temp->str[secondStart] = temp->str[firstEnd];
                firstEnd--;
                secondStart += step;
                // kontrola jestli jsem uz na konci posloupnosti
                endL = temp->str[firstEnd] >= temp->str[firstEnd+1];
            }
            // pokud jsem narazil na konec posloupnosti od konce, tak musim
            // zbytek posloupnosti od zacatku zkopirovat taky
            while (endR) {
                temp->str[secondStart] = temp->str[firstStart];
                firstStart++;
                secondStart += step;
                // kontrola jestli jsem uz na konci posloupnosti
                endR = temp->str[firstStart] >= temp->str[firstStart-1];
            }

            // stredovy prvek je treba taky zkopirovat
            if (firstStart == firstEnd) {
                temp->str[secondStart] = temp->str[firstStart];
                endM = true;
            }
            // prohozeni indexu zacatku a konce ciloveho pole
            // jednotlive serazene posloupnosti se ukladaji stridave na zacatek
            // a konec ciloveho pole
            secondStart += secondEnd;
            secondEnd = secondStart - secondEnd;
            secondStart = secondStart - secondEnd;
            // zvyseni poctu posloupnosti
            num++;
            step = -step;
        } while (firstStart != firstEnd);
        ahead = !ahead;
    } while (num > 1);
    // podle posledniho smeru kopirovani si vyberu serazene pole a to prekopiruju
    // do vysledneho strigu
    if(!ahead) {
        for(firstStart = 0; firstStart < src->length; firstStart++)
            sorted->str[firstStart] = temp->str[firstStart+src->length];
    }
    else {
        for(firstStart = 0; firstStart < src->length; firstStart++)
            sorted->str[firstStart] = temp->str[firstStart];
    }

    strFree(temp);
    return sorted;
}
