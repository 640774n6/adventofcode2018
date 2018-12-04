//
//  day2.c
//  day2
//
//  Created by Brandon Holland on 2018-12-01.
//  Copyright © 2018 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define BOX_ID_LEN  27

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day1_2 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    int *charCountMap = malloc(sizeof(int) * 26);
    char **boxIds = malloc(sizeof(char *));
    int boxIdsCount = 0;
    int x2Counter = 0;
    int x3Counter = 0;
    
    //Parse box ids
    FILE *file = fopen(argv[1], "r");
    char newBoxId[BOX_ID_LEN + 1];
    while(fscanf(file, "%s", newBoxId) == 1)
    {
        //Add box id
        boxIdsCount++;
        boxIds = realloc(boxIds, sizeof(char *) * boxIdsCount);
        boxIds[boxIdsCount - 1] = malloc(sizeof(char) * BOX_ID_LEN + 1);
        memcpy(boxIds[boxIdsCount - 1], newBoxId, sizeof(char) * BOX_ID_LEN + 1);
        
        //Reset and populate char count map for box id
        memset(charCountMap, 0, sizeof(int) * 26);
        for(char *c = newBoxId; *c != '\0'; c++)
        {
            int i = *c - 'a';
            charCountMap[i]++;
        }
        
        //Get x2 and x3 values from char count map for box id
        int x2 = 0;
        int x3 = 0;
        for(int i = 0; i < 26; i++)
        {
            if (charCountMap[i] == 2)
            { x2 = 1; }
            else if (charCountMap[i] == 3)
            { x3 = 1; }
        }
        
        //Update x2 and x3 counters
        x2Counter += x2;
        x3Counter += x3;
    }
    fclose(file);
    
    //Calculate checksum
    int checksum = x2Counter * x3Counter;
    printf("part1: checksum = %d\n", checksum);
    
    //Find and trim deltas from box id with least number of deltas
    int minDelta = INT_MAX;
    char minDeltaTrimmedBoxId[BOX_ID_LEN + 1];
    for(int i = 0; i < boxIdsCount; i++)
    {
        char *boxId1 = boxIds[i];
        for(int j = 0; j < boxIdsCount; j++)
        {
            if(i == j)
            { continue; }
            
            char *boxId2 = boxIds[j];
            char trimmedBoxId[BOX_ID_LEN + 1];
            int trimmedBoxIdIndex = 0;
            int delta = 0;
            
            for(int k = 0; k < BOX_ID_LEN; k++)
            {
                if(boxId1[k] != boxId2[k])
                {
                    delta++;
                    continue;
                }
                
                trimmedBoxId[trimmedBoxIdIndex] = boxId1[k];
                trimmedBoxIdIndex++;
            }
            trimmedBoxId[trimmedBoxIdIndex] = '\0';
            
            if (minDelta > delta)
            {
                minDelta = delta;
                memcpy(minDeltaTrimmedBoxId, trimmedBoxId, sizeof(char) * BOX_ID_LEN + 1);
            }
        }
    }
    printf("part2: min delta trimmed box id = %s\n", minDeltaTrimmedBoxId);
    
    //Cleanup and return
    for(int i = 0; i < boxIdsCount; i++)
    { free(boxIds[i]); }
    
    free(boxIds);
    free(charCountMap);
    return 0;
}
