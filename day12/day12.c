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
                          size_t stateCount,
                          int zeroIndex,
                          int64_t generationCount)
{
    char *newState = malloc(sizeof(char) * stateCount);
    char *currentState = malloc(sizeof(char) * stateCount);
    memcpy(currentState, initialState, sizeof(char) * stateCount);
    
    int64_t lastStateSum = 0;
    int64_t lastDelta = 0;
    for(int64_t g = 1; g < generationCount + 1; g++)
    {
        int64_t currentStateSum = 0;
        for(int i = 2; i < stateCount - 2; i++)
        {
            char *check = currentState + i - 2;
            int ruleIndex = calculateRuleIndex(check);
            char ruleResult = ruleResultMap[ruleIndex];
            newState[i] = ruleResult;
            currentStateSum += (ruleResult == '#') ? (i - zeroIndex) : 0;
        }
        
        int64_t d = currentStateSum - lastStateSum;
        if(lastDelta == d)
        {
            int64_t gDelta = generationCount - g;
            lastStateSum = currentStateSum + (d * gDelta);
            break;
        }
        
        char *tmp = currentState;
        currentState = newState;
        newState = tmp;
        lastStateSum = currentStateSum;
        lastDelta = d;
    }
    
    free(currentState);
    free(newState);
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
    char state[256];
    fscanf(file, "initial state: %s\n\n", state);

    //Initialize initial state
    size_t stateCount = strlen(state);
    size_t initialStateCount = stateCount + 5000;
    int zeroIndex = 4;
    char *initialState = malloc(sizeof(char) * initialStateCount);
    memset(initialState, '.', sizeof(char) * initialStateCount);
    memcpy(initialState + zeroIndex, state, stateCount);
    
    //Initialize rule result map
    char ruleResultMap[32];
    memset(ruleResultMap, '.', 32);
    
    //Parse rules
    char rule[6];
    char ruleResult;
    while(fscanf(file, "%s => %c", rule, &ruleResult) == 2)
    {
        int ruleIndex = calculateRuleIndex(rule);
        ruleResultMap[ruleIndex] = ruleResult;
    }
    fclose(file);

    //Find state sum after twenty generations
    int64_t lastStateSumTwenty = calculateStateSum(ruleResultMap,
                                                   initialState,
                                                   initialStateCount,
                                                   zeroIndex,
                                                   20);
    printf("part1: state sum after twenty = %lld\n", lastStateSumTwenty);
    
    //Find state sum after fifty billion generations
    int64_t lastStateSumFiftyBillion = calculateStateSum(ruleResultMap,
                                                         initialState,
                                                         initialStateCount,
                                                         zeroIndex,
                                                         50000000000);
    printf("part2: state sum after fifty billion = %lld\n", lastStateSumFiftyBillion);
    
    //Finished
    return 0;
}
