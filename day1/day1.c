//
//  day1.c
//  day1
//
//  Created by Brandon Holland on 2018-12-01.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node node;
struct node
{
    int key;
    int val;
    node *next;
};

typedef struct table table;
struct table
{
    int size;
    node **list;
};

table *createTable(int size)
{
    table *t = (table*)malloc(sizeof(table));
    t->size = size;
    t->list = (node**)malloc(sizeof(node*) * size);
    
    for(int i = 0; i < size; i++)
    { t->list[i] = NULL; }
    
    return t;
}

int hashCode(table *t, int key)
{
    if(key < 0)
    { return -(key % t->size); }
    return key % t->size;
}

void tableInsert(table *t, int key, int val)
{
    int pos = hashCode(t, key);
    node *list = t->list[pos];
    node *newNode = (node*)malloc(sizeof(node));
    node *temp = list;
    while(temp)
    {
        if(temp->key == key)
        {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}

int tableLookup(struct table *t, int key)
{
    int pos = hashCode(t, key);
    node *list = t->list[pos];
    node *temp = list;
    while(temp)
    {
        if(temp->key==key)
        { return temp->val; }
        temp = temp->next;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day1 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    int *deltas = malloc(sizeof(int));
    int deltasCount = 0;
    int total = 0;
    
    //Parse deltas
    FILE *file = fopen(argv[1], "r");
    int delta = 0;
    while(fscanf(file, "%d", &delta) == 1)
    {
        //Add delta
        deltasCount++;
        deltas = realloc(deltas, sizeof(int) * deltasCount);
        deltas[deltasCount - 1] = delta;
        
        //Update total
        total += delta;
    }
    fclose(file);
    printf("part1: total = %d\n", total);
    
    //Initialize totals
    struct table *totalsTable = createTable(deltasCount);
    
    //Reset total and loop forever until double total is found
    total = 0;
    for(;;)
    {
        for(int i = 0; i < deltasCount; i++)
        {
            delta = deltas[i];
            total += delta;
            
            if (tableLookup(totalsTable, total) == 1)
            {
                printf("part2: first double total = %d\n", total);
                free(deltas);
                free(totalsTable);
                exit(0);
            }
            tableInsert(totalsTable, total, 1);
        }
    }
    return 0;
}
