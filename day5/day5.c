//
//  day5.c
//  day5
//
//  Created by Brandon Holland on 2018-12-03.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int reactPolymer(char **p, size_t s)
{
    int fullReactionUnits = (int)s;
    for(char *cl = *p; *cl != '\0'; cl++)
    {
        char *cr = cl + 1;
        if(*cr == '\0')
        { break; }
        
        int d = abs(*cl - *cr);
        if(d == 32)
        {
            memmove(cl, (cr + 1), fullReactionUnits - (cr - *p));
            fullReactionUnits -= 2;
            cl -= 2;
        }
    }
    return fullReactionUnits;
}

int main(int argc, char * argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day5 <INPUT>\n");
        exit(0);
    }
    
    //Get polymer size
    FILE *file = fopen(argv[1], "r");
    fseek(file, 0, SEEK_END);
    size_t polymerSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    //Load polymer into memory
    char *polymer = malloc(polymerSize + 1);
    fread(polymer, polymerSize, 1, file);
    polymer[polymerSize] = '\0';
    fclose(file);
    
    //Copy polymer for full reaction
    char *fullReactionPolymer = malloc(polymerSize + 1);
    memcpy(fullReactionPolymer, polymer, polymerSize + 1);
    
    //React full polymer and find remaining units
    int fullReactionUnits = reactPolymer(&fullReactionPolymer, polymerSize);
    printf("part1: full reaction remaining units = %d\n", fullReactionUnits);
    
    //Cleanup full
    free(fullReactionPolymer);
    
    //Find min reaction remaining units
    int minPartReactionUnits = INT_MAX;
    for(char u = 'A'; u <= 'Z'; u++)
    {
        char l = u + 32;
        
        char *partReactionPolymer = malloc(polymerSize + 1);
        memcpy(partReactionPolymer, polymer, polymerSize + 1);
        
        int partReactionPolymerSize = (int)polymerSize;
        for(int i = 0; i < polymerSize; i++)
        {
            if(partReactionPolymer[i] == u ||
               partReactionPolymer[i] == l)
            {
                memmove(&partReactionPolymer[i],
                        &partReactionPolymer[i + 1],
                        partReactionPolymerSize - i);
                partReactionPolymerSize--;
                i--;
            }
        }
        
        int partReactionUnits = reactPolymer(&partReactionPolymer, partReactionPolymerSize);
        minPartReactionUnits = MIN(minPartReactionUnits, partReactionUnits);
        free(partReactionPolymer);
    }
    printf("part2: min reaction remaining units = %d\n", minPartReactionUnits);
    
    //Cleanup
    free(polymer);
    return 0;
}
