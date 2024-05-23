#ifndef _LISTE_H
#define _LISTE_H

struct Message {
    char contenu[250];
    char destinataire[2];
};

typedef struct Message Message;
typedef Message* Data;
typedef struct SCell SCell;
typedef struct SList SList;
typedef struct SBlock SBlock;

SList* CreateList();
void DeleteList(SList *list);

SCell* AddElementBegin(SList *list,Data elem);
SCell* AddElementEnd(SList *list,Data elem);
SCell* AddElementAfter(SList *list,SCell *cell,Data elem);
void DeleteCell(SList *list,SCell *cell);

SCell* GetFirstElement(SList *list);
SCell* GetLastElement(SList *list);
SCell* GetPrevElement(SCell *cell);
SCell* GetNextElement(SCell *cell);
Data GetData(SCell *cell);

#endif