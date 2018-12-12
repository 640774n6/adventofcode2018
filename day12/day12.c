//
//  day12.c
//  day12
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

unsigned int calculateRuleIndex(char *rule)
{
    unsigned int ruleIndex = 0;
    for(int i = 0; i < 5; i++)
    {
        ruleIndex <<= 1;
        if(rule[i] == '#')
        { ruleIndex |= 1; }
    }
    return ruleIndex;
}

int64_t calculateStateSum(char *ruleResultMap,
                          char *initialState,
                          int initialStateCount,
                          int64_t generationCount)
{
    //Initialize variables
    int firstPotId = -4;
    int workingStateCount = initialStateCount + 8;
    int64_t lastStateSum = 0;
    int64_t lastDelta = 0;
    
    //Initialize current state
    char *currentState = malloc(sizeof(char) * workingStateCount);
    memset(currentState, '.', sizeof(char) * workingStateCount);
    memcpy(currentState + 4, initialState, sizeof(char) * initialStateCount);
    
    //Initialize new state
    char *newState = malloc(sizeof(char) * workingStateCount);
    memset(newState, '.', sizeof(char) * workingStateCount);
    
    //Start generations
    for(int64_t g = 1; g < generationCount + 1; g++)
    {
        //Get next generation
        int64_t newStateSum = 0;
        int firstPlant = 0;
        int newStateIndex = 4;
        for(int i = 2; i < workingStateCount - 2; i++)
        {
            //Get rule for current index
            //Calculate rule index using rule
            //Get rule result from map using rule index
            char *rule = currentState + i - 2;
            int ruleIndex = calculateRuleIndex(rule);
            char ruleResult = ruleResultMap[ruleIndex];

            //If pot results in a plant
            if(ruleResult == '#')
            {
                //If this is the first plant for this generation
                if(!firstPlant)
                {
                    //Toggle first plant and reset first pot id
                    firstPlant = 1;
                    firstPotId += i - newStateIndex;
                }
                
                //Update new state sum and new state
                int newPlantId = (firstPotId + newStateIndex);
                newStateSum += newPlantId;
                newState[newStateIndex] = ruleResult;
            }
            
            //Increment new state index only if we've already found the first plant
            newStateIndex += firstPlant ? 1 : 0;
        }
        
        //If last state sum delta is the same, we have stabilized
        int64_t d = newStateSum - lastStateSum;
        if(lastDelta == d)
        {
            //Use stablized delta to calculate final state sum
            int64_t gDelta = generationCount - g;
            lastStateSum = newStateSum + (d * gDelta);
            break;
        }
        
        //If either of the last 2 pots have plants
        if(newState[workingStateCount - 3] == '#' ||
           newState[workingStateCount - 4] == '#')
        {
            //Grow working state count
            workingStateCount += 2;
            
            //Realloc and initialize new state pots
            newState = realloc(newState, sizeof(char) * workingStateCount);
            memset(newState + workingStateCount - 2, '.', sizeof(char) * 2);
            
            //Realloc current state pots
            currentState = realloc(currentState, sizeof(char) * workingStateCount);
        }
        
        //Swap current for new and reset new
        char *swap = currentState;
        currentState = newState;
        newState = swap;
        memset(newState, '.', sizeof(char) * workingStateCount);
        
        //Update last state sum & delta
        lastStateSum = newStateSum;
        lastDelta = d;
    }
    
    //Cleanup
    free(currentState);
    free(newState);
    
    //Return last state sum
    return lastStateSum;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day12 <INPUT>\n");
        exit(0);
    }
    
    //Parse initial state
    FILE *file = fopen(argv[1], "r");
    char initialState[256];
    fscanf(file, "initial state: %s\n\n", initialState);
    
    //Get initial state count
    int initialStateCount = (int)strlen(initialState);
    
    //Initialize rule result map
    char ruleResultMap[32];
    memset(ruleResultMap, '.', 32);
    
    //Parse rules
    char rule[6];
    char ruleResult;
    while(fscanf(file, "%s => %c", rule, &ruleResult) == 2)
    {
        if(ruleResult == '#')
        {
            int ruleIndex = calculateRuleIndex(rule);
            ruleResultMap[ruleIndex] = ruleResult;
        }
    }
    fclose(file);
    
    //Find state sum after twenty generations
    int64_t lastStateSumTwenty = calculateStateSum(ruleResultMap,
                                                   initialState,
                                                   initialStateCount,
                                                   20);
    printf("part1: state sum after twenty = %lld\n", lastStateSumTwenty);
    
    //Find state sum after fifty billion generations
    int64_t lastStateSumFiftyBillion = calculateStateSum(ruleResultMap,
                                                         initialState,
                                                         initialStateCount,
                                                         50000000000);
    printf("part2: state sum after fifty billion = %lld\n", lastStateSumFiftyBillion);
    
    //Finished
    return 0;
}
