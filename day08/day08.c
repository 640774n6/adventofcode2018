//
//  day08.c
//  day08
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct node node;
struct node
{
    node **children;
    int childrenCount;
    int *metaData;
    int metaDataCount;
};

node *createNewNode(int c, int m)
{
    node *newNode = malloc(sizeof(node));
    memset(newNode, 0, sizeof(node));
    newNode->children = malloc(sizeof(node *) * c);
    newNode->childrenCount = c;
    newNode->metaData = malloc(sizeof(int) * m);
    newNode->metaDataCount = m;
    return newNode;
}

node *parseNode(int *n, int *i, int *t)
{
    //Create new node using next 2 numbers
    node *newNode = createNewNode(n[*i], n[*i + 1]);
    *i += 2;
    
    //Parse child nodes
    for(int c = 0; c < newNode->childrenCount; c++)
    {
        node *childNode = parseNode(n, i, t);
        newNode->children[c] = childNode;
    }
    
    //Parse meta data
    for(int m = 0; m < newNode->metaDataCount; m++)
    {
        int meta = n[*i];
        newNode->metaData[m] = meta;
        *t += n[*i];
        *i += 1;
    }
    
    //Return new node
    return newNode;
}

int getNodeValue(node *n)
{
    //The value
    int value = 0;
    
    //If node doesn't have any child nodes
    if(n->childrenCount == 0)
    {
        //Add up meta data
        for(int m = 0; m < n->metaDataCount; m++)
        { value += n->metaData[m]; }
        
        //Return the value
        return value;
    }
    
    //Iterate through meta data
    for(int m = 0; m < n->metaDataCount; m++)
    {
        //Get child node from meta data index if index is in range
        int c = n->metaData[m] - 1;
        if(c >= 0 && c < n->childrenCount)
        {
            //Add child node's value to value
            node *childNode = n->children[c];
            value += getNodeValue(childNode);
        }
    }
    
    //Return the value
    return value;
}

void freeNode(node *n)
{
    //Free child nodes
    for(int c = 0; c < n->childrenCount; c++)
    {
        node *childNode = n->children[c];
        freeNode(childNode);
    }
    
    //Free children and meta data arrays
    free(n->children);
    free(n->metaData);
    
    //Free node
    free(n);
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day08 <INPUT>\n");
        exit(0);
    }
    
    //Initialize number list
    int *numbers = malloc(sizeof(int));
    int numbersCount = 0;
    
    //Parse numbers
    FILE *file = fopen(argv[1], "r");
    int number;
    while(fscanf(file, "%d", &number) == 1)
    {
        numbersCount++;
        numbers = realloc(numbers, sizeof(int) * numbersCount);
        numbers[numbersCount - 1] = number;
    }
    fclose(file);
    
    //Populate nodes and total up meta data
    int startIndex = 0;
    int metaDataTotal = 0;
    node *rootNode = parseNode(numbers, &startIndex, &metaDataTotal);
    printf("part1: total of meta data = %d\n", metaDataTotal);
    
    //Get root node value
    int rootNodeValue = getNodeValue(rootNode);
    printf("part2: root node value = %d\n", rootNodeValue);
    
    //Cleanup
    freeNode(rootNode);
    return 0;
}
