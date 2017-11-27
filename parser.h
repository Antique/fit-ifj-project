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
 *   Soubor: parser.h                                                        *
 *                                                                           *
 *   Popis: Obsahuje definice funkcí pro syntaktickou analýzu.               *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SYNTAX_H
#define	SYNTAX_H

// vycet akci po kontrole v precedentni tabulce
typedef enum {
    SRigh,
    Sleft,
    SErro,
    SEqua,
} precedentKey;

/**
 * Funkce volana pro spusteni syntakticke analyzy
 * @return bool informace o provedeni
 */
bool parser();

/**
 * Funkce reprezentujici nonterminal <program>
 * @return bool informace o provedeni
 */
bool parserProgram();

/**
 * Funkce reprezentujici nonterminal <declrList>
 * @return bool informace o provedeni
 */
bool parserDeclrList();

/**
 * Funkce reprezentujici nonterminal <declare>
 * @return bool informace o provedeni
 */
bool parserDeclare();

/**
 * Funkce reprezentujici nonterminal <dataType>
 * @param string* ukazatel na nazev promenne
 * @return bool informace o provedeni
 */
bool parserDataType(string* tempString);

/**
 * Funkce reprezentujici nonterminal <stateList>
 * @return bool informace o provedeni
 */
bool parserStateList();

/**
 * Funkce reprezentujici nonterminal <stateList2>
 * @return bool informace o provedeni
 */
bool parserStateList2();

/**
 * Funkce reprezentujici nonterminal <state>
 * @return bool informace o provedeni
 */
bool parserState();

/**
 * Funkce reprezentujici nonterminal <value>
 * @param htableItem* ukazatel promennou do ktere se prirazuje
 * @return bool informace o provedeni
 */
bool parserValue(htableItem* tempState);

/**
 * Funkce reprezentujici nonterminal <idList>
 * @return bool informace o provedeni
 */
bool parserIdList();

/**
 * Funkce reprezentujici nonterminal <idList2>
 * @return bool informace o provedeni
 */
bool parserIdList2();

/**
 * Funkce reprezentujici nonterminal <exprList>
 * @return bool informace o provedeni
 */
bool parserExprList();

/**
 * Funkce reprezentujici nonteminal <exprList2>
 * @return bool informace o provedeni
 */
bool parserExprList2();

/**
 * Funkce reprezentujici nonterminal <strParam>
 * @return bool informace o provedeni
 */
bool parserStrParam();

/**
 * Funkce reprezentujici nonterminal <expression>
 * @return bool informace o provedeni
 */
bool parserExpresionAnalize();

#endif	/* SYNTAX_H */
