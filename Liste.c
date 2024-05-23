#include <stdlib.h>
#include <stdio.h>

#include "Liste.h"

#define taille_bloc 5

struct SCell {
    Data value;
    SCell *prev_cell;
    SCell *next_cell;
};

struct SList {
    SCell *first;
    SCell *last;
    SBlock *current_block;
    int current_index;
    // Recyclage
    SCell *recycle_first;
};

struct SBlock {
    SBlock *prev;
    SCell cells[taille_bloc];
};

SCell* CreateCell(SList *list) {
    // Si une cell peut etre recyclée, on l'utilise en priorité
    if (list->recycle_first != NULL) {
        SCell *recycled_cell =  list->recycle_first;
        list->recycle_first = recycled_cell->next_cell;
        // printf("Recycling a cell, old_value = %d\n", recycled_cell->value);
        return recycled_cell;
    }

    // si le bloc actuel n'a plus de cellule vides, on en crée un nouveau
    if (list->current_index == taille_bloc) {
        // printf("Creating a new block\n");
        SBlock *new_block = (SBlock*) malloc(sizeof(SBlock));
        new_block->prev = list->current_block;
        list->current_index = 0;
        list->current_block = new_block;
    }

    // on renvoit l'adresse de la première cellule allouée non utilisée
    list->current_index++;
    return &(list->current_block->cells[list->current_index-1]);
}

SList* CreateList() {
    SList *list = (SList*) malloc(sizeof(SList));
    SBlock *new_block = (SBlock*) malloc(sizeof(SBlock));
    new_block->prev = NULL;
    list->first = NULL;
    list->last = NULL;
    list->current_index = 0;
    list->current_block = new_block;
    list->recycle_first = NULL;
    return list;
}

void DeleteList(SList *list) {
    while (list->current_block != NULL) {
        SBlock *nextDeleteBlock = list->current_block->prev;
        free(list->current_block);
        list->current_block = nextDeleteBlock;
    }
    free(list);
}

SCell* AddElementBegin(SList *list,Data elem) {
    SCell *new_cell = (SCell*) CreateCell(list);
    new_cell->value = elem;
    new_cell->prev_cell = NULL;
    new_cell->next_cell = list->first;
    
    if (list->first != NULL) {
        // si liste vide
        list->first->prev_cell = new_cell;
    } else {
        // si liste non vide
        list->last = new_cell;
    }
    list->first = new_cell;
    
    return new_cell;
}

SCell* AddElementEnd(SList *list,Data elem) {
    SCell *new_cell = CreateCell(list);
    new_cell->value = elem;
    new_cell->prev_cell = list->last;
    new_cell->next_cell = NULL;
    
    if (list->last != NULL) {
        // si liste vide
        list->last->next_cell = new_cell;
    } else {
        // si liste non vide
        list->first = new_cell;
    }
    list->last = new_cell;

    return new_cell;
}

SCell* AddElementAfter(SList *list, SCell *cell, Data elem) {
    SCell *new_cell = CreateCell(list);
    new_cell->value = elem;
    new_cell->prev_cell = cell->prev_cell;
    new_cell->next_cell = cell->next_cell;

    if (cell->next_cell != NULL) {
        cell->next_cell->prev_cell = new_cell;
    } else {
        list->last = new_cell;
    }
    cell->next_cell = new_cell;

    return new_cell;
}

void DeleteCell(SList *list,SCell *cell) {
    if (cell->prev_cell != NULL) {
        cell->prev_cell->next_cell = cell->next_cell;
    } else {
        list->first = cell->next_cell;
    }
    if (cell->next_cell != NULL) {
        cell->next_cell->prev_cell = cell->prev_cell;
    } else {
        list->last = cell->prev_cell;
    }

    // au lieu de free la cellule, on la place sur la pile de recyclage
    cell->next_cell = list->recycle_first;
    list->recycle_first = cell;
}

SCell* GetFirstElement(SList *list) {
    return list->first;
}

SCell* GetLastElement(SList *list) {
    return list->last;
}

SCell* GetPrevElement(SCell *cell) {
    return cell->prev_cell;
}

SCell* GetNextElement(SCell *cell) {
    return cell->next_cell;
}

Data GetData(SCell *cell) {
    return cell->value;
}