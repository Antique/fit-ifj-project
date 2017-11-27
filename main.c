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
 *   Soubor: main.c                                                          *
 *                                                                           *
 *   Popis: Hlavní tìlo programu, obsahuje definici chybových zpráv.         *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "defines.h"
#include "string.h"
#include "hash.h"
#include "interpret.h"
#include "lex.h"
#include "stack.h"
#include "parser.h"

// globalni promenne
FILE *input;
htableTable *h_table;
sOut parserToken;
iList tripleAddressL;
progErrors error;
errorMessages errorMsg;

const char* messages[] = {
    "",
    "Chybny pocet parametru.",
    "Soubor se nepodarilo otevrit.",
    "Nedostatek pameti.",
    "Nelze delit nulou",
    "Byl ocekavany integer.",
    "Byl ocekavany double.",
    "Nebyl ocekavany konec souboru.",
    "Nepodporovany znak.",
    "Chybne zadana ciselna konstanta.",
    "Identifikator nebyl ocekavan.",
    "Prilis velka ciselna konstanta.",
    "Byl ocekavany string.",
    "Chybne zadana escape sekvence.",
    "Chybne zadany retezec.",
    "Identifikator nebyl deklarovany.",
    "Nebyl ocekavany token.",
    "Nekompatibilni typy promennych ci konstant.",
    "Chybny vyraz.",
    "Byl ocekavany 'begin'.",
    "Byl ocekavany 'end'.",
    "Byla ocekavana tecka.",
    "Byl ocekavany konec souboru.",
    "Byl ocekavany 'var' nebo 'begin'.",
    "Byl ocekavany identifikator.",
    "Byla ocekavana dvojtecka.",
    "Byl ocekavany strednik.",
    "Redeklarace promenne.",
    "Byl ocekavany typ promenne.",
    "Byla ocekavana leva zavorka.",
    "Byla ocekavana prava zavorka.",
    "Byl ocekavany bool vyraz.",
    "Byl ocekavany 'then'.",
    "Byl ocekavany 'else'.",
    "Byl ocekavany 'do'.",
    "Bylo ocekavane prirazeni.",
    "Byl ocekavany prikaz.",
    "Byla ocekavana carka.",
    "Byla ocekavany vyraz nebo funkce 'find()' nebo funkce 'sort()'.",
    "Nebyl ocekavany string.",
    "Byl ocekavany identifikator prava leva zavorka.",
    "Byl ocekavany vyraz nebo prava zavorka."
};

/**
 * Hlavni funkce programu :)
 * @param int pocet argumentu
 * @param char*[] pole retezcu, ktere obsahuje argumenty
 */
int main(int argc, char *argv[]) {
    error = NONE_ERROR;
    // pokud je zadany nazev souboru
    if (argc == 2) {
        input = fopen(argv[1], "r");
        // pokud se podarilo soubor otevrit
        if (input) {
            // inicializace tabulky
            h_table = htableInit();
            // inicializace seznamu instrukci
            iListInit(&tripleAddressL);
            // inicializace predavaneho tokenu
            parserToken.code = T_IDDLE;
            parserToken.key = strInit();
            // jen kdyz probehl parser ok muze pracovat interpret
            if(parser()) {
                interprate(&tripleAddressL);
            }
            fclose(input);
            iListDispose(&tripleAddressL);
            htableFree(h_table);
            strFree(parserToken.key);
        }
        else {
            error = OTHER_ERROR;
            errorMsg = FILE_MSG;
        }
    }
    else {
        error = OTHER_ERROR;
        errorMsg = ARG_MSG;
    }

    if (error) {
        fprintf(stderr, "Chyba: %s\n", messages[errorMsg]);
    }
    return error;
}
