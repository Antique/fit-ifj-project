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
 *   Soubor: interpre.h                                                      *
 *                                                                           *
 *   Popis: Obsahuje definice enumu a struktur pro seznam instrukcí a        *
 *          definice potøebných funkcí pro práci s nimi.                     *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef INTERPRET_H
#define	INTERPRET_H

// vycet moznych instrukci
typedef enum {
    I_ADD = 0,          // add   src1    src2    dest
    I_SUB,              // sub   src1    src2    dest
    I_MUL,              // mul   src1    src2    dest
    I_DIV,              // div   src1    src2    dest
    I_LESS,             // les   src1    src2    dest
    I_LESSOREQUAL = 5,  // leseq src1    src2    dest
    I_GREATER,          // grt   src1    src2    dest
    I_GREATEROREQUAL,   // grteq src1    src2    dest
    I_DIFFERENT,        // dif   src1    src2    dest
    I_SAME,             // eq    src1    src2    dest
    I_MOV = 10,         // mov   src1     --     dest
    I_NOT,              // not   src1     --     dest
    I_READ,             // read   --      --     dest
    I_CLEAR,            // clear  --      --      --
    I_WRITE,            // write src1     --      --
    I_GOTO = 15,        // goto  [src1]   --     dest
    I_SORT,             // sort  src1     --     dest
    I_FIND,             // find  src1    src2    dest
    I_LAB,              // lab    --      --      --
    I_ENDDOT = 19       // end    --      --      --
} iInstruct;

// polozka seznamu instrukci
typedef struct iListItem {
    iInstruct instruction;
    struct sHtableItem* src1;
    struct sHtableItem* src2;
    struct sHtableItem* dest;
    struct iListItem *next;
} *iListItemPtr;

// seznam instrukci
typedef struct {
    iListItemPtr first;
    iListItemPtr act;
    iListItemPtr last;
} iList;

/**
 * Provede inicializaci seznamu instrukci.
 * @param iList ukazatel na seznam instrukci
 */
void iListInit(iList *);

/**
 * Provede smazani seznamu instrukci.
 * @param iList ukazatel na seznam instrukci
 */
void iListDispose(iList *);

/**
 * Vlozi novou instrukci na konec seznamu.
 * @param iList ukazatel na seznam instrukci
 * @param iInstruct instrukce
 * @param htableItem* prvni zdrojova adresa
 * @param htableItem* druha zdrojova adresa
 * @param htableItem* cilova adresa
 * @return bool vrati informaci o spravnem ulozeni instrukce
 */
bool iInsertInstruction(iList *, iInstruct ins, htableItem* op1, htableItem* op2, htableItem* prod);

/**
 * Vlozi novou instrukci na konec seznamu.
 * @param iList ukazatel na seznam instrukci
 * @return bool vrati informaci o spravnem provedeni iterpretu
 */
bool interprate(iList *L);

#endif
