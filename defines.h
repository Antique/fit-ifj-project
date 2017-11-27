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
 *   Soubor: defines.h                                                       *
 *                                                                           *
 *   Popis: Obsahuje definice chybových stavù a zpráv.                       *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DEFINES_H
#define	DEFINES_H

// vycet chybovych stavu
typedef enum {NONE_ERROR, LEX_ERROR, SYN_ERROR, SEM_ERROR, INT_ERROR, OTHER_ERROR} progErrors;

// vycet chybovych zprav
typedef enum {
    NONE_MSG,                       // zadna chyba
    ARG_MSG,                        // chybny pocet argumentu
    FILE_MSG,                       // nelze otevrit soubor
    MEM_MSG,                        // nedostatek pameti
    DIV_BY_ZERO_MSG,                // deleni nulou
    INT_EXPECT_MSG,                 // ocekavany int
    DBL_EXPECT_MSG,                 // ocekavany double
    EOF_UNEXPECT_MSG,               // neocekavany eof
    UNKNOWN_CHAR_MSG,               // nepodporovany znak
    BAD_NUM_MSG,                    // chybne zadane cislo
    ID_UNEXPECT_MSG,                // neocekavany identifikator
    NUM_OVERFLOW_MSG,               // preteceni velikosti cisla
    STR_EXPECT_MSG,                 // ocekavany string
    BAD_ESCAPE_MSG,                 // chybna escape sekvence
    BAD_CHAR_MSG,                   // nepodporovany znak v retezci
    ID_UNDECLARED_MSG,              // nedeklarovana promenna
    TOKEN_UNEXPECT_MSG,             // neocekavany token
    INCOMPAT_TYPE_MSG,              // nekompatibilni typ promennych a konstant
    BAD_EXPRESSION_MSG,             // chybny vyraz
    BEGIN_EXPECT_MSG,               // ocekavany begin
    END_EXPECT_MSG,                 // ocekavany end
    DOT_EXPECT_MSG,                 // ocekavana tecka
    EOF_EXPECT_MSG,                 // ocekavany konec souboru
    VAR_OR_BEGIN_EXPECT_MSG,        // ocekavany var nebo begin
    ID_EXPECT_MSG,                  // ocekavana promenna
    DBLDOT_EXPECT_MSG,              // ocekavana dvojtecka
    SEMCOL_EXPECT_MSG,              // ocekavany strednik
    REDECLARE_MSG,                  // redeklarace
    DECLARE_TYPE_EXPECTED_MSG,      // ocekavany typ promenne
    LEFT_BAR_EXPECT_MSG,            // ocekavana leva zavorka
    RIGHT_BAR_EXPECT_MSG,           // ocekavana prava zavorka
    INT_EXPR_EXPECT_MSG,            // ocekavany bool vyraz
    THEN_EXPECT_MSG,                // ocekavany then
    ELSE_EXPECT_MSG,                // ocekavany else
    DO_EXPECT_MSG,                  // ocekavany do
    ASSIG_EXPECT_MSG,               // ocekavane prirazeni
    STATE_EXPECT_MSG,               // ocekavany prikaz
    COMA_EXPECT_MSG,                // ocekavana carka
    RIGHT_SIDE_EXPECT_MSG,          // ocekavany vyraz nebo find nebo sort
    STR_UNEXPECT_MSG,               // neocekavany string
    ID_OR_RIGHT_BAR_EXPECT_MSG,     // ocekavana promenna nebo prava zavorka
    EXPR_OR_RIGHT_BAR_EXPECT_MSG    // ocekavany vyraz nebo prava zavorka
} errorMessages;

#endif
